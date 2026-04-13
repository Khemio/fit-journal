#include "JournalCtrl.h"

#include "../dto/journal.h"

Task<HttpResponsePtr> Journals::get_all(HttpRequestPtr req) {
    // HttpResponsePtr resp;
    HttpViewData data;
    std::vector<Journal> jrs;

    auto user_id = req->session()->getOptional<unsigned long>("ID").value_or(0);
    auto client = app().getDbClient();
    auto result = co_await client->execSqlCoro("SELECT * FROM journals WHERE owner_id = ?;", user_id);

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
