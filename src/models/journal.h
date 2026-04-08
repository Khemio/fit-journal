#pragma once

struct Journal {
    unsigned long ID;
    unsigned long owner_id;
    std::string name;
    std::string type ;
    std::string description ;
};