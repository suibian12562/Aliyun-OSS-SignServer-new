﻿#include "main.h"
#include "MyHttpServer.h"
#include "SQLiteCacheManager.h"
#include "logger.h"

using namespace Poco::Net;
using namespace Poco::Util;
using Poco::Logger;
using namespace std;

extern const Config rconfig = readConfigFromFile("config.json");
const AlibabaCloud::OSS::ClientConfiguration conf;

// 启动web 服务器
POCO_SERVER_MAIN(MyHttpServer)


void createDefaultDatabase(const std::string& filename)
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", filename);

    
    if (int result = remove(filename.c_str());result == 0)
    {
        poco_information(logger_handle, "Deleted existing database");
    }
    else
    {
        poco_error(logger_handle, "No existing database to delete");
    }

    poco_information(logger_handle, "Creating database");

    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS Cache (GetobjectUrlName TEXT PRIMARY KEY, GenedUrl TEXT, RequestTime INTEGER, ExpirationTime INTEGER)";

    try
    {
        Poco::Data::Statement createTable(session);
        createTable << createTableSQL, Poco::Data::Keywords::now;
        poco_information(logger_handle, "Table created successfully");
    }
    catch (const Poco::Exception& ex)
    {
        poco_error(logger_handle, "Error creating table: " + ex.displayText());
    }

    Poco::Data::SQLite::Connector::unregisterConnector();
}


void createDefaultConfig(const std::string &filename)
{
    Config config;
    config.AccessKeyId = "your_access_key";
    config.AccessKeySecret = "your_access_secret";
    config.sign_time = 40;
    config.port = 8080;

    Poco::JSON::Object::Ptr jsonObj = new Poco::JSON::Object;
    jsonObj->set("AccessKeyId", config.AccessKeyId);
    jsonObj->set("AccessKeySecret", config.AccessKeySecret);
    jsonObj->set("sign_time", config.sign_time);
    jsonObj->set("port", config.port);

    std::ostringstream oss;
    Poco::JSON::Stringifier::stringify(jsonObj, oss, 4);

    try
    {
        Poco::FileOutputStream fileStream(filename);
        fileStream << oss.str();
        fileStream.close();
        std::cout << "config file created successfully" << std::endl;
        poco_information(logger_handle, "config file created successfully");
    }
    catch (Poco::Exception &e)
    {
        std::cerr << "create config failed" << e.displayText() << std::endl;
        poco_error(logger_handle, "create config failed");
    }
}
Config readConfigFromFile(const std::string &filename)
{
    Config _config;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Config file not found. Creating a default config file. Please configure and restart the program." << std::endl;
        poco_error(logger_handle, "Config file not found. Creating a default config file. Please configure and restart the program.");
        createDefaultConfig(filename);
        // Return the default config by reference
        exit(1);
    }

    try
    {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(file);
        Poco::JSON::Object::Ptr jsonObj = result.extract<Poco::JSON::Object::Ptr>();

        _config.AccessKeyId = jsonObj->getValue<std::string>("AccessKeyId");
        _config.AccessKeySecret = jsonObj->getValue<std::string>("AccessKeySecret");
        _config.sign_time = jsonObj->getValue<int>("sign_time");
        _config.port = jsonObj->getValue<int>("port");
    }
    catch (const Poco::Exception &e)
    {
        std::cerr << "Error: Unable to parse config JSON: " << e.displayText() << std::endl;
        poco_error(logger_handle, "Error: Unable to parse config JSON");
    }

    file.close();
    // Return the loaded config
    return _config;
}
std::string extractTime(const std::chrono::system_clock::time_point &now)
{
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &now_c);
#else
    localtime_r(&now_c, &timeinfo);
#endif

    std::ostringstream oss;
    oss << std::put_time(&timeinfo, "%Y/%m/%d %H:%M:%S");
    return oss.str();
}




void genearateSignedUrl(const string &_Endpoint, const string &_Bucket, const string &_GetobjectUrlName, string &_GenedUrl)
{
    try
    {
        AlibabaCloud::OSS::OssClient client(_Endpoint, rconfig.AccessKeyId, rconfig.AccessKeySecret, conf);
        auto genOutcome = client.GeneratePresignedUrl(_Bucket, _GetobjectUrlName, rconfig.sign_time, AlibabaCloud::OSS::Http::Get);
        if (genOutcome.isSuccess())
        {
            // std::cout << "GeneratePresignedUrl success, Gen url: " << genOutcome.result().c_str() << std::endl;
            _GenedUrl = genOutcome.result().c_str();
        }
        else
        {
            std::clog << "GeneratePresignedUrl fail, code: " << genOutcome.error().Code()
                      << ", message: " << genOutcome.error().Message()
                      << ", requestId: " << genOutcome.error().RequestId() << std::endl;
            poco_information(logger_handle, "GeneratePresignedUrl fail, code: " + genOutcome.error().Code() + ", message: " + genOutcome.error().Message() + ", requestId: " + genOutcome.error().RequestId());
        }
    }
    catch (const Poco::Exception &e)
    {
        std::cerr << "Error generating Presigned URL: " << e.displayText() << std::endl;
        poco_error(logger_handle, "Error generating Presigned URL: " + e.displayText());
    }
}