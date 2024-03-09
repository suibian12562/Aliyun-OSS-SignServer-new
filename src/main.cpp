// #include "main.h"
// Config config;



// class MyRequestHandler : public HTTPRequestHandler
// {
// public:
//     void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
//     {
//         std::cout << request.getURI() << std::endl;
//         std::cout << request.getVersion() << std::endl;
//         std::cout << request.getContentType() << std::endl;
//         std::cout << request.getTransferEncoding() << std::endl;

//         std::string recv_string;
//         Poco::StreamCopier::copyToString(request.stream(), recv_string);
//         std::cout << recv_string << std::endl;
//         //

//         response.setChunkedTransferEncoding(true);
//         response.setContentType("text/html");
//         std::ostream& ostr = response.send();
//         ostr << "<html><head><title>HTTP Server powered by POCO C++ Libraries</title></head>";
//         ostr << "<body>";
//         ostr << "<h1>hello</h1>";
//         ostr << "</body></html>";


        
//         // try
//         // {
//         //     std::istream& requestBody = request.stream();
//         //     std::stringstream ss;
//         //     Poco::StreamCopier::copyStream(requestBody, ss);
//         //     std::string payload = ss.str();

//         //     std::cout << "Request received with payload: " << payload << std::endl;

//         //     Poco::JSON::Parser parser;
//         //     Poco::Dynamic::Var result = parser.parse(payload);

//         //     if (result.isStruct())
//         //     {
//         //         Object::Ptr json = result.extract<Object::Ptr>();

//         //         if (json->has("Endpoint") && json->has("Bucket") &&
//         //             json->has("GetobjectUrlName") && json->has("request_time"))
//         //         {
//         //             message_info temp;
//         //             temp._Endpoint = json->getValue<std::string>("Endpoint");
//         //             temp._Bucket = json->getValue<std::string>("Bucket");
//         //             temp._GetobjectUrlName = json->getValue<std::string>("GetobjectUrlName");
//         //             temp._request_time = json->getValue<long>("request_time");

//         //             std::string genedUrl = generatePresignedUrl(temp);

//         //             response.setStatus(HTTPResponse::HTTP_OK);
//         //             response.setContentType("text/plain");
//         //             std::ostream& ostr = response.send();
//         //             ostr << "Generated URL: " << genedUrl;
//         //         }
//         //         else
//         //         {
//         //             response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
//         //             response.send() << "JSON data does not contain the expected fields.";
//         //         }
//         //     }
//         //     else
//         //     {
//         //         response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
//         //         response.send() << "Failed to parse JSON data.";
//         //     }
//         // }
//         // catch (const Poco::Exception& e)
//         // {
//         //     response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
//         //     response.send() << "Internal Server Error: " << e.displayText();
//         // }
//     }

// private:
//     // std::string generatePresignedUrl(const message_info& info)
//     // {
//     //     try
//     //     {
//     //         AlibabaCloud::OSS::InitializeSdk();
//     //         AlibabaCloud::OSS::ClientConfiguration conf;
//     //         AlibabaCloud::OSS::OssClient client(info._Endpoint, config.AccessKeyId, config.AccessKeySecret, conf);
//     //         std::time_t t = std::time(nullptr) + config.sign_time;

//     //         auto genOutcome = client.GeneratePresignedUrl(info._Bucket, info._GetobjectUrlName, t, AlibabaCloud::OSS::Http::Get);
//     //         if (genOutcome.isSuccess())
//     //         {
//     //             std::cout << "GeneratePresignedUrl success, Gen url: " << genOutcome.result().c_str() << std::endl;
//     //             return genOutcome.result().c_str();
//     //         }
//     //         else
//     //         {
//     //             std::cout << "GeneratePresignedUrl fail, code: " << genOutcome.error().Code()
//     //                       << ", message: " << genOutcome.error().Message()
//     //                       << ", requestId: " << genOutcome.error().RequestId() << std::endl;
//     //             return "Error generating Presigned URL";
//     //         }
//     //     }
//     //     catch (const Poco::Exception& e)
//     //     {
//     //         std::cerr << "Error generating Presigned URL: " << e.displayText() << std::endl;
//     //         return "Error generating Presigned URL";
//     //     }
//     // }
// };

// class MyRequestHandlerFactory : public HTTPRequestHandlerFactory
// {
// public:
//     HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
//     {
//         return new MyRequestHandler();
//     }
// };

