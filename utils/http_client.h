#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <vector>

//typedef long long cpr_pf_arg_t;
/*namespace cpr {
    typedef long long cpr_pf_arg_t;
}*/

class HttpClient {
public:
    HttpClient() = default;
    HttpClient(const HttpClient &) = delete;
    HttpClient(HttpClient &&) = delete;
    HttpClient &operator=(const HttpClient &) = delete;
    HttpClient &operator=(HttpClient &&) = delete;

    static HttpClient &instance();

    // 
    bool succeed(int status_code=200);
    bool maybe_succeed(int status_code=200);

    // GET请求
    std::string getRequest(const std::string& url);
    
    // POST请求
    std::string postRequest(const std::string& url, const std::string& data);

    // POST请求（返回 HTTP 状态码，用于 /validate 这类只看状态码的接口）
    int postRequestStatus(const std::string& url, const std::string& data);
    
    // 下载文件
    bool downloadFile(const std::string& url, const std::string& savePath);
    
    // 下载文件（带进度回调）
    bool downloadFileWithProgress(
        const std::string& url,
        const std::string& savePath,
        std::function<bool(
            long long, long long, long long, long long, intptr_t
        )> progressCallback=NULL,
        intptr_t user_data=0
    );
    bool downloadFileWithProgress(
        const std::string& url,
        const std::string& savePath,
        bool(*progressCallback)(
            long long, long long, long long, long long, intptr_t
        )=NULL,
        intptr_t user_data=0
    );
    
    // 设置超时
    void setTimeout(long long seconds);
    void setTimeout(std::chrono::milliseconds ms);
    long long getTimeOutSec();
};

#endif // HTTP_CLIENT_H
