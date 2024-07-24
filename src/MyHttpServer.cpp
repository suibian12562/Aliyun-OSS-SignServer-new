#include "main.h"
#include "RequestHandlerFactory.h"
#include "MyHttpServer.h"
#include "logger.h"

using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::ServerSocket;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionSet;

extern const Config rconfig;
void MyHttpServer::initialize(Application &self)
{
    loadConfiguration(); // load default configuration files, if present

    MyLogger::Setup_logger();
    // get parameters from configuration file
    createDefaultDatabase("cache.db");
    // std::cout << rconfig.sign_time;
    std::cout << "Specified port: " << rconfig.port << std::endl;
    std::cout << "AccessKeyId: " << rconfig.AccessKeyId << std::endl;
    std::cout << "AccessKeySecret: " << rconfig.AccessKeySecret << std::endl;
    std::cout << "sign_time: " << rconfig.sign_time << std::endl;

    AlibabaCloud::OSS::InitializeSdk();

    ServerApplication::initialize(self);
}

void MyHttpServer::uninitialize()
{
    ServerApplication::uninitialize();
}
void MyHttpServer::defineOptions(OptionSet &options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
            .required(false)
            .repeatable(false));
}

void MyHttpServer::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A sample HTTP server supporting the WebSocket protocol.");
    helpFormatter.format(std::cout);
}

int MyHttpServer::main(const std::vector<std::string> &args)
{
    HTTPServerParams *params = new HTTPServerParams;
    params->setMaxQueued(100);
    params->setMaxThreads(8);
    try
    {
        ServerSocket svs(rconfig.port);

        //安装一个HttpServer实例  并且传递 请求处理器工厂  和一个HttpServerParams对象
        HTTPServer srv(new RequestHandlerFactory, svs, params);

        srv.start();

        waitForTerminationRequest();
        // 停止HTTP服务器
        srv.stop();
    }
    catch(Poco::Exception &ex)
    {
        std::cout << ex.displayText() << std::endl;
    }
    return Application::EXIT_OK; // 返回正常退出状态
}