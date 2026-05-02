#include "FoodCtrl.h"

#include "../dto/food.h"
#include "../database/DbManager.h"

HttpResponsePtr render_food(std::string page, bool isFinal, std::vector<Food> fds) {
    HttpViewData data;

    data.insert("isFinal", isFinal);
    data.insert("foods", fds);

    return HttpResponse::newHttpViewResponse(page, data);
}

HttpResponsePtr render_food_page(bool isFinal, std::vector<Food> fds) {
    return render_food("FoodPage", isFinal, fds);
}

HttpResponsePtr render_food_list(bool isFinal, std::vector<Food> fds) {
    return render_food("FoodList", isFinal, fds);
}

Task<HttpResponsePtr> Foods::get_all(HttpRequestPtr req) {
    auto fds = co_await DbManager::get_instance()->get_all_foods();
    auto resp = render_food_page(false, fds);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_some(HttpRequestPtr req, std::string &&name) {
    auto fds = co_await DbManager::get_instance()->get_foods_by_name(name);
    auto resp = render_food_list(false, fds);

    co_return resp;
}

Task<HttpResponsePtr> Foods::get_one(HttpRequestPtr req, unsigned long &&food_id) {
    auto fds = co_await DbManager::get_instance()->get_food_by_id(food_id);
    auto resp = render_food_list(true, fds);

    co_return resp;
}
