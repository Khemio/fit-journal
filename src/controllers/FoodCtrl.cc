#include "FoodCtrl.h"

#include "../dto/food.h"
#include "../database/DbManager.h"


Task<HttpResponsePtr> Foods::get_all(HttpRequestPtr req) {
    auto fds = co_await DbManager::get_all_foods();
    HttpViewData data;

    data.insert("isFinal", false);
    data.insert("foods", fds);

    auto resp = HttpResponse::newHttpViewResponse("FoodPage", data);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_some(HttpRequestPtr req, std::string &&name) {
    auto fds = co_await DbManager::get_foods_by_name(name);
    HttpViewData data;

    data.insert("isFinal", false);
    data.insert("foods", fds);

    auto resp = HttpResponse::newHttpViewResponse("FoodList", data);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_one(HttpRequestPtr req, unsigned long &&food_id) {
    auto fds = co_await DbManager::get_food_by_id(food_id);
    HttpViewData data;

    data.insert("isFinal", true);
    data.insert("foods", fds);
    
    auto resp = HttpResponse::newHttpViewResponse("FoodList", data);

    co_return resp;
}
