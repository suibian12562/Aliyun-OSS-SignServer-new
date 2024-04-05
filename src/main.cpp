﻿#include "main.h"

using namespace Poco::Net;
using namespace Poco::JSON;
Config rconfig;

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
void genearateSignedUrl(const string _Endpoint, const string _Bucket, const string _GetobjectUrlName, string &_GenedUrl, long &_request_time)
{
    try
    {
    AlibabaCloud::OSS::ClientConfiguration conf;
    AlibabaCloud::OSS::OssClient client(_Endpoint, rconfig.AccessKeyId, rconfig.AccessKeySecret, conf);

    auto genOutcome = client.GeneratePresignedUrl(_Bucket, _GetobjectUrlName, _request_time, AlibabaCloud::OSS::Http::Get);
    if (genOutcome.isSuccess())
        {
            std::cout << "GeneratePresignedUrl success, Gen url: " << genOutcome.result().c_str() << std::endl;
            _GenedUrl = genOutcome.result().c_str();
        }
    else
        {
            std::cout   << "GeneratePresignedUrl fail, code: " << genOutcome.error().Code()
                        << ", message: " << genOutcome.error().Message()
                        << ", requestId: " << genOutcome.error().RequestId() << std::endl;
        }
    }
    catch (const Poco::Exception &e)
    {
        std::cerr << "Error generating Presigned URL: " << e.displayText() << std::endl;
    }
}

class RequestHandler: public Poco::Net::HTTPRequestHandler {
private:

public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
        Poco::URI uri(request.getURI());
        message_info info;

        for (const auto& param : uri.getQueryParameters()) {
            if (param.first == "Endpoint") {
                info._Endpoint = param.second;
            } else if (param.first == "Bucket") {
                info._Bucket = param.second;
            } else if (param.first == "GetobjectUrlName") {
                info._GetobjectUrlName = param.second;
            }
        }

        std::string cachedUrl;
        long requestTime;

        genearateSignedUrl(info._Endpoint, info._Bucket, info._GetobjectUrlName, info._GenedUrl, info._request_time);

        // 将消息信息转换为JSON对象
        Poco::JSON::Object::Ptr jsonInfo = info.toJSON();

        // 发送JSON响应
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setContentType("application/json");
        std::ostream& ostr = response.send();
        jsonInfo->stringify(ostr);
    }
};



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

        AlibabaCloud::OSS::InitializeSdk();
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