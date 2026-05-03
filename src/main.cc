#include <iostream>

#include <drogon/drogon.h>

using namespace drogon;

int main() {
    app().loadConfigFile("../config.json");
    
    app()
        .enableSession(12000)
        .run();
    return 0;
}
