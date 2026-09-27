#ifndef JAVA_DETECTOR_H
#define JAVA_DETECTOR_H

#include <string>
#include <vector>

struct JavaInfo {
    std::string path;            // Java路径
    int version;                 // Java版本 (8, 11, 17, 21)
    std::string vendor;          // 供应商 (Oracle/OpenJDK/Microsoft)
    bool isValid;                // 是否有效
};

class JavaDetector {
public:
    // 检测系统中的Java安装
    std::vector<JavaInfo> detectJavaInstallations();
    
    // 验证Java版本是否符合要求
    bool validateJavaVersion(const std::string& javaPath, int requiredVersion);
    
    // 根据Minecraft版本获取推荐的Java路径
    std::string getRecommendedJavaPath(int minecraftVersion);
    
    // 下载并安装Java（需要HTTP支持）
    bool downloadJava(int version, const std::string& installPath);
    
private:
    // 从路径中检测版本号
    int detectVersionFromPath(const std::string& path);
    
    // 从路径中提取供应商名称
    std::string extractVendor(const std::string& path);
    
    // 从java -version输出中解析版本号
    int parseVersionFromString(const std::string& output);
};

#endif // JAVA_DETECTOR_H
