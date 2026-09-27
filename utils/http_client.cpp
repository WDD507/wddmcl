#include "http_client.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include "../third_party/libcpr/include/cpr/cpr.h"

cpr::Timeout tmout=cpr::Timeout(60000);

bool HttpClient::succeed(int status_code) {
    return status_code == 200;
}
bool HttpClient::maybe_succeed(int status_code) {
    return (200 <= status_code) && (status_code < 300);
}

HttpClient &HttpClient::instance() {
    static HttpClient inst;
    return inst;
}

std::string HttpClient::getRequest(const std::string& url) {
    std::cout << "[HttpClient] GET request: " << url << std::endl;
    // DONE(2026/6/9 14:40)(2026/7/8 6:30): 实现GET请求
    try {
        cpr::Response res=cpr::Get(
            cpr::Url{url},
            (cpr::Timeout)tmout
        );
        return res.text;
    } catch (const std::exception &e) {
        std::cerr << "Download error: " << e.what() << std::endl;
        return "";
    }
    return "";
}

std::string HttpClient::postRequest(const std::string& url, const std::string& data) {
    std::cout << "[HttpClient] POST request: " << url << std::endl;
    // DONE(2026/6/9 14:45)(2026/7/8 6:30): 实现POST请求
    try {
        cpr::Response res=cpr::Post(
            cpr::Url{url},
            cpr::Body{data},
            (cpr::Timeout)tmout
        );
        return res.text;
    } catch (const std::exception &e) {
        std::cerr << "Download error: " << e.what() << std::endl;
        return "";
    }
    return "";
}

int HttpClient::postRequestStatus(const std::string& url, const std::string& data) {
    std::cout << "[HttpClient] POST request (status): " << url << std::endl;
    try {
        cpr::Response res=cpr::Post(
            cpr::Url{url},
            cpr::Body{data},
            (cpr::Timeout)tmout
        );
        return res.status_code;
    } catch (const std::exception &e) {
        std::cerr << "[HttpClient] POST error: " << e.what() << std::endl;
        return 0;  // 网络异常，状态码 0 表示失败
    }
    return 0;
}

bool HttpClient::downloadFile(const std::string& url, const std::string& savePath) {
    std::cout << "[HttpClient] Downloading: " << url << " to " << savePath << std::endl;
    // DONE(2026/6/9 17:00)(2026/7/8 6:31)(2026/9/6 9:02): 实现HTTP下载功能
    try {
        std::ofstream fout(savePath);
        cpr::Response res=cpr::Download(
            fout, cpr::Url{url},
            (cpr::Timeout)tmout
        );
        //res.downloaded_bytes;
        return maybe_succeed(res.status_code);
    } catch (const std::exception &e) {
        std::cerr << "Download error: " << e.what() << std::endl;
        return false;
    }
    return false;
}

bool HttpClient::downloadFileWithProgress(
    const std::string& url,
    const std::string& savePath,
    std::function<bool(
        cpr::cpr_pf_arg_t, cpr::cpr_pf_arg_t,
        cpr::cpr_pf_arg_t, cpr::cpr_pf_arg_t, intptr_t
    )> progressCallback,
    intptr_t user_data
) {
    std::cout << "[HttpClient] Downloading with progress: " << url << std::endl;
    // DONE(2026/6/9 17:10)(2026/7/8 6:31)(2026/9/6 9:06): 实现带进度的下载
    try {
        std::ofstream fout(savePath);
        cpr::Response res=(
            (user_data != 0) ?
            cpr::Download(
                fout,
                cpr::Url{url},
                cpr::ProgressCallback{
                    progressCallback, user_data
                },
                (cpr::Timeout)tmout
            ) :
            cpr::Download(
                fout,
                cpr::Url{url},
                cpr::ProgressCallback{
                    progressCallback
                },
                (cpr::Timeout)tmout
            )
        );
        return maybe_succeed(res.status_code);
    } catch (const std::exception &e) {
        std::cerr << "Download error: " << e.what() << std::endl;
        return false;
    }
    return false;
}
bool HttpClient::downloadFileWithProgress(
    const std::string& url,
    const std::string& savePath,
    bool(*progressCallback)(
        cpr::cpr_pf_arg_t, cpr::cpr_pf_arg_t,
        cpr::cpr_pf_arg_t, cpr::cpr_pf_arg_t, intptr_t
    ),
    intptr_t user_data
) {
    return downloadFileWithProgress(
        url, savePath,
        std::function<bool(
            cpr::cpr_pf_arg_t, cpr::cpr_pf_arg_t,
            cpr::cpr_pf_arg_t, cpr::cpr_pf_arg_t, intptr_t
        )>(progressCallback),
        user_data
    );
}

void HttpClient::setTimeout(long long seconds) {
    std::cout << "[HttpClient] Setting timeout: " << seconds << "s" << std::endl;
    // DONE(2026/8/31 21:29): 设置超时时间
    tmout = cpr::Timeout(seconds * 1000);
}
void HttpClient::setTimeout(std::chrono::milliseconds mseconds) {
    std::cout << "[HttpClient] Setting timeout: " << mseconds.count()
        << "ms" << std::endl;
    tmout = cpr::Timeout(mseconds);
}
/*cpr::Timeout &getTimeout() {
    return tmout;
}*/
long long HttpClient::getTimeOutSec() {
    return tmout.ms.count() / 1000;
}
