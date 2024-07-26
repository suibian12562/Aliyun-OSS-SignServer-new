# Aliyun-OSS-GeneratePresignedUrl-new
[![.github/workflows/build on dev.yml](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/build%20on%20dev.yml/badge.svg)](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/build%20on%20dev.yml)
阿里云OSS签名工具(外链工具),纯cpp实现,高性能http服务器,跨平台
***
**编译**<br>
使用[microsoft/_vcpkg_](https://github.com/microsoft/vcpkg)管理包,为此编译前你需使用此命令安装依赖<br>
```BASH
vcpkg install
```
Linux下需安装依赖:libcurl4-openssl-dev libssl-dev
```BASH
sudo apt-get install libcurl4-openssl-dev libssl-dev
```

然后使用Cmake进行编译
```SHELL
cmake -B build
cmake --build build
```
***
**TODO**  
- [X] 签名有效时间  
- [ ] 人机验证  
- [ ] Bucket权限管理  
***
**配置**  
程序在第一次启动时会在目录下生成一个config.json文件,内容如下
```json
{
    "AccessKeyId": "your_access_key",
    "AccessKeySecret": "your_access_secret",
    "port": 8080,
    "sign_time": 40
}
```
将其中的配置替换为你自己的数值.
***
**使用**  
```BASH
curl --location --request GET 'http://you_server_address:port/?Endpoint=example&Bucket=example&GetobjectUrlName=example' \
--header 'User-Agent: example/1.0.0 (https://example.com)' \
--header 'Accept: */*' \
--header 'Connection: keep-alive'
```
GET或POST都可用
