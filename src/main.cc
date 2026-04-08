#include <print>
#include <iostream>

// #include <windows.h>
// #include "../../vendor/Bcrypt.cpp/include/bcrypt.h"
// #include "../../vendor/Bcrypt.cpp/include/bcrypt-hash.h"
// #pragma comment (lib, "bcrypt.lib")

#include <drogon/drogon.h>

using namespace drogon;

int main() {
    app().loadConfigFile("../config.json");
    
    app()
        .enableSession(12000)
        .run();
    return 0;
}
