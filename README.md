# Aliyun-OSS-GeneratePresignedUrl-new
http API,传入要访问的私有bucket中的文件生成签名URL并返回签名URL.计划包含客户端验证.

**编译**  
使用[microsoft/_vcpkg_](https://github.com/microsoft/vcpkg)管理包,为此编译前你需使用此命令安装依赖
```BASH
vcpkg install
```
然后使用Cmake进行编译
```SHELL
cmake -B build
cmake --build build
```
***
**TODO**  
- [x] 签名有效时间  
- [ ] 人机验证  
- [ ] Bucket权限管理  
***
**配置**  
程序在第一次启动时会在目录下生成一个config.json文件,内容如下
```json
{
    "AccessKeyId": "your_access_key",
    "AccessKeySecret": "your_access_secret",
    "port": 1145,
    "sign_time": 40
}
```
将其中的配置替换为你自己的数值.
***
**使用**  
程序会开启一个在设定端口的Websocket服务器
向此端口传入
```JSON
{
        "_Endpoint": endpoint,
        "_Bucket": bucket,
        "_GetobjectUrlName": getObjectUrlName
      }
```
将会收到签名后的链接以供下载
