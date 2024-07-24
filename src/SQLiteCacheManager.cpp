#include "main.h"
#include "SQLiteCacheManager.h"
#include "logger.h"

extern const Config rconfig;


SQLiteCacheManager::SQLiteCacheManager(const std::string &dbPath) : _dbPath(dbPath)
{
    Poco::Data::SQLite::Connector::registerConnector();
    _session = std::make_shared<Poco::Data::Session>("SQLite", _dbPath);
}

SQLiteCacheManager::~SQLiteCacheManager()
{
    try
    {
        Poco::Data::SQLite::Connector::unregisterConnector();
    }
    catch (Poco::Exception &e)
    {
        // do nothing
    }
}

int SQLiteCacheManager::deleteFromCache(const std::string &getObjectUrlName)
{

    Poco::Data::Statement deleteStmt(*_session);
    deleteStmt << "DELETE FROM Cache WHERE GetobjectUrlName = ?", Poco::Data::Keywords::useRef(getObjectUrlName);

    if (deleteStmt.execute() > 0) // 如果影响行数大于0，则表示删除成功
    {
        return 1;
        std::clog << "delete successful\n";
    }
    poco_information(logger_handle,"delete failed");
    std::clog << "delete failed\n";
    return 0; // 返回错误代码或其他适当值
}

int SQLiteCacheManager::saveToCache(const std::string &getObjectUrlName, const std::string &genedUrl, const unsigned int &timeNow)
{
    long expirationTime = timeNow + rconfig.sign_time;
    Poco::Data::Statement insert(*_session);

    try
    {
        insert << "INSERT OR REPLACE INTO Cache (GetobjectUrlName, GenedUrl, RequestTime, ExpirationTime) VALUES (?, ?, ?, ?)",
            Poco::Data::Keywords::useRef(getObjectUrlName),
            Poco::Data::Keywords::useRef(genedUrl),
            Poco::Data::Keywords::useRef(timeNow),
            Poco::Data::Keywords::useRef(expirationTime);
        insert.execute();
    }
    catch (Poco::Exception &e)
    {
        std::cerr << e.what() << std::endl;
        poco_error(logger_handle, "Error executing SQL statement");
        return 0;
    }

    return 1;
}

bool SQLiteCacheManager::getFromCache(const std::string &getObjectUrlName, std::string &genedUrl, unsigned int &timeNow)
{
    if (getObjectUrlName.empty())
    {
        std::cerr << "getObjectUrlName is empty." << std::endl;
        poco_error(logger_handle, "getObjectUrlName is empty");
        return false;
    }
    int expirationTime = 0;
    int waste;
    Poco::Data::Statement select(*_session);
    select << "SELECT GenedUrl, RequestTime, ExpirationTime FROM Cache WHERE GetobjectUrlName = ?",
        Poco::Data::Keywords::into(genedUrl),
        Poco::Data::Keywords::into(waste),
        Poco::Data::Keywords::into(expirationTime),
        Poco::Data::Keywords::useRef(getObjectUrlName);

    if (select.execute() == 0)
    {
        // std::cerr << "No data found for getObjectUrlName: " << getObjectUrlName << std::endl;
        return false;
    }


    if (expirationTime > timeNow)
    {
        return true;
    }
    else
    {
        std::clog << "Data expired for getObjectUrlName: " << getObjectUrlName << std::endl;
        poco_information(logger_handle, "Data expired for getObjectUrlName: " + getObjectUrlName);
        // 删除过期缓存
        deleteFromCache(getObjectUrlName);
        return false;
    }
}
