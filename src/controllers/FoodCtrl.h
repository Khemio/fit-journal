#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

//TODO: Add foods
//TODO: Add foods to journal entries
//TODO: Remove foods

class Foods : public drogon::HttpController<Foods>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(Food::get, "/{2}/{1}", Get); // path is Foods/{arg2}/{arg1}
    // METHOD_ADD(Food::your_method_name, "/{1}/{2}/list", Get); // Foods/{arg1}/{arg2}/list
    // ADD_METHOD_TO(Food::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    METHOD_ADD(Foods::get_all, "", Get);
    METHOD_ADD(Foods::get_all, "/", Get);
    METHOD_ADD(Foods::get_some, "/search?food_name={1}", Get);
    METHOD_ADD(Foods::get_some, "/name/{}", Get);
    METHOD_ADD(Foods::get_one, "/id/{}", Get);

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    Task<HttpResponsePtr> get_all(HttpRequestPtr req);
    Task<HttpResponsePtr> get_some(HttpRequestPtr req, std::string &&name);
    Task<HttpResponsePtr> get_one(HttpRequestPtr req, unsigned long &&food_id);
};

