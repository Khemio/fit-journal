#include "FoodCtrl.h"

#include "../dto/food.h"

Task<std::vector<Food>> list_foods() {
    std::vector<Food> fds;
    auto client = app().getDbClient();

    // std::cout << "Before query" << std::endl;
    // auto test_res = co_await client->execSqlCoro("PRAGMA foreign_keys;");
    // std::cout << "result: " << test_res[0][0].as<int>() << std::endl;

    // co_await client->execSqlCoro("PRAGMA foreign_keys = ON;");
    // test_res = co_await client->execSqlCoro("PRAGMA foreign_keys;");
    // std::cout << "result: " << test_res[0][0].as<int>() << std::endl;

    auto result = co_await client->execSqlCoro("SELECT * FROM foods;");

    for (auto row : result) {
        Food fd{
            .ID = row[0].as<unsigned long>(),
            .name = row[1].as<std::string>(),
            .quantity = row[2].as<float>(),
            .quantity_type = row[3].as<std::string>(),
            .protein = row[4].as<float>(),
            .calories = row[5].as<float>()
        };

        fds.push_back(fd);
    }

    co_return fds;
}

// Add definition of your processing function here
Task<HttpResponsePtr> Foods::get_all(HttpRequestPtr req) {
    std::cout << "get_all()" << std::endl;
    HttpViewData data;

    auto fds = co_await list_foods();
    data.insert("foods", fds);
    auto resp = HttpResponse::newHttpViewResponse("foods.csp", data);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_some(HttpRequestPtr req, std::string &&name) {
    // std::cout << "get_some(" << column << ',' << term << ')' << std::endl;
    std::cout << "get_some(" << name << ')' << std::endl;
    HttpViewData data;

    std::vector<Food> fds;
    auto client = app().getDbClient();

    // auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE ? LIKE ?;", column, '%' + name + '%');
    auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE food_name LIKE ?;", '%' + name + '%');

    for (auto row : result) {
        Food fd{
            .ID = row[0].as<unsigned long>(),
            .name = row[1].as<std::string>(),
            .quantity = row[2].as<float>(),
            .quantity_type = row[3].as<std::string>(),
            .protein = row[4].as<float>(),
            .calories = row[5].as<float>()
        };

        fds.push_back(fd);
    }

    data.insert("foods", fds);
    auto resp = HttpResponse::newHttpViewResponse("foods_search.csp", data);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_one(HttpRequestPtr req, unsigned long &&food_id) {
    std::cout << "get_one(" << food_id << ')' << std::endl;
    HttpViewData data;

    std::vector<Food> fds;
    auto client = app().getDbClient();

    // auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE food_id = ?;", food_id);
    auto row = (co_await client->execSqlCoro("SELECT * FROM foods WHERE food_id = ?;", food_id))[0];

    std::cout << "name: " << row[1].as<std::string>() << std::endl;
    // for (auto row : result) {
        Food fd{
            .ID = row[0].as<unsigned long>(),
            .name = row[1].as<std::string>(),
            .quantity = row[2].as<float>(),
            .quantity_type = row[3].as<std::string>(),
            .protein = row[4].as<float>(),
            .calories = row[5].as<float>()
        };

    //     fds.push_back(fd);
    // }

    data.insert("food", fd);
    auto resp = HttpResponse::newHttpViewResponse("food_item.csp", data);

    co_return resp;
}
