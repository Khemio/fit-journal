#pragma once

struct Journal {
    unsigned long ID;
    unsigned long owner_id;
    bool is_active;
    std::string type;
};