#include "FoodCtrl.h"

#include "../dto/food.h"

Task<std::vector<Food>> list_foods() {
    std::vector<Food> fds;
    auto client = app().getDbClient();
    
    auto result = co_await client->execSqlCoro("SELECT * FROM foods;");

    for (auto row : result) {
        Food fd{row};
        fds.push_back(fd);
    }

    co_return fds;
}

Task<HttpResponsePtr> Foods::get_all(HttpRequestPtr req) {
    std::cout << "get_all()" << std::endl;
    HttpViewData data;

    auto fds = co_await list_foods();

    data.insert("isFinal", false);
    data.insert("foods", fds);

    auto resp = HttpResponse::newHttpViewResponse("FoodPage", data);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_some(HttpRequestPtr req, std::string &&name) {
    std::cout << "get_some(" << name << ')' << std::endl;
    HttpViewData data;

    std::vector<Food> fds;
    auto client = app().getDbClient();
    auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE food_name LIKE ?;", '%' + name + '%');

    for (auto row : result) {
        Food fd{row};
        fds.push_back(fd);
    }

    data.insert("isFinal", false);
    data.insert("foods", fds);
    auto resp = HttpResponse::newHttpViewResponse("FoodList", data);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_one(HttpRequestPtr req, unsigned long &&food_id) {
    std::cout << "get_one(" << food_id << ')' << std::endl;
    HttpViewData data;

    std::vector<Food> fds;
    auto client = app().getDbClient();

    auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE food_id = ?;", food_id);

    for (auto row : result) {
        Food fd{row};
        fds.push_back(fd);
    }

    data.insert("isFinal", true);
    data.insert("foods", fds);
    
    auto resp = HttpResponse::newHttpViewResponse("FoodList", data);

    co_return resp;
}
