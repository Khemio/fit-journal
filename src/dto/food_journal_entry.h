#pragma once

struct Entry {
    unsigned long entry_id;
    unsigned long journal_id;
    std::string date;
    float total_protein;
    float total_calories;
    // std::vector<Item> items;
};

struct Item {
    unsigned long item_id;
    unsigned long entry_id;
    std::string name;
    float quantity;
    std::string quantity_type ;
    float protein;
    float calories;
};