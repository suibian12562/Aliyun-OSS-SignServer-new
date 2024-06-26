cmake_minimum_required(VERSION 3.25)



execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE
)



project(Aliyun-OSS-SignServer-new
    LANGUAGES CXX
    DESCRIPTION "A Aliyun OSS sign server by cpp"
    # VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 11)
set(EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME} src/main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${GIT_BRANCH_NAME}_${GIT_COMMIT_HASH}")
add_subdirectory(third_party)

find_package(websocketpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(Poco REQUIRED COMPONENTS Net )
find_package(Poco REQUIRED COMPONENTS Util Foundation JSON)
find_package(unofficial-sqlite3 CONFIG REQUIRED)


target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/third_party/osssdk/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
osssdk
websocketpp::websocketpp
nlohmann_json::nlohmann_json
OpenSSL::SSL 
OpenSSL::Crypto
CURL::libcurl
Poco::Net
Poco::Util
Poco::Foundation
Poco::JSON
unofficial::sqlite3::sqlite3
)
