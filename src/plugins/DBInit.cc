#include "DBInit.h"

using namespace drogon;

void init_db_settings() {
    /* 
        Application of settings to SQLite has to happen per connection. 
        If there is more then 1 connection, Drogon chooses connection randomly to execute queries.
        To apply settings to all connections:
        1. Block all connections with transactions.
        2. Make one query with one setting to each connection (for N connections make N identical queries).
        3. Release transactions.
        4. Repeat steps 1-3 for each setting.
    */

    auto client = app().getDbClient();
        if (client) {
            try {
                    client->execSqlSync("PRAGMA foreign_keys = ON;");
                    LOG_INFO << "Foreign keys are enabled now." ;
            } catch (const orm::DrogonDbException &e) {
                LOG_ERROR << "Error executing PRAGMA foreign_keys = ON: " << e.base().what();
            }
        } else {
            LOG_ERROR << "Could not get DBClient on DBInit plugin start";
        }

}

void init_tables() {
    auto client = app().getDbClient();

    client->execSqlSync(R"(
            CREATE TABLE IF NOT EXISTS foods (
                food_id INTEGER PRIMARY KEY,
                food_name TEXT NOT NULL,
                food_quantity INTEGER NOT NULL,
                food_quantity_type TEXT NOT NULL,
                protein NUMERIC NOT NULL,
                calories NUMERIC NOT NULL
            );
    )");

    client->execSqlSync(R"(
            CREATE TABLE IF NOT EXISTS users (
                user_id INTEGER PRIMARY KEY,
                username TEXT NOT NULL UNIQUE,
                password TEXT NOT NULL
            );
    )");

    client->execSqlSync(R"(
            CREATE TABLE IF NOT EXISTS journals (
                journal_id INTEGER PRIMARY KEY,
                owner_id INTEGER NOT NULL,
                is_active INTEGR NOT NULL DEFAULT 0,
                type TEXT NOT NULL,
                FOREIGN KEY (owner_id)
                    REFERENCES users (user_id)
                    ON DELETE CASCADE
            );
    )");

    client->execSqlSync(R"(
            CREATE TRIGGER IF NOT EXISTS add_journal_for_new_user 
                AFTER INSERT ON users
                BEGIN
                    INSERT INTO journals (owner_id, type, is_active)
                        VALUES (NEW.user_id, 'food', 1);
                    INSERT INTO journals (owner_id, type)
                        VALUES (NEW.user_id, 'life');
                    INSERT INTO journals (owner_id, type)
                        VALUES (NEW.user_id, 'sport');
                END;
    )");

    client->execSqlSync(R"(
            CREATE TABLE IF NOT EXISTS food_entries (
                entry_id INTEGER PRIMARY KEY,
                journal_id INTEGER NOT NULL,
                entry_date TEXT NOT NULL,
                total_protein NUMERIC NOT NULL,
                total_calories NUMERIC NOT NULL,
                FOREIGN KEY (journal_id)
                    REFERENCES journals
                    ON DELETE CASCADE
            );
    )");

    client->execSqlSync(R"(
            CREATE TABLE IF NOT EXISTS food_entry_items (
                item_id INTEGER PRIMARY KEY,
                entry_id INTEGER NOT NULL,
                item_name TEXT NOT NULL,
                quantity NUMERIC NOT NULL,
                quantity_type TEXT NOT NULL,
                protein NUMERIC NOT NULL,
                calories NUMERIC NOT NULL,
                FOREIGN KEY (entry_id)
                    REFERENCES food_entries
                    ON DELETE CASCADE
            );
    )");

    client->execSqlSync(R"(
            CREATE TRIGGER IF NOT EXISTS update_entry_totals 
                AFTER INSERT ON food_entry_items 
                BEGIN
                    UPDATE food_entries SET 
                    total_protein = NEW.protein + (
                            SELECT total_protein FROM food_entries WHERE entry_id = NEW.entry_id),
                    total_calories = NEW.calories + (
                            SELECT total_calories FROM food_entries WHERE entry_id = NEW.entry_id)
                    WHERE entry_id = NEW.entry_id;
                END;
    )");

    client->execSqlSync(R"(
            CREATE TRIGGER IF NOT EXISTS update_entry_totals_on_delete
                AFTER DELETE ON food_entry_items 
                BEGIN
                    UPDATE food_entries SET 
                    total_protein = (SELECT total_protein FROM food_entries 
                        WHERE entry_id = OLD.entry_id) - OLD.protein,
                    total_calories = (SELECT total_calories FROM food_entries 
                        WHERE entry_id = OLD.entry_id) - OLD.calories
                    WHERE entry_id = OLD.entry_id;
                END;
    )");
}

void init_foods() {
    auto client = app().getDbClient();

    client->execSqlSync("DELETE FROM foods;");

    client->execSqlSync(R"(
            INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
                VALUES 
                    ('Milk 1.5%', 100, 'ml', 3, 44),
                    ('Turo Rudi', 1, 'pc', 3.3, 141),
                    ('Canned Corn', 100, 'g', 2.4, 80),
                    ('Canned Tuna', 100, 'g', 23, 263),
                    ('Egg L', 1, 'pc', 6.3, 75),
                    ('Apple', 1, 'pc', 0.4, 110),
                    ('Onions', 100, 'g', 0.8, 40),
                    ('Pickles', 100, 'g', 0.5, 28),
                    ('Honey', 1, 'ts', 0, 30),
                    ('Oil', 100, 'ml', 0, 884),
                    ('Sausage', 1, 'pc', 9, 165),
                    ('Milk Cofee', 1, 'cup', 0.9, 13.2),
                    ('Peas', 100, 'g', 5, 74),
                    ('Bread', 100, 'g', 9.4, 271),
                    ('Bread', 1, 'pc', 2.3, 68),
                    ('Rice Pudding (Phistachio)', 100, 'g', 3.2, 108),
                    ('Pork (Fatty)', 100, 'g ', 23, 290),
                    ('Pork (Lean)', 100, 'g', 22, 127),
                    ('Protein Powder', 1, 'htbs', 10, 54);

    )");
}

void DBInit::initAndStart(const Json::Value &config)
{

    init_db_settings();
    init_tables();
    init_foods();
}

void DBInit::shutdown() 
{
    
}
