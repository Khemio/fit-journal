#pragma once

using namespace drogon::orm;

struct User {
    unsigned long ID;
    std::string username;
    std::string password;

    User () {
        ID = 0;
        username = "";
        password = "";
    }

    User(Row row) {
        ID = row[0].as<unsigned long>();
        username = row[1].as<std::string>();
        password = row[2].as<std::string>();
    }
};