#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/FileStream.h"
#include "Poco/Exception.h"

#include "alibabacloud/oss/OssClient.h"  // 阿里云 OSS C++ SDK 头文件

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>


using namespace Poco::Net;
using namespace Poco::JSON;
using namespace Poco::Util;
using namespace std;

struct message_info
{
    std::string _Endpoint;
    std::string _Bucket;
    std::string _GetobjectUrlName;
    std::string _GenedUrl;
    long _request_time;

    Object::Ptr toJSON() const
    {
        Object::Ptr pObj = new Object;
        pObj->set("Endpoint", _Endpoint);
        pObj->set("Bucket", _Bucket);
        pObj->set("GetobjectUrlName", _GetobjectUrlName);
        pObj->set("GenedUrl", _GenedUrl);
        pObj->set("request_time", _request_time);
        return pObj;
    }
};

struct Config {
    std::string AccessKeyId;
    std::string AccessKeySecret;
    long sign_time;
    int port;
};

