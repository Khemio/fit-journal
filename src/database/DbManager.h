#pragma once
#include <drogon/drogon.h>

#include "../dto/food.h"

using namespace drogon::orm;

class DbManager {
    public:
        DbManager(DbManager &other) = delete;
        void operator=(const DbManager &other) = delete;
        static DbManager* get_instance();

        Task<std::vector<Food>> get_all_foods();
        Task<std::vector<Food>> get_foods_by_name(std::string &name);
        Task<std::vector<Food>> get_food_by_id(unsigned long &food_id);
    private:
        static DbManager* manager;
        DbManager() {};

        static Task<Result> get_from_foods_table() ;
};

DbManager* DbManager::manager = nullptr;

DbManager* DbManager::get_instance() {
    if (manager == nullptr) {
        manager = new DbManager();
    }
    return manager;
}

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