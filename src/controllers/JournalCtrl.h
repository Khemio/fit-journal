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
        METHOD_ADD(Journals::get_journal, "/{journal_id}", Get, "AuthFilter");
        // METHOD_ADD(Journals::create_journal, "/new", Post, "AuthFilter");
        // METHOD_ADD(Journals::delete_journal, "/{journal_id}", Delete, "AuthFilter");
        METHOD_ADD(Journals::get_entry, "/{journal_id}/{entry_id}", Get, "AuthFilter");
        // METHOD_ADD(Journals::create_entry, "/{journal_id}/new", Post, "AuthFilter");
        // METHOD_ADD(Journals::delete_entry, "/{journal_id}/{entry_id}", Delete, "AuthFilter");

        METHOD_LIST_END
        // your declaration of processing function maybe like this:
        Task<HttpResponsePtr> get_all(HttpRequestPtr req);
        Task<HttpResponsePtr> get_journal(HttpRequestPtr req, unsigned long &&journal_id);
        Task<HttpResponsePtr> get_entry(HttpRequestPtr req, unsigned long &&journal_id, unsigned long &&entry_id);
};