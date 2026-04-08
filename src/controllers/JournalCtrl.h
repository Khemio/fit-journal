#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

//TODO: List all journals
//TODO: Show one journal
//TODO: Add journal
//TODO: Remove journal?

class Journals : public drogon::HttpController<Journals>
{
    public:
        METHOD_LIST_BEGIN
        // use METHOD_ADD to add your custom processing function here;
        // METHOD_ADD(Journals::get_all, "", Get);
        // METHOD_ADD(Journals::get_all, "/", Get);
        // METHOD_ADD(Journals::get_some, "/search?food_name={1}", Get, Post);
        // METHOD_ADD(Journals::get_some, "/{1}", Get);
        // METHOD_ADD(Journals::get_one, "/add/{}", Get);

        METHOD_LIST_END
        // your declaration of processing function maybe like this:
        // Task<HttpResponsePtr> get_all(HttpRequestPtr req);
        // Task<HttpResponsePtr> get_some(HttpRequestPtr req, std::string &&name);
        // Task<HttpResponsePtr> get_one(HttpRequestPtr req, unsigned long &&food_id);
};