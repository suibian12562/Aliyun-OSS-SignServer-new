#include "Poco/URI.h"
#include "Poco/FileStream.h" 
#include "Poco/Net/HTTPServer.h"                //继承自TCPServer 实现了一个完整的HTTP多线程服务器
#include "Poco/Net/HTTPRequestHandler.h"        //抽象基类类 被HttpServer所创建 用来处理Http的请求
#include "Poco/Net/HTTPRequestHandlerFactory.h" //HTTPRequestHandler的工厂 给予工厂设计模式
#include "Poco/Net/HTTPServerParams.h"          //被用来指定httpserver以及HTTPRequestHandler的参数
#include "Poco/Net/HTTPServerRequest.h"         //ServerRequest的抽象子类用来指定 服务器端的 http请求
#include "Poco/Net/HTTPServerResponse.h"        //ServerResponse的抽象子类用来指定服务器端的http响应
#include "Poco/Util/ServerApplication.h"        //Application的子类 所有服务器程序 包括 Reactor FTP HTTP等都用到 算是服务器的启动类
#include "Poco/Net/HTTPRequestHandler.h"        //抽象基类类 被HttpServer所创建 用来处理Http的请求
#include "Poco/Util/Option.h"                   //存储了命令行选项
#include "Poco/Util/OptionSet.h"                //一个Opention对象的集合
#include "Poco/Util/HelpFormatter.h"            //从OptionSet格式化帮助信息
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/Semaphore.h"
#include "Poco/Thread.h"


#include <sqlite3.h>

#include "alibabacloud/oss/OssClient.h" // 阿里云 OSS C++ SDK 头文件

#include <fstream>
#include <iostream>
#include <cstdlib>

Poco::Semaphore semaphore_db(1); 


struct message_info
{
    std::string _Endpoint;
    std::string _Bucket;
    std::string _GetobjectUrlName;
    std::string _GenedUrl;
    long _request_time;

    Poco::JSON::Object::Ptr toJSON() const
    {
        Poco::JSON::Object::Ptr pObj = new Poco::JSON::Object;
        pObj->set("Endpoint", _Endpoint);
        pObj->set("Bucket", _Bucket);
        pObj->set("GetobjectUrlName", _GetobjectUrlName);
        pObj->set("GenedUrl", _GenedUrl);
        pObj->set("request_time", _request_time);
        return pObj;
    }
};

struct Config
{
    std::string AccessKeyId;
    std::string AccessKeySecret;
    int sign_time;
    int port;
};

class SQLiteCacheManager
{
private:
    sqlite3 *db;

public:
    SQLiteCacheManager()
    {   
        semaphore_db.wait();
        int rc = sqlite3_open("cache.db", &db);
        if (rc)
        {
            std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
        }
        else
        {
            std::cout << "Opened SQLite database successfully" << std::endl;
        }
        semaphore_db.set();
    }

    ~SQLiteCacheManager()
    {
        semaphore_db.wait();
        sqlite3_close(db);
        semaphore_db.set();
    }

    int deleteFromCache(const std::string &getObjectUrlName)
    {
        std::string sql = "DELETE FROM Cache WHERE GetobjectUrlName = ?";
        sqlite3_stmt *stmt;

        semaphore_db.wait();

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
            semaphore_db.set();
            return 0;
        }

        sqlite3_bind_text(stmt, 1, getObjectUrlName.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
            semaphore_db.set();
            return 0;
        }

        sqlite3_finalize(stmt);
        semaphore_db.set();
        return 1;
    }

    int saveToCache(const std::string &getObjectUrlName, const std::string &genedUrl, long requestTime, long cacheDuration)
    {
        long expirationTime = requestTime + cacheDuration;
        std::string sql = "INSERT OR REPLACE INTO Cache (GetobjectUrlName, GenedUrl, RequestTime, ExpirationTime) VALUES (?, ?, ?, ?)";
        sqlite3_stmt *stmt;

        semaphore_db.wait();

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
            semaphore_db.set();
            return 0;
        }

        sqlite3_bind_text(stmt, 1, getObjectUrlName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, genedUrl.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 3, requestTime);
        sqlite3_bind_int64(stmt, 4, expirationTime);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
            semaphore_db.set();
            return 0;
        }

        sqlite3_finalize(stmt);
        semaphore_db.set();
        return 1;
    }

    bool getFromCache(const std::string &getObjectUrlName, std::string &genedUrl, const long &requestTime)
    {
        if (getObjectUrlName.empty())
        {
            std::cerr << "getObjectUrlName is empty." << std::endl;
            return false;
        }

        std::string sql = "SELECT GenedUrl, RequestTime, ExpirationTime FROM Cache WHERE GetobjectUrlName = ?";
        sqlite3_stmt *stmt = nullptr; // 初始化为nullptr

        semaphore_db.wait();

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
            semaphore_db.set();
            return false;
        }

        rc = sqlite3_bind_text(stmt, 1, getObjectUrlName.c_str(), getObjectUrlName.size(), SQLITE_STATIC);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Error binding parameter: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            semaphore_db.set();
            return false;
        }

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            if (sqlite3_column_text(stmt, 0) != nullptr)
            {
                genedUrl = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
            }
            long expirationTime = sqlite3_column_int64(stmt, 2);
            if (expirationTime > requestTime)
            {
                sqlite3_finalize(stmt); // 在使用完stmt后释放资源
                semaphore_db.set();
                return true;
            }
            else
            {
                std::clog << "Data expired for getObjectUrlName: " << getObjectUrlName << std::endl;
                sqlite3_finalize(stmt);
                semaphore_db.set();
                //删除过期缓存
                deleteFromCache(getObjectUrlName);

                return false;
            }

            // requestTime = sqlite3_column_int64(stmt, 1);

        }
        else if (rc == SQLITE_DONE)
        {
            // std::cerr << "No data found for getObjectUrlName: " << getObjectUrlName << std::endl;
        }
        else
        {
            std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt); // 在所有返回路径上都确保释放stmt资源
        semaphore_db.set();
        return false;
    }
};

class RequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
    SQLiteCacheManager cacheManager;


public:
    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
};

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) // 覆盖方法
    {
        return new RequestHandler;
    }
};