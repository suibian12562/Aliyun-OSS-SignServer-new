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
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/FileStream.h"
#include "Poco/Exception.h"
#include "Poco/Net/HTTPServer.h"   //继承自TCPServer 实现了一个完整的HTTP多线程服务器
#include "Poco/Net/HTTPRequestHandler.h"   //抽象基类类 被HttpServer所创建 用来处理Http的请求
#include "Poco/Net/HTTPRequestHandlerFactory.h" //HTTPRequestHandler的工厂 给予工厂设计模式 
#include "Poco/Net/HTTPServerParams.h"    //被用来指定httpserver以及HTTPRequestHandler的参数
#include "Poco/Net/HTTPServerRequest.h"  //ServerRequest的抽象子类用来指定 服务器端的 http请求
#include "Poco/Net/HTTPServerResponse.h" //ServerResponse的抽象子类用来指定服务器端的http响应
#include "Poco/Net/NetException.h"  //网络异常
#include "Poco/Util/ServerApplication.h" //Application的子类 所有服务器程序 包括 Reactor FTP HTTP等都用到 算是服务器的启动类 
#include "Poco/Util/Option.h"  //存储了命令行选项
#include "Poco/Util/OptionSet.h"  //一个Opention对象的集合
#include "Poco/Util/HelpFormatter.h" //从OptionSet格式化帮助信息
#include "Poco/Format.h"  //格式化函数的实现类似于 C的 sprintf函数 具体看文档
#include "Poco/Net/ServerSocket.h"  //提供了一个TCP服务器套接字接口
#include "Poco/Net/WebSocket.h"  //这个类实现了RFC 6455 web套接字接口  专门针对web服务器用
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/Dynamic/Var.h"


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
    int sign_time;
    int port;
};

