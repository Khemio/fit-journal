# Fit Journal

This is a health and fitness journaling web application. It supports multiple users and allows tracking of food intake.

## Build Info

This application relies on Drogon framework that dinamically links some of it's dependencies. For this project I used MSYS2 UCRT, downloaded dependecies and built Drogon from source. Not sure how to set it up in other environments. For now I created a Dockerfile and an [image](https://hub.docker.com/r/khemio/fitness-journal). Image is unoptimized and takes up around 1.5 GB.

## Thecnical Info

This is a server driven application, made with C++ framework Drogon. Front End is server rendered with Drogon CSP (C++ Server Pages) and styled with Pico CSS. Interactivity and server communication achieved with HTMX. Database of choice is SQLite.

## Dependencies

- Drogon Framework and it's dependencies
- Bcrypt.cpp (vendored, changed some names to avoid collision with Windows "bcrypt.h")
- HTMX (vendored)
- Pico CSS (vendored)

## DB Schema

``` mermaid
erDiagram
    direction LR
    USERS ||--|{ JOURNALS : own

    JOURNALS ||--o{ FOOD_ENTRIES : contain

    FOOD_ENTRIES ||--o{ FOOD_ENTRY_ITEMS : includes

    USERS {

        int user_id PK

        string name UK

        string password

    }

    JOURNALS {

        int journal_id PK

        int owner_id FK

        bool is_active

        string type
    }

    FOOD_ENTRIES {

        int enry_id PK

        int journal_id FK

        date entry_date

        float total_protein

        float total_calories

    }

    FOOD_ENTRY_ITEMS {
    
        int item_id PK

        int entry_id FK

        string item_name

        float quantity

        string quantity_type

        float protein

        float calories

    }

    FOODS {
    
        int food_id PK

        string food_name

        int food_quantity

        string food_quantity_type

        float protein

        float calories
    }
```
