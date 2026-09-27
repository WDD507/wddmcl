#ifndef VERSION_UTILS_H
#define VERSION_UTILS_H "version_utils.h"

#include <iostream>
#include <string>
//#include <iosfwd>

typedef struct Version_ {
	int major,minor,micro,complie;
	int setup(int b1v=0, int rv=0, int b2v=0, int cv=0);
    Version_(int mjr, int mnr, int mcr, int cmpl);
    Version_(std::string str_ver);
    Version_(const char str_ver[]);
    Version_();
    ~Version_() = default;
    std::string ver_to_string();
    static std::string to_string(Version_ ver);
    Version_ &operator=(std::string str_ver);
    Version_ &operator=(const char str_ver[]);
    friend bool operator>(Version_ ver1, Version_ ver2);
    friend bool operator<(Version_ ver1, Version_ ver2);
    friend bool operator==(Version_ ver1, Version_ ver2);
    friend bool operator>=(Version_ ver1, Version_ ver2);
    friend bool operator<=(Version_ ver1, Version_ ver2);
    friend bool operator!=(Version_ ver1, Version_ ver2);
    friend std::istream &operator>>(
        std::istream &inputer, Version_ &ver
    );
    friend std::ostream &operator<<(
        std::ostream &outputer, Version_ ver
    );
}Version,*PVersion;
//Version App_Ver;


#endif /* version_utils.h */
