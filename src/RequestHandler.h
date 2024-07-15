#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "main.h"
#include "SQLiteCacheManager.h" 


class RequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
    SQLiteCacheManager cacheManager;


public:
    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
};

#endif
