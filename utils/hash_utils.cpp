//#include <iostream>
#include <string>
#include "./hash_utils.h"
#include "md5_utils.h"
using namespace std;

string HashUtils::calculateMD5(const std::string &input) {
    return MD5Utils::calculateMD5(input);
}
