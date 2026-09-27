# CPR Network Static Library (MinGW GCC)

C++ HTTP client library based on libcurl, statically compiled for MinGW GCC.

## Usage

### Compilation Command
```bash
g++ your_code.cpp -o your_app.exe -std=c++17 -Iinclude/cpr -Iinclude -Llib -lcpr -lcurl -lm
```

### Runtime Dependency
Copy `bin/libcurl-x64.dll` to the same directory as your executable.

### Header Inclusion
```cpp
#include "cpr.h"
```

## Directory Structure
```
cpr-network-static/
├── include/
│   ├── cpr/          (37 headers)
│   └── curl/         (11 headers)
├── lib/
│   ├── libcpr.a      (CPR static library)
│   └── libcurl.a     (libcurl import library)
├── bin/
│   └── libcurl-x64.dll (runtime DLL)
├── examples/
│   └── test_cpr.cpp  (usage example)
├── build_test.bat    (build script)
└── README.txt        (this file)
```

## Compilation Environment
- MinGW64 GCC (g++)
- C++17 Standard

## Notes
- This package uses libcurl DLL at runtime
- For pure static linking without DLL dependency, compile libcurl from source with MinGW
