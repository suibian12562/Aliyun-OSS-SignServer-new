#ifndef SQLiteCacheManager_h
#define SQLiteCacheManager_h

#include "main.h"

extern Poco::Semaphore semaphore_db;


class SQLiteCacheManager
{
private:
    sqlite3 *db;

public:
    SQLiteCacheManager();

    ~SQLiteCacheManager();

    int deleteFromCache(const std::string &getObjectUrlName);

    int saveToCache(const std::string &getObjectUrlName, const std::string &genedUrl, long requestTime, long cacheDuration);

    bool getFromCache(const std::string &getObjectUrlName, std::string &genedUrl, const long &requestTime);
};

#endif
