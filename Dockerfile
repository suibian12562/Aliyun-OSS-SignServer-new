# 使用alpine作为基础镜像
FROM alpine:latest

# 安装必要的工具
RUN apk add --no-cache curl unzip

# 设置环境变量
ENV GITHUB_REPO="YOUR_GITHUB_REPO"
ENV ARCH="Linux-x64"
ENV RELEASE_NAME="${ARCH}-Release.zip"
ENV PORT=8080

# 使用curl从GitHub下载最新的release
RUN curl -s https://api.github.com/repos/${GITHUB_REPO}/releases/latest \
    | grep "browser_download_url.*${RELEASE_NAME}" \
    | cut -d : -f 2,3 \
    | tr -d \" \
    | wget -qi -

# 创建目录并解压缩文件
RUN mkdir -p /app && \
    unzip ${RELEASE_NAME} -d /app

# 复制启动脚本到容器中
COPY start.sh /app/start.sh
RUN chmod +x /app/start.sh

# 进入解压后的目录
WORKDIR /app

# 查找可执行文件并赋予执行权限
RUN find . -type f -executable -exec chmod +x {} +

# 暴露容器内部端口
EXPOSE ${PORT}

# 设置ENTRYPOINT来运行启动脚本
ENTRYPOINT ["/app/run.sh"]
