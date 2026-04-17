#include "JournalCtrl.h"

#include "../dto/journal.h"
#include "../dto/food.h"
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
    auto result = co_await client->execSqlCoro("SELECT * FROM journals WHERE active = 1 and owner_id = ?;", user_id);

    //? Add more info to the journal (Last Entrie Date, Avg stats, etc.)

    for (auto row : result) {
        Journal jr{row};
        jrs.push_back(jr);
    }

    data.insert("journals", jrs);

    auto resp = HttpResponse::newHttpViewResponse("JournalPage", data);

    co_return resp;
}

Task<HttpResponsePtr> Journals::get_journal(HttpRequestPtr req, unsigned long &&journal_id) {
    std::cout << "journals: get_journal()" << std::endl;
    HttpViewData data;
    std::vector<Entry> ents;

    auto client = app().getDbClient();

    auto query = R"(SELECT * FROM food_entries WHERE journal_id = $1
                    ORDER BY entry_date DESC;)";

    auto result = co_await client->execSqlCoro(query, journal_id);

    for (auto row : result) {
        Entry ent{row};
        ents.push_back(ent);
    }

    data.insert("journal_id", journal_id);
    data.insert("entries", ents);

    auto resp = HttpResponse::newHttpViewResponse("FoodJournal", data);

    co_return resp;
}

Task<HttpResponsePtr> Journals::get_entry(HttpRequestPtr req, unsigned long &&journal_id, unsigned long &&entry_id) {
    std::cout << "journals: get_entry()" << std::endl;

    HttpViewData data;
    std::vector<Item> items;
    float tp = 0;
    float tc = 0;

    // auto user_id = req->session()->getOptional<unsigned long>("ID").value_or(0);
    auto client = app().getDbClient();

    try {

        auto result = co_await client->execSqlCoro("SELECT * FROM food_entry_items WHERE entry_id = $1;", entry_id);

        for (auto row : result) {
            Item item{row};

            tp += item.protein;
            tc += item.calories;

            items.push_back(item);
        }
    } catch (orm::DrogonDbException &e) {
        std::cout << e.base().what() << std::endl;
    }

    data.insert("journal_id", journal_id);
    data.insert("entry_id", entry_id);
    data.insert("total_protein", tp);
    data.insert("total_calories", tc);
    data.insert("items", items);

    auto resp = HttpResponse::newHttpViewResponse("FoodJournalEntry", data);

    co_return resp;
}

Task<HttpResponsePtr> Journals::create_entry(HttpRequestPtr req, unsigned long &&journal_id) {
    std::cout << "create_entry(" << journal_id << ")" << std::endl;
    HttpResponsePtr resp = HttpResponse::newHttpResponse();

    auto client = app().getDbClient();

    try {
        auto entry_date = req->getParameter("entry-date");

        float total_protein = 0;
        float total_calories = 0;

        auto query = R"(INSERT INTO food_entries (journal_id, entry_date, total_protein, total_calories)
                        VALUES ($1, $2, $3, $4);)";

        auto result = co_await client->execSqlCoro(query, journal_id, entry_date, total_protein, total_calories);

        // std::cout << "rows affected: " << result.affectedRows() << std::endl;

        std::string header = "{\"itemAdded\":{\"target\" : \"#entries-container\"}}";

        resp->setStatusCode(k201Created);
        resp->addHeader("HX-Trigger", header);
    } catch (std::invalid_argument &e) {
        resp->setStatusCode(k302Found);
        resp->setBody("<script>alert('Failed to add: invalid input')</script>");
    }

    co_return resp;
}

Task<HttpResponsePtr> Journals::calc_food_item(HttpRequestPtr req, unsigned long &&food_id) {
    std::cout << "calc_food_item(" << food_id << ')' << std::endl;
    auto amount = std::stof(req->getParameter("amount"));
    HttpViewData data;

    std::vector<Food> fds;
    auto client = app().getDbClient();

    auto result = co_await client->execSqlCoro("SELECT * FROM foods WHERE food_id = ?;", food_id);

    for (auto row : result) {
        Food fd{row, amount};
        fds.push_back(fd);
    }

    data.insert("isFinal", true);
    data.insert("foods", fds);
    
    auto resp = HttpResponse::newHttpViewResponse("FoodList", data);

    co_return resp;
}

Task<HttpResponsePtr> Journals::add_food_item(HttpRequestPtr req, unsigned long &&food_id) {
    std::cout << "add_food_item(" << food_id << ")" << std::endl;
    HttpResponsePtr resp = HttpResponse::newHttpResponse();

    auto client = app().getDbClient();

    try {
        auto amount = std::stof(req->getParameter("amount"));
        auto entry_id = std::stoul(req->getParameter("entry-id"));
        auto row = (co_await client->execSqlCoro("SELECT * FROM foods WHERE food_id = ?;", food_id))[0];

        auto name = row[1].as<std::string>();
        auto quantity = row[2].as<float>() * amount;
        auto quantity_type = row[3].as<std::string>();
        auto protein = row[4].as<float>() * amount;
        auto calories = row[5].as<float>() * amount;

        auto query = R"(INSERT INTO food_entry_items 
            (entry_id, item_name, quantity, quantity_type, protein, calories)
            VALUES ($1, $2, $3, $4, $5, $6);)";

        auto result = co_await client->execSqlCoro(query, entry_id, name, quantity, quantity_type, protein, calories);

        // std::cout << "rows affected: " << result.affectedRows() << std::endl;

        std::string header = "{\"itemAdded\":{\"target\" : \"#entries-table\"}}";

        resp->setStatusCode(k201Created);
        resp->addHeader("HX-Trigger", header);
    } catch (std::invalid_argument &e) {
        resp->setStatusCode(k302Found);
        resp->setBody("<script>alert('Failed to add: invalid input')</script>");
    }

    co_return resp;
}

Task<HttpResponsePtr> Journals::delete_food_item(HttpRequestPtr req, unsigned long &&item_id) {
    std::cout << "delete_food_item(" << item_id << ")" << std::endl;
    HttpResponsePtr resp = HttpResponse::newHttpResponse();

    auto client = app().getDbClient();

    try {
        auto result = co_await client->execSqlCoro("DELETE FROM food_entry_items WHERE item_id = ?;", item_id);

        std::cout << "rows affected: " << result.affectedRows() << std::endl;

        std::string header = "{\"itemAdded\":{\"target\" : \"#entries-table\"}}"; //TODO: Use more appropriate name

        resp->setStatusCode(k204NoContent);
        resp->addHeader("HX-Trigger", header);
    } catch (std::invalid_argument &e) {
        resp->setStatusCode(k302Found);
        resp->setBody("<script>alert('Failed to add: invalid input')</script>");
    }

    co_return resp;
}