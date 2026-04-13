#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

//TODO: List all journals
//TODO: Show one journal
//TODO: Add journal
//TODO: Remove journal?
//? Might be a good idea to limit amount of journals

class Journals : public drogon::HttpController<Journals>
{
    public:
        METHOD_LIST_BEGIN
        // use METHOD_ADD to add your custom processing function here;
        METHOD_ADD(Journals::get_all, "", Get, "AuthFilter");
        METHOD_ADD(Journals::get_all, "/", Get, "AuthFilter");
        // METHOD_ADD(Journals::get_one, "/{journal_id}", Get, "AuthFilter");
        // METHOD_ADD(Journals::create_new, "/new", Post, "AuthFilter");
        // METHOD_ADD(Journals::delete_one, "/{journal_id}/delete", Delete, "AuthFilter");

        METHOD_LIST_END
        // your declaration of processing function maybe like this:
        Task<HttpResponsePtr> get_all(HttpRequestPtr req);
        // Task<HttpResponsePtr> get_one(HttpRequestPtr req, unsigned long &&journal_id);
};