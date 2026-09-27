//#include <iostream>
#include "mutually.h"
#include "../account/account_manager.h"
//#include "../minecraft/game_launcher.h"
#include "../launcher.h"
using namespace std;

namespace {
    AccountManager &accmgr=AccountManager::instance();
}
extern Launcher launcher;
extern "C" ret_t AWTKLaunchGame(const char version[], const char account[]) {
    //launcher = Launcher(true);
    return (
        launcher.launchGame(version, account) ?
        RET_OK : RET_FAIL
    );
    /*GameLauncher game_launcher(0, 0);
    return (
        game_launcher.launchGame(
            version, accmgr.getAccountFromName(account)
        ) ? RET_OK : RET_FAIL
    );*/
}
