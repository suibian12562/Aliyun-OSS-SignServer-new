#include "RequestHandler.h"
#include "main.h"
#include "logger.h"

extern const Config rconfig;

void RequestHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
{
    Poco::URI uri(request.getURI());
    message_info data;

    const auto &queryParameters = uri.getQueryParameters();
    if (!queryParameters.empty())
    {
        // 遍历查询参数，使用结构化绑定进行声明
        for (const auto &[key, value] : queryParameters)
        {
            // 根据参数名进行赋值
            if (key == "Endpoint")
            {
                data._Endpoint = value;
            }
            else if (key == "Bucket")
            {
                data._Bucket = value;
            }
            else if (key == "GetobjectUrlName")
            {
                data._GetobjectUrlName = value;
            }
        }
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    int requestTime = static_cast<int>(timestamp);

    std::string cachedUrl;
    if (cacheManager.getFromCache(data._GetobjectUrlName, cachedUrl, requestTime))
    {
        data._GenedUrl = cachedUrl;
        data._request_time = requestTime;
    }
    else
    {

        // 生成签名URL
        genearateSignedUrl(data._Endpoint, data._Bucket, data._GetobjectUrlName, data._GenedUrl);
        // std::cout << "genearated" << std::endl;

        // 保存到缓存
        data._request_time = requestTime;
        if (cacheManager.saveToCache(data._GetobjectUrlName, data._GenedUrl, requestTime, rconfig.sign_time))
        {
            // std::cout << "cached" << std::endl;
        }
        else
        {
            std::clog << "datebase error at " << data._Bucket << " for " << data._GetobjectUrlName << " and at " << extractTime(now) << std::endl;
            poco_error(logger_handle, "datebase error at " + data._Bucket + " for " + data._GetobjectUrlName + " and at " + extractTime(now));
        }
        // std::cout << requestTime;
        // std::cout << info._request_time;
    }

    // 将消息信息转换为JSON对象
    Poco::JSON::Object::Ptr jsonInfo = data.toJSON();

    // 发送JSON响应
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &ostr = response.send();
    jsonInfo->stringify(ostr);
}
