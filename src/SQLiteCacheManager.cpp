#include "main.h"
#include "SQLiteCacheManager.h"
#include "logger.h"

Poco::Semaphore semaphore_db(1);

SQLiteCacheManager::SQLiteCacheManager()
{
    semaphore_db.wait();
    int rc = sqlite3_open("cache.db", &db);
    if (rc)
    {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
        poco_error(logger_handle, "Error opening SQLite database");
    }
    else
    {
        // std::cout << "Opened SQLite database successfully" << std::endl;
        // poco_information(logger_handle, "Opened SQLite database successfully");
    }
    semaphore_db.set();
}

SQLiteCacheManager::~SQLiteCacheManager()
{
    try
    {
        semaphore_db.wait();
        sqlite3_close(db);
        semaphore_db.set();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

int SQLiteCacheManager::deleteFromCache(const std::string &getObjectUrlName)
{
    std::string sql = "DELETE FROM Cache WHERE GetobjectUrlName = ?";
    sqlite3_stmt *stmt;

    semaphore_db.wait();

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        poco_error(logger_handle, "Error preparing SQL statement");
        semaphore_db.set();
        return 0;
    }

    sqlite3_bind_text(stmt, 1, getObjectUrlName.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        poco_error(logger_handle, "Error executing SQL statement");
        semaphore_db.set();
        return 0;
    }

    sqlite3_finalize(stmt);
    semaphore_db.set();
    return 1;
}

int SQLiteCacheManager::saveToCache(const std::string &getObjectUrlName, const std::string &genedUrl, long requestTime, long cacheDuration)
{
    long expirationTime = requestTime + cacheDuration;
    std::string sql = "INSERT OR REPLACE INTO Cache (GetobjectUrlName, GenedUrl, RequestTime, ExpirationTime) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    semaphore_db.wait();

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        poco_error(logger_handle, "Error preparing SQL statement");
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
        poco_error(logger_handle, "Error executing SQL statement");
        semaphore_db.set();
        return 0;
    }

    sqlite3_finalize(stmt);
    semaphore_db.set();
    return 1;
}

bool SQLiteCacheManager::getFromCache(const std::string &getObjectUrlName, std::string &genedUrl, const long &requestTime)
{
    if (getObjectUrlName.empty())
    {
        std::cerr << "getObjectUrlName is empty." << std::endl;
        poco_error(logger_handle, "getObjectUrlName is empty");
        return false;
    }

    std::string sql = "SELECT GenedUrl, RequestTime, ExpirationTime FROM Cache WHERE GetobjectUrlName = ?";
    sqlite3_stmt *stmt = nullptr; // 初始化为nullptr

    semaphore_db.wait();

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        poco_error(logger_handle, "Error preparing SQL statement");
        semaphore_db.set();
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, getObjectUrlName.c_str(), getObjectUrlName.size(), SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error binding parameter: " << sqlite3_errmsg(db) << std::endl;
        poco_error(logger_handle, "Error binding parameter");
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
            poco_information(logger_handle, "Data expired for getObjectUrlName: " + getObjectUrlName);
            sqlite3_finalize(stmt);
            semaphore_db.set();
            // 删除过期缓存
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
        poco_error(logger_handle, "Error executing SQL statement");
    }

    sqlite3_finalize(stmt); // 在所有返回路径上都确保释放stmt资源
    semaphore_db.set();
    return false;
}