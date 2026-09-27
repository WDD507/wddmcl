#include <iostream>
//#include "md5_utils.h"

class HashUtils {
public:
    HashUtils() = delete;
    HashUtils(const HashUtils &) = delete;
    HashUtils(HashUtils &&) = delete;
    HashUtils &operator=(const HashUtils &) = delete;
    HashUtils &operator=(HashUtils &&) = delete;

    static std::string calculateMD5(const std::string& input);
    static std::string calculateSHA256(const std::string& input);
};
