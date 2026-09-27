#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>
#include "version_utils.h"
using namespace std;

int Version::setup(int b1v, int rv, int b2v, int cv) {
    this->major = b1v;
    this->minor = rv;
    this->micro = b2v;
    this->complie = cv;
    return 0;
}
Version_::Version_(int mjr, int mnr, int mcr, int cmpl) {
    setup(mjr, mnr, mcr, cmpl);
}
Version_::Version_(string str_ver) {
    int b1v=0,rv=0,b2v=0,cv=0;
    sscanf(
        str_ver.c_str(), "%d.%d.%d.%d",
        &b1v, &rv, &b2v, &cv
    );
    setup(b1v, rv, b2v, cv);
}
Version_::Version_(const char str_ver[]) {
	(*this) = str_ver;
}
Version_::Version_() {
	*this = Version_(0, 0, 0, 0);
}
string Version_::ver_to_string() {
    static char buffer[1024]="";
	sprintf(
		buffer, "%d.%d.%d.%d",
        this->major, this->minor, this->micro, this->complie
	);
	return string(buffer);
}
std::string Version_::to_string(Version_ ver) {
	return ver.ver_to_string();
}
Version_ &Version_::operator=(string str_ver) {
	Version_ &ver_ref=(*this);
	ver_ref = Version_(str_ver);
	return ver_ref;
}
Version_ &Version_::operator=(const char str_ver[]) {
	return (*this) = string(str_ver);
}

bool operator>(Version_ ver1, Version_ ver2) {
	if(ver1.major > ver2.major){
		return true;
	}else if(ver1.major == ver2.major){
		if(ver1.minor > ver2.minor){
			return true;
		}else if(ver1.minor == ver2.minor){
			if(ver1.micro > ver2.micro){
				return true;
			}else{
				return false;
			}
		}
	}
	return false;
}
bool operator<(Version_ ver1, Version_ ver2) {
	if(ver1.major < ver2.major){
		return true;
	}else if(ver1.major == ver2.major){
		if(ver1.minor < ver2.minor){
			return true;
		}else if(ver1.minor == ver2.minor){
			if(ver1.micro < ver2.micro){
				return true;
			}else{
				return false;
			}
		}
	}
	return false;
}
bool operator==(Version_ ver1, Version_ ver2) {
	/*if(ver1.big_ver == ver2.big_ver){
		if(ver1.re_ver == ver2.re_ver){
			if(ver1.bug_ver == ver2.bug_ver){
				return true;
			}else{
				return false;
			}
		}else{
			return false;
		}
	}else{
		return false;
	}*/
	return (
        ver1.major == ver2.major &&
        ver1.minor == ver2.minor &&
        ver1.micro == ver2.micro
    );
}
bool operator>=(Version_ ver1, Version_ ver2) {
    return (ver1 > ver2) || (ver1 == ver2);
}
bool operator<=(Version_ ver1, Version_ ver2) {
    return (ver1 < ver2) || (ver1 == ver2);
}
bool operator!=(Version_ ver1, Version_ ver2) {
    return !(ver1 == ver2);
}
istream &operator>>(istream &inputer, Version_ &ver) {
	/*scanf(
        "%d.%d.%d.%d",
        &ver.big_ver, &ver.re_ver, &ver.bug_ver, &ver.complie_ver
    );*/
    static string buffer="";
    inputer >> buffer;
    ver = Version_(buffer);
	return inputer;
}
ostream &operator<<(ostream &outputer, Version_ ver) {
	/*if((int)outputer == (int)cerr){
		cerr << "\a";
	}*/
	/*printf(
        "%d.%d.%d.%d",
        ver.big_ver, ver.re_ver, ver.bug_ver, ver.complie_ver
    );*/
	outputer << ver.ver_to_string();
	return outputer;
}

int test_ver() {
	cout << (Version(1, 18, 0, 1) >= "1.18.0.1")
		<< endl;
	Version();
	return 0;
}
