#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "account_manager.h"
#include <string>

class AuthService {
public:
    // 离线模式认证
    bool authenticateOffline(const std::string& username);

    // Yggdrasil协议统一认证（Mojang/LittleSkin/authlib-injector 兼容）
    bool yggdrasil(
        const std::string &url,
        const std::string &email,
        const std::string &password
    );

    // Mojang账户认证
    bool authenticateMojang(
        const std::string &email,
        const std::string &password
    );

    // LittleSkin认证
    bool authenticateLittleSkin(
        const std::string &email,
        const std::string &password
    );
    
    // 微软账户认证
    bool authenticateMicrosoft(
        const std::string& email, const std::string& password
    ) = delete;
    
    // 刷新令牌
    //bool refreshToken(const std::string& refreshTokenS);
    bool refreshToken(Account &acc);
    bool refreshToken(const std::string &username);
    bool refreshToken();
    
    // 获取访问令牌
    std::string getAccessToken() const;
    
    // 验证令牌有效性
    bool isTokenValid();
};

#endif // AUTH_SERVICE_H
