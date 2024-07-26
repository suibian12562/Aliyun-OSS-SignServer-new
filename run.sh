#!/bin/sh

# 创建或更新配置文件
echo '{
    "AccessKeyId": "'"${ACCESS_KEY_ID}"'",
    "AccessKeySecret": "'"${ACCESS_KEY_SECRET}"'",
    "port": '"${PORT}"',
    "sign_time": '"${SIGN_TIME}"'
}' > /app/config.json

# 运行可执行文件
exec "./$(find . -type f -executable)"
