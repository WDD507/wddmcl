#ifndef JAVA_RUNTIME_H
#define JAVA_RUNTIME_H

#include <string>
#include <vector>
//#include "./java_detector.h"

class JavaRuntime {
public:
    // 设置Java路径
    void setJavaPath(const std::string& path);
    
    // 获取Java版本
    int getJavaVersion(const std::string& javaPath);
    
    // 构建JVM参数
    std::vector<std::string> buildJvmArgs(
        int memoryMB,
        const std::vector<std::string>& extraArgs
    );
};

#endif // JAVA_RUNTIME_H