// int main(int argc, char** argv)
// {
// Config config;
//         config = readConfigFromFile("config.json");
//         std::cout << "Specified port: " << config.port << std::endl;
//         std::cout << "AccessKeyId: " << config.AccessKeyId << std::endl;
//         std::cout << "AccessKeySecret: " << config.AccessKeySecret << std::endl;
//         std::cout << "sign_time: " << config.sign_time << std::endl;
//         cout << "\n\n\n" <<endl;///log

//         HTTPServer s(new MyRequestHandlerFactory,ServerSocket(config.port),new HTTPServerParams);
//         s.start();
        
//         s.stop();

//         return 0;


// }




#include "main.h"

// using Poco::Net::ServerSocket;
// using Poco::Net::WebSocket;
// using Poco::Net::WebSocketException;
// using Poco::Net::HTTPRequestHandler;
// using Poco::Net::HTTPRequestHandlerFactory;
// using Poco::Net::HTTPServer;
// using Poco::Net::HTTPServerRequest;
// using Poco::Net::HTTPResponse;
// using Poco::Net::HTTPServerResponse;
// using Poco::Net::HTTPServerParams;
// using Poco::Timestamp;
// using Poco::ThreadPool;
// using Poco::Util::ServerApplication;
// using Poco::Util::Application;
// using Poco::Util::Option;
// using Poco::Util::OptionSet;
// using Poco::Util::HelpFormatter;

using namespace Poco::Net;
using namespace Poco::JSON;


void createDefaultConfig(const std::string& filename) {
    Config config;
    config.AccessKeyId = "your_access_key";
    config.AccessKeySecret = "your_access_secret";
    config.sign_time = 40;
    config.port = 1145;

    Poco::JSON::Object::Ptr jsonObj = new Poco::JSON::Object;
    jsonObj->set("AccessKeyId", config.AccessKeyId);
    jsonObj->set("AccessKeySecret", config.AccessKeySecret);
    jsonObj->set("sign_time", config.sign_time);
    jsonObj->set("port", config.port);

    std::ostringstream oss;
    Poco::JSON::Stringifier::stringify(jsonObj, oss, 4);

    try {
        Poco::FileOutputStream fileStream(filename);
        fileStream << oss.str();
        fileStream.close();
        std::cout << "配置文件已创建成功。" << std::endl;
    } catch (Poco::Exception& e) {
        std::cerr << "错误: 无法创建配置文件。" << e.displayText() << std::endl;
    }
}
Config readConfigFromFile(const std::string& filename) {
    Config _config;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Config file not found. Creating a default config file. Please configure and restart the program." << std::endl;
        createDefaultConfig(filename);
        // Return the default config by reference
        exit(1);
    }

    try {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(file);
        Poco::JSON::Object::Ptr jsonObj = result.extract<Poco::JSON::Object::Ptr>();

        _config.AccessKeyId = jsonObj->getValue<std::string>("AccessKeyId");
        _config.AccessKeySecret = jsonObj->getValue<std::string>("AccessKeySecret");
        _config.sign_time = jsonObj->getValue<int>("sign_time");
        _config.port = jsonObj->getValue<int>("port");
    }
    catch (const Poco::Exception& e) {
        std::cerr << "Error: Unable to parse config JSON: " << e.displayText() << std::endl;
    }

    file.close();
    // Return the loaded config
    return _config;
}

//////页面处理器 链接到来的时候 直接打印html内容
// class PageRequestHandler: public HTTPRequestHandler
//     /// Return a HTML document with some JavaScript creating
//     /// a WebSocket connection.
// {
// public:
//     void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
//     {
//
//         response.setChunkedTransferEncoding(true);
//         response.setContentType("text/html");
//         std::ostream& ostr = response.send();
//         ostr << "<html>";
//         ostr << "<head>";
//         ostr << "<title>WebSocketServer</title>";
//         ostr << "<script type=\"text/javascript\">";
//         ostr << "function WebSocketTest()";
//         ostr << "{";
//         ostr << "  if (\"WebSocket\" in window)";
//         ostr << "  {";
//         ostr << "    var ws = new WebSocket(\"ws://" << request.serverAddress().toString() << "/ws\");";
//         ostr << "    ws.onopen = function()";
//         ostr << "      {";
//         ostr << "        ws.send(\"Hello, world!\");";
//         ostr << "      };";
//         ostr << "    ws.onmessage = function(evt)";
//         ostr << "      { ";
//         ostr << "        var msg = evt.data;";
//         ostr << "        alert(\"Message received: \" + msg);";
//         ostr << "        ws.close();";
//         ostr << "      };";
//         ostr << "    ws.onclose = function()";
//         ostr << "      { ";
//         ostr << "        alert(\"WebSocket closed.\");";
//         ostr << "      };";
//         ostr << "  }";
//         ostr << "  else";
//         ostr << "  {";
//         ostr << "     alert(\"This browser does not support WebSockets.\");";
//         ostr << "  }";
//         ostr << "}";
//         ostr << "</script>";
//         ostr << "</head>";
//         ostr << "<body>";
//         ostr << "  <h1>WebSocket Server</h1>";
//         ostr << "  <p><a href=\"javascript:WebSocketTest()\">Run WebSocket Script</a></p>";
//         ostr << "</body>";
//         ostr << "</html>";
//     }
// };



