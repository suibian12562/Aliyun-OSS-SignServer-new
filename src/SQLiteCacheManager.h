#ifndef SQLiteCacheManager_h
#define SQLiteCacheManager_h

#include "main.h"



class SQLiteCacheManager
{
private:
    std::shared_ptr<Poco::Data::Session> _session;   
    // Poco::Data::Session* _session;
    std::string _dbPath;



public:


    SQLiteCacheManager(const std::string& dbPath = "cache.db");

    ~SQLiteCacheManager();

    int deleteFromCache(const std::string &getObjectUrlName);

    int saveToCache(const std::string &getObjectUrlName, const std::string &genedUrl, const unsigned int &requestTime);

    bool getFromCache(const std::string &getObjectUrlName, std::string &genedUrl,unsigned int &requestTime);
};

#endif
