# Aliyun-OSS-GeneratePresignedUrl-new
[![.github/workflows/cmake-linux.yml](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/cmake-linux.yml)
[![.github/workflows/cmake-windows.yml](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/cmake-windows.yml)  
http API,传入要访问的私有bucket中的文件生成签名URL并返回签名URL.计划包含客户端验证.

**编译**  
使用[microsoft/_vcpkg_](https://github.com/microsoft/vcpkg)管理包,为此编译前你需使用此命令安装依赖
```BASH
vcpkg install
```[![.github/workflows/cmake-windows.yml](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/suibian12562/Aliyun-OSS-SignServer-new/actions/workflows/cmake-windows.yml)
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
程序会开启一个在设定端口的http服务器

