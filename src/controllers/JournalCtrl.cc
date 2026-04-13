#include "JournalCtrl.h"

#include "../dto/journal.h"
#include "../dto/food_journal_entry.h"

//TODO: Add authentication for DB rows
/*
    Request can be changed client-side, here is an example:
    let item =  document.getElementById("journal-item")
    item.setAttribute("hx-get", "/journals/2");
    htmx.process(item);
*/

Task<HttpResponsePtr> Journals::get_all(HttpRequestPtr req) {
    std::cout << "journals: get_all()" << std::endl;

    HttpViewData data;
    std::vector<Journal> jrs;

    auto user_id = req->session()->getOptional<unsigned long>("ID").value_or(0);
    auto client = app().getDbClient();
    auto result = co_await client->execSqlCoro("SELECT * FROM journals WHERE owner_id = ?;", user_id);

    //? Add more info to the journal (Last Entrie Date, Avg stats, etc.)

    for (auto row : result) {
        Journal jr{
            .ID = row[0].as<unsigned long>(),
            .owner_id = row[1].as<unsigned long>(),
            .name = row[2].as<std::string>(),
            .type = row[3].as<std::string>(),
            .description = row[4].as<std::string>()
        };

        jrs.push_back(jr);
    }

    data.insert("journals", jrs);

    auto resp = HttpResponse::newHttpViewResponse("journals", data);

    co_return resp;
}

Task<HttpResponsePtr> Journals::get_journal(HttpRequestPtr req, unsigned long &&journal_id) {
    std::cout << "journals: get_journal()" << std::endl;
    HttpViewData data;
    std::vector<Entry> ents;

    // auto user_id = req->session()->getOptional<unsigned long>("ID").value_or(0);
    auto client = app().getDbClient();

    auto result = co_await client->execSqlCoro("SELECT * FROM food_entries WHERE journal_id = $1;", journal_id);

    for (auto row : result) {
        Entry ent{
            .entry_id = row[0].as<unsigned long>(),
            .journal_id = row[1].as<unsigned long>(),
            .date = row[2].as<std::string>(),
            .total_protein = row[3].as<float>(),
            .total_calories = row[4].as<float>(),
            // .items = std::vector<Item>
        };

        ents.push_back(ent);
    }

    data.insert("journal_id", journal_id);
    data.insert("entries", ents);

    auto resp = HttpResponse::newHttpViewResponse("journal_entries", data);

    co_return resp;
}

Task<HttpResponsePtr> Journals::get_entry(HttpRequestPtr req, unsigned long &&journal_id, unsigned long &&entry_id) {
    std::cout << "journals: get_entry()" << std::endl;
    HttpViewData data;
    std::vector<Item> items;

    // auto user_id = req->session()->getOptional<unsigned long>("ID").value_or(0);
    auto client = app().getDbClient();

    try {

        auto result = co_await client->execSqlCoro("SELECT * FROM food_entry_items WHERE entry_id = $1;", entry_id);

        for (auto row : result) {
            Item item{
                .item_id = row[0].as<unsigned long>(),
                .entry_id = row[1].as<unsigned long>(),
                .name = row[2].as<std::string>(),
                // .type = row[3].as<std::string>(),
                .quantity = row[3].as<float>(),
                .quantity_type = row[4].as<std::string>(),
                .protein = row[5].as<float>(),
                .calories = row[6].as<float>(),
            };

            items.push_back(item);
        }
    } catch (orm::DrogonDbException &e) {
        std::cout << e.base().what() << std::endl;
    }

    data.insert("journal_id", journal_id);
    data.insert("entry_id", entry_id);
    data.insert("items", items);

    auto resp = HttpResponse::newHttpViewResponse("food_journal_entry", data);

    co_return resp;
}