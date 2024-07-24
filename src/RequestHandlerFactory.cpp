#include "main.h"
#include "RequestHandler.h"
#include "RequestHandlerFactory.h"

Poco::Net::HTTPRequestHandler *RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request) // 覆盖方法
{
    return new RequestHandler;
}
