#include "java_runtime.h"
//#include "java_detector.h"
#include "../config/config_manager.h"
#include <iostream>
#include <cstdlib>
#include <string>

void JavaRuntime::setJavaPath(const std::string& path) {
    std::cout << "[JavaRuntime] Setting Java path: " << path << std::endl;
    // DONE(2026/9/5 13:38): 保存Java路径
    CfgMgr.setJavaPath(path);
}

int JavaRuntime::getJavaVersion(const std::string& javaPath) {
    std::cout << "[JavaRuntime] Getting Java version for: " << javaPath << std::endl;
    // DONE(2)(2026/6/13 9:14): 执行java -version获取版本
    //system(std::string(javaPath + " -version").c_str());
    /*JavaDetector java_detector;
    java_detector.validateJavaVersion(javaPath);*/
        // 执行 java -version 获取版本信息
    std::string command = "\"" + javaPath + "\" -version 2>&1";
    
#ifdef _WIN32
    // Windows: 使用_popen执行命令
    FILE* pipe = _popen(command.c_str(), "r");
#else
    // Linux/macOS: 使用popen
    FILE* pipe = popen(command.c_str(), "r");
#endif
    
    if (!pipe) {
        std::cerr << "[JavaDetector] Failed to execute java -version" << std::endl;
        return false;
    }
    
    char buffer[256];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    // 解析版本号（例如：openjdk version "17.0.1"）
    int detectedVersion = ([](const std::string &output) -> int{
        try {
            size_t quoteStart = output.find('"');
            if (quoteStart != std::string::npos) {
                size_t quoteEnd = output.find('"', quoteStart + 1);
                if (quoteEnd != std::string::npos) {
                    std::string versionStr = output.substr(
                        quoteStart + 1, quoteEnd - quoteStart - 1
                    );
                    size_t dotPos = versionStr.find('.');
                    if (dotPos != std::string::npos) {
                        return std::stoi(versionStr.substr(0, dotPos));
                    }
                    return std::stoi(versionStr);
                }
            }
        } catch (...) {
            // 解析失败
        }
        return 0;
    /* 旧代码（bug）：误传 javaPath 而非命令输出 output，导致版本号解析拿不到实际输出
    })(javaPath);//JavaDetector::parseVersionFromString(output);
    */
    })(output);
    return detectedVersion;
}

std::vector<std::string> JavaRuntime::buildJvmArgs(
    int memoryMB,
    const std::vector<std::string>& extraArgs
) {
    std::cout << "[JavaRuntime] Building JVM args with " << memoryMB << "MB memory"
     << std::endl;
    
    std::vector<std::string> args;
    args.push_back("-Xmx" + std::to_string(memoryMB) + "m");
    args.push_back("-Xms" + std::to_string(memoryMB / 2) + "m");
    
    // 添加额外参数
    args.insert(args.end(), extraArgs.begin(), extraArgs.end());
    /*for (const auto& arg : extraArgs) {
        args.push_back(arg);
    }*/
    
    return args;
}
