# 使用alpine作为基础镜像
FROM alpine:latest

# 安装必要的工具
RUN apk add curl unzip jq

# 创建一个非root用户
RUN adduser -D -u 1000 appuser

# 创建目录并设置权限
RUN mkdir -p /app && chown appuser:appuser /app

# 设置环境变量
ENV GITHUB_REPO="suibian12562/Aliyun-OSS-SignServer-new"
ENV ARCH="Linux-x64"
ENV RELEASE_NAME="${ARCH}-Release.zip"
ENV PORT=8080

# 获取最新的release信息并打印出来以进行调试
RUN --mount=type=cache,target=/root/.cache \
    curl -s https://api.github.com/repos/${GITHUB_REPO}/releases/latest -o /release_info.json && \
    cat /release_info.json

# 打印JSON文件内容进行调试
RUN --mount=type=tmpfs,target=/root/.cache \
    cat /release_info.json

# 使用jq获取下载链接并使用curl下载（禁用缓存）
RUN --mount=type=tmpfs,target=/root/.cache \
    RELEASE_URL=$(jq -r ".assets[] | select(.name == \"${RELEASE_NAME}\") | .browser_download_url" /release_info.json) && \
    echo "Release URL: $RELEASE_URL" && \
    curl -L -o /app/${RELEASE_NAME} "$RELEASE_URL"

# 解压文件
RUN cd /app \
    unzip ${RELEASE_NAME} -d /app

# 创建config.json文件
RUN echo '{ \
    "AccessKeyId": "your_access_key", \
    "AccessKeySecret": "your_access_secret", \
    "port": 8080, \
    "sign_time": 40 \
}' > /app/config.json

# 复制启动脚本到容器中
COPY  run.sh /app/run.sh

# 赋予启动脚本执行权限
RUN chmod +x /app/run.sh

# 进入解压后的目录
WORKDIR /app

# 查找可执行文件并赋予执行权限
RUN find . -type f -executable -exec chmod +x {} +

# 暴露容器内部端口
EXPOSE ${PORT}

# 切换到非root用户
USER appuser

# 设置ENTRYPOINT来运行启动脚本
ENTRYPOINT ["/app/run.sh"]
