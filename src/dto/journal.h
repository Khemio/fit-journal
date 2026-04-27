#pragma once
#include <drogon/drogon.h>

using namespace drogon::orm;

struct Journal {
    unsigned long ID;
    unsigned long owner_id;
    bool is_active;
    std::string type;

    Journal() {
        ID = 0;
        owner_id = 0;
        is_active = false;
        type = "";
    }

    Journal(Row row) {
        ID = row[0].as<unsigned long>();
        owner_id = row[1].as<unsigned long>();
        is_active = row[2].as<bool>();
        type = row[3].as<std::string>();
    }
};