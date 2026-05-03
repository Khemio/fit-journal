#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class Foods : public HttpController<Foods>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Foods::get_all, "", Get);
    METHOD_ADD(Foods::get_all, "/", Get);
    METHOD_ADD(Foods::get_some, "/search?food_name={1}", Get);
    METHOD_ADD(Foods::get_some, "/name/{}", Get);
    METHOD_ADD(Foods::get_one, "/id/{}", Get);

    METHOD_LIST_END

    Task<HttpResponsePtr> get_all(HttpRequestPtr req);
    Task<HttpResponsePtr> get_some(HttpRequestPtr req, std::string &&name);
    Task<HttpResponsePtr> get_one(HttpRequestPtr req, unsigned long &&food_id);
};

