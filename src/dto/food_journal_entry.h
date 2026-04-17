#pragma once
#include <drogon/drogon.h>

using namespace drogon::orm;

struct Entry {
    unsigned long entry_id;
    unsigned long journal_id;
    std::string date;
    float total_protein;
    float total_calories;

    Entry(Row row) {
        entry_id = row[0].as<unsigned long>();
        journal_id = row[1].as<unsigned long>();
        date = row[2].as<std::string>();
        total_protein = row[3].as<float>();
        total_calories = row[4].as<float>();
    }
};

struct Item {
    unsigned long item_id;
    unsigned long entry_id;
    std::string name;
    float quantity;
    std::string quantity_type ;
    float protein;
    float calories;

    Item(Row row) {
        item_id = row[0].as<unsigned long>();
        entry_id = row[1].as<unsigned long>();
        name = row[2].as<std::string>();
        quantity = row[3].as<float>();
        quantity_type = row[4].as<std::string>();
        protein = row[5].as<float>();
        calories = row[6].as<float>();
    }
};