class RequestHandler: public HTTPRequestHandler
    /// Return a HTML document with some JavaScript creating
    /// a WebSocket connection.
{
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        Poco::URI uri(request.getURI());
        message_info info;
        
        for (const auto& param : uri.getQueryParameters()) {
            if (param.first == "Endpoint") {
                info._Endpoint = param.second;
            } else if (param.first == "Bucket") {
                info._Bucket = param.second;
            } else if (param.first == "GetobjectUrlName") {
                info._GetobjectUrlName = std::stol(param.second);
            } 
        }

        Object::Ptr jsonInfo = info.toJSON();

        // 输出 JSON 对象
        response.setStatus(HTTPResponse::HTTP_OK);
        response.setContentType("application/json");
        std::ostream& ostr = response.send();
        jsonInfo->stringify(ostr);


    }
};

/////////http请求处理器 1
// class WebSocketRequestHandler: public HTTPRequestHandler
//     /// Handle a WebSocket connection.
// {
// public:
//     void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
//     {
//         Application& app = Application::instance();
//         try
//         {
//             WebSocket ws(request, response);
//             app.logger().information("WebSocket connection established.");
//             char buffer[1024];
//             int flags;
//             int n;
//             do
//             {
//                 n = ws.receiveFrame(buffer, sizeof(buffer), flags);
//                 app.logger().information(Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)));
//                 ws.sendFrame(buffer, n, flags);
//             }
//             while (n > 0 || (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
//             app.logger().information("WebSocket connection closed.");
//         }
//         catch (WebSocketException& exc)
//         {
//             app.logger().log(exc);
//             switch (exc.code())
//             {
//             case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
//                 response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
//                 // fallthrough
//             case WebSocket::WS_ERR_NO_HANDSHAKE:
//             case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
//             case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
//                 response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
//                 response.setContentLength(0);
//                 response.send();
//                 break;
//             }
//         }
//     }
// };

//HTTP请求处理器工厂 
class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) //覆盖方法 
    {  
        return new RequestHandler;
    }
};


//////////网络应用程序的启动器 都是ServerApplication
class WebSocketServer: public Poco::Util::ServerApplication
{
public:
    WebSocketServer() 
    {
    }
    
    ~WebSocketServer()
    {
    }

protected:
    ///再启动的时候先调用
    void initialize(Application& self)
    {
        loadConfiguration(); // load default configuration files, if present
        ServerApplication::initialize(self);
    }
    //在释放的时候调用    
    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

   ///覆盖基类的函数 定义一些命令选项
    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);
        
        options.addOption(
            Option("help", "h", "display help information on command line arguments")
                .required(false)
                .repeatable(false));
    }

    int main(const std::vector<std::string>& args)
    {

        Config rconfig;
        rconfig = readConfigFromFile("config.json");
        // get parameters from configuration file
        std::cout << "Specified port: " << rconfig.port << std::endl;
        std::cout << "AccessKeyId: " << rconfig.AccessKeyId << std::endl;
        std::cout << "AccessKeySecret: " << rconfig.AccessKeySecret << std::endl;
        std::cout << "sign_time: " << rconfig.sign_time << std::endl<< std::endl;


        //从配置文件中获取端口
        unsigned short port = (unsigned short) rconfig.port;

        HTTPServerParams* params = new HTTPServerParams;
        params->setMaxQueued(100);
        params->setMaxThreads(4);
        // 安装一个ServerSocket
        ServerSocket svs(port);
        // 安装一个HttpServer实例  并且传递 请求处理器工厂  和一个HttpServerParams对象
        HTTPServer srv(new RequestHandlerFactory, svs, params);
        // 启动服务器
        srv.start();
        // 等待kill 或者控制台CTRL+C
        waitForTerminationRequest();
        // 停止HTTP服务器
        srv.stop();
        
        return Application::EXIT_OK;  //返回正常退出状态
    }
    
private:
};


//启动web 服务器
POCO_SERVER_MAIN(WebSocketServer)
