#include <iostream>
#include <ostream>
#include <string>
#include "app.h"
#include "launcher.h"
//#define USE_AWTK "Used AWTK"
#ifdef USE_AWTK
#include <cstdio>
#include <cstddef>
#include "third_party/awtk/include_flat/awtk.h"
#include "third_party/awtk/include_flat/awtk_global.h"
#include "third_party/awtk/include_flat/base_types_def.h"
#include "third_party/awtk/include_flat/base_window_manager.h"
#include "third_party/awtk/include_flat/tkc_types_def.h"
#include "src/application.h"
#include "src/common/navigator.h"
#endif
using namespace std;

#ifdef USE_AWTK
/*
// 页面声明
extern widget_t* home_page_create(widget_t* parent, const void* data);
extern widget_t* download_page_create(widget_t* parent, const void* data);
extern widget_t* settings_page_create(widget_t* parent, const void* data);
extern widget_t* more_page_create(widget_t* parent, const void* data);
*/
// 使用 navigator_to 进行页面导航
// 页面初始化由 src/application.c 和 src/common/navigator.c 处理
ret_t app_init() {
    /*
    // 注册页面
    window_manager_add_window(
        "home_page", home_page_create, NULL
    );
    window_manager_add_window(
        "download_page", download_page_create, NULL
    );
    window_manager_add_window(
        "settings_page", settings_page_create, NULL
    );
    window_manager_add_window(
        "more_page", more_page_create, NULL
    );
    */
    // 打开首页
    //navigator_to("home_page");
    return application_init();
}
ret_t GUI_init() {
    int InitCode=(int)RET_OK;
    InitCode |= (int)tk_pre_init();
    InitCode |= (int)tk_init(800, 600, APP_DESKTOP,
        "WDD's Minecraft Launcher",
        NULL
    );
    InitCode |= (int)app_init();
    return ret_t(InitCode);
}
#endif

App app;
Launcher launcher;
//int command();
int main(int argc, char* argv[]) {
    for (int i=0; i<argc; i++) {
        args.push_back(string(argv[i]));
    }
//    command();
#ifdef USE_AWTK
    freopen("latest.log", "w", stdout);
    freopen("latest.log", "a", stderr);
#endif
    cout << "========================================" << endl;
#ifdef USE_AWTK // GUI Logger
    cout << "     WDD's Minecraft Launcher (Log)     " << endl;
#else // Console
    cout << "WDD's Minecraft Launcher (Console Version) " << endl;
#endif
    cout << "========================================" << endl;
    cout << "Version: " << App::AppVersion << endl;
    cout << endl;

    // 创建启动器实例
    launcher = Launcher();

    // 初始化启动器
    cout << "[Main] Initializing launcher..." << endl;
    if (!launcher.initialize()) {
        cerr << "[Main] Failed to initialize launcher!" << endl;
        cout << "========================================" << endl;
        cout << "[Main] Program finished." << endl;
        cout << "========================================" << endl;
        return 1;
    }
    
    cout << endl;
    cout << "[Main] Launcher status: " << launcher.getStatus() << endl;
    cout << endl;

#ifdef USE_AWTK // AWTK GUI
    GUI_init();
    tk_run();
    tk_exit();
#else // Console
    cout << "[Main] You can press \"launch\" to start the game,\n"
         << "or press \"exit\" to quit the program." << endl;
    string condition="";
    while (condition != "exit") {
        cout << "[Main] Please enter your choice: ";
        cin >> condition;
        if(condition == ""){
            //
        }else if(condition == "help"){
            cout <<
"=== 账户管理 ===\n"
"account list              列出所有账户\n"
"account add <用户名> <类型>   添加账户（类型: offline/mojang/littleskin）\n"
"account remove <用户名>      删除账户\n"
"account select <用户名>      选择当前账户\n"
"account login <用户名> <密码> <类型>  登录账户（外置登录）\n"
"account current           显示当前账户\n"

"=== 版本管理 ===\n"
"version list              列出已安装版本\n"
"version remote            列出远程可用版本\n"
"version download <版本号>  下载指定版本\n"
"version info <版本号>      显示版本详细信息\n"
"version remove <版本号>     删除版本\n"
"\n"
"=== 游戏启动 ===\n"
"launch <版本号>            启动游戏（用当前账户）\n"
"launch <版本号> <用户名>    启动游戏（指定账户）\n"
"\n"
"=== Java ===\n"
"java detect               检测已安装的 Java\n"
"java path                 显示当前 Java 路径\n"
"java set <路径>            设置 Java 路径\n"
"\n"
"=== 配置 ===\n"
"config show              显示当前配置\n"
"config set <键> <值>       设置配置项\n"
"\n"
"=== 其他 ===\n"
"update                    检查更新\n"
"status                    显示启动器状态\n"
"help                      显示帮助\n"
"exit                      退出\n"
            << endl;
        }else if(condition == "launch"){
            // 测试启动游戏（示例）
            cout << "[Main] Testing game launch..." << endl;
            if(launcher.launchGame("1.20.1", "Player")){
                cout << "[Main] Game launched successfully!" << endl;
            }else{
                cerr << "[Main] Failed to launch game!" << endl;
                return 1;
            }
        }else if(condition == "exit"){
            cout << "[Main] Exiting program..." << endl;
            break;
        }else{
            cout << "[Main] Invalid choice!" << endl;
        }
    }
#endif
    cout << endl;
    cout << "========================================" << endl;
    cout << "[Main] Program finished." << endl;
    cout << "========================================" << endl;
    
    return 0;
}
