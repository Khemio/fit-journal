#pragma once
#include <drogon/drogon.h>

// #include "../dto/user.h"
#include "../dto/food.h"

using namespace drogon::orm;

class DbManager {
    public:

        static Task<std::vector<Food>> get_all_foods();
        static Task<std::vector<Food>> get_foods_by_name(std::string &name);
        static Task<std::vector<Food>> get_food_by_id(unsigned long &food_id);

        // static Task<void> add_user(std::string user, std::string hash);
        // static Task<User> get_user(std::string user);   

    private:
        static Task<Result> get_from_foods_table() ;
};

Task<Result> DbManager::get_from_foods_table() {
    auto client = app().getDbClient();
    
    auto result = co_await client->execSqlCoro("SELECT * FROM foods;");
    co_return result;
}

Task<std::vector<Food>> DbManager::get_all_foods() {
    auto result = co_await get_from_foods_table();
    std::vector<Food> fds;

    for (auto row : result) {
        Food fd{row};
        fds.push_back(fd);
    }

    co_return fds;
}

Task<std::vector<Food>> DbManager::get_foods_by_name(std::string &name) {
    auto client = app().getDbClient();

    auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE food_name LIKE ?;", '%' + name + '%');
    std::vector<Food> fds;

    for (auto row : result) {
        Food fd{row};
        fds.push_back(fd);
    }

    co_return fds;
}

Task<std::vector<Food>> DbManager::get_food_by_id(unsigned long &food_id) {
    auto client = app().getDbClient();

    auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE food_id = ?;", food_id);
    std::vector<Food> fds;

    for (auto row : result) {
        Food fd{row};
        fds.push_back(fd);
    }

    co_return fds;
}

// Task<void> DbManager::add_user(std::string user, std::string hash) {
//     auto client = app().getDbClient();
//     co_await client->execSqlCoro(
//         "INSERT INTO users (username, password) VALUES ($1, $2);", 
//         user, hash);
// }

// Task<User> DbManager::get_user(std::string user) {
//     auto client = app().getDbClient();
//     auto result = co_await client->execSqlCoro("SELECT * FROM users WHERE username = ?;", user);

//     if (result.size() > 0) { //? Size probably should be 1 for a singular row
//         User user{result[0]};
//         co_return user;
//     } else {
//         co_return User{};
//     }

// }