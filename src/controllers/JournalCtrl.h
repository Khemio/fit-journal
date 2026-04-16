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
        METHOD_ADD(Journals::get_all, "", Get, "AuthFilter");
        METHOD_ADD(Journals::get_all, "/", Get, "AuthFilter");

        METHOD_ADD(Journals::get_journal, "/{journal_id}", Get, "AuthFilter");
        // METHOD_ADD(Journals::create_journal, "/new", Post, "AuthFilter");
        // METHOD_ADD(Journals::delete_journal, "/{journal_id}", Delete, "AuthFilter");

        METHOD_ADD(Journals::get_entry, "/{journal_id}/{entry_id}", Get, "AuthFilter");
        // METHOD_ADD(Journals::create_entry, "/{journal_id}/new", Post, "AuthFilter");
        // METHOD_ADD(Journals::delete_entry, "/{journal_id}/{entry_id}", Delete, "AuthFilter");

        // METHOD_ADD(Journals::create_entry_item, "/{journal_id}/{entry_id}/new", Post, "AuthFilter");
        METHOD_ADD(Journals::calc_food_item, "/new-food-item/calc/{}", Post, "AuthFilter");
        METHOD_ADD(Journals::add_food_item, "/new-food-item/add/{}", Post, "AuthFilter");
        METHOD_LIST_END

        Task<HttpResponsePtr> get_all(HttpRequestPtr req);
        Task<HttpResponsePtr> get_journal(HttpRequestPtr req, unsigned long &&journal_id);
        Task<HttpResponsePtr> get_entry(HttpRequestPtr req, unsigned long &&journal_id, unsigned long &&entry_id);

        Task<HttpResponsePtr> calc_food_item(HttpRequestPtr req, unsigned long &&food_id);
        Task<HttpResponsePtr> add_food_item(HttpRequestPtr req, unsigned long &&food_id);

        // Task<HttpResponsePtr> create_entry_item(HttpRequestPtr req, unsigned long &&journal_id, unsigned long &&entry_id);
};