#pragma once
#include <drogon/drogon.h>

using namespace drogon::orm;

struct Food {
    unsigned long ID;
    std::string name;
    float quantity;
    std::string quantity_type ;
    float protein;
    float calories;

    Food() {
        ID = 0;
        name = "";
        quantity = 0;
        quantity_type = "";
        protein = 0;
        calories = 0;
    }

    Food(Row row) {
        ID = row[0].as<unsigned long>();
        name = row[1].as<std::string>();
        quantity = row[2].as<float>();
        quantity_type = row[3].as<std::string>();
        protein = row[4].as<float>();
        calories = row[5].as<float>();
    }
    
    Food(Row row, float amount) {
        ID = row[0].as<unsigned long>();
        name = row[1].as<std::string>();
        quantity = row[2].as<float>() * amount;
        quantity_type = row[3].as<std::string>();
        protein = row[4].as<float>() * amount;
        calories = row[5].as<float>() * amount;
    }
};