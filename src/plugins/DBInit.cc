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
                    ('Turo Rudi', 1, 'pc', 3.3, 141 ),
                    ('Canned Corn', 100, 'g', 2.4, 80),
                    ('Canned Corn', 1, 'can', 6.84, 228),
                    ('Canned Tuna', 100, 'g', 23, 180),
                    ('Canned Tuna', 1, 'can', 29.9, 234),
                    ('Egg L', 1, 'pc', 6.3, 75),
                    ('Apple', 1, 'pc', 0.4, 110),
                    ('Onions', 100, 'g', 0.8, 40),
                    ('Pickles', 100, 'g', 0.5, 28),
                    ('Honey', 1, 'tsp', 0, 30),
                    ('Honey', 1, 'tbsp', 0, 70),
                    ('Oil', 100, 'ml', 0, 884),
                    ('Sausage', 1, 'pc', 9, 165),
                    ('Milk Cofee', 1, 'cup', 0.9, 13.2),
                    ('Peas', 100, 'g', 5, 74),
                    ('Bread', 100, 'g', 9.4, 271),
                    ('Bread', 1, 'pc', 2.3, 68),
                    ('Rice Pudding (Phistachio)', 100, 'g', 3.2, 108),
                    ('Pork (Fatty)', 100, 'g ', 23, 290),
                    ('Pork (Lean)', 100, 'g', 22, 127),
                    ('Protein Powder', 1, 'htbs', 10, 54),
                    ('Veggie Mix', 100, 'g', 1.5, 27),
                    ('Turkey (Lean Minced)', 100, 'g', 22, 160),
                    ('Oats', 100, 'g', 13.5, 372),
                    ('Rice (Cooked)', 100, 'g', 2.6, 130),
                    ('Kefir', 100, 'ml', 3, 59),
                    ('Sauces', 100, 'g', 3, 150),
                    ('Asian Soup', 1, 'pack', 4.8, 150),
                    ('Broccoli Mix', 100, 'g', 1.8, 34),
                    ('Hashbrowns (Small)', 100, 'g', 2, 190),
                    ('Hashbrowns (Small)', 1, 'pc', 0.7, 66.5),
                    ('Canned Beans (Tomato)', 1, 'can', 16, 314),
                    ('Canned Beans (Tomato)', 100, 'g', 3.8, 75),
                    ('Canned Beans', 1, 'can', 17.5, 218),
                    ('White Wine (Dry)', 100, 'ml', 0, 75),
                    ('Fish Sticks', 100, 'g', 13, 182),
                    ('Fish Sticks', 1, 'pc', 4.9, 54.6),
                    ('Hashbrowns', 100, 'g', 2.4, 202),
                    ('Hashbrowns', 1, 'pc', 1.2, 101),
                    ('Frozen Spinach', 100, 'g', 3.1, 26),
                    ('Chocolate (Ginger & Lime)', 1, 'pc', 1.1, 128),
                    ('Chocolate (Ginger & Lime)', 100, 'g', 4.5, 513),
                    ('Sausages (Thin)', 100, 'g', 11, 300),
                    ('Sausages (Thin)', 1, 'pc', 7.5, 204),
                    ('Frozen Potato (Rolls)', 100, 'g', 2.5, 171),
                    ('Brownie', 100, 'g', 5.6, 444),
                    ('Canned Popcorn', 1, 'can', 2.8, 343),
                    ('Popcorn', 100, 'g', 8, 433),
                    ('Chips (Fake Pringles)', 100, 'g', 4.3, 500),
                    ('Tofu', 100, 'g', 17, 158),
                    ('Lasagna', 100, 'g', 7.3, 139),
                    ('Frozen Potatos (Wavy)', 100, 'g', 2.5, 174),
                    ('Frozen Potatos (Wedges)', 100, 'g', 2, 136),
                    ('Frozen Potatos (Loops)', 100, 'g', 2.5, 179),
                    ('Greek Yogurt', 100, 'g', 2.7, 117),
                    ('Greek Yogurt (Thick)', 100, 'g', 3.3, 103),
                    ('Natural Yogurt', 100, 'g', 3.2, 53),
                    ('Ice cream cone', 100, 'g', 2.4, 234),
                    ('Ice cream cone', 1, 'pc', 1.8, 175),
                    ('Apricot Biscuits', 1, 'pc', 1.6, 103),
                    ('Lemon Biscuits', 1, 'pc', 1, 95),
                    ('Frozen Pizza (Chicken Thin)', 100, 'g', 10, 240),
                    ('Frozen Pizza (Chicken Thin)', 1, 'pizza', 36, 852),
                    ('Frozen Pizza (Chorizo Thin)', 100, 'g', 9.8, 202),
                    ('Frozen Pizza (Chorizo Thin)', 1, 'pizza', 31, 640),
                    ('Muffin', 100 , 'g', 6, 425),
                    ('Ice Cream (Twister)', 1, 'pc', 0, 101),
                    ('Fanta (Cherry)', 100, 'ml', 0, 17),
                    ('Sweet Milk (Donut)', 100, 'ml', 3.3, 67),
                    ('Ice Cream (Coockies & Caramel)', 100, 'g', 3.6, 172),
                    ('Ice Cream (Coockies & Caramel)', 1, 'prt', 1.8, 86),
                    ('Wrap', 100, 'g', 11.1, 200),
                    ('Wrap', 1, 'pc', 21, 380),
                    ('Sandwitch (Carrot)', 100, 'g', 9.5, 227),
                    ('Sandwitch (Carrot)', 1, 'pc', 22.8, 545),
                    ('Sandwitch (Pulled Pork)', 100, 'g', 11, 266),
                    ('Sandwitch (Pulled Pork)', 1, 'pc', 22, 532),
                    ('Sandwitch (Spicy)', 100, 'g', 11.4, 258),
                    ('Sandwitch (Spicy)', 1, 'pc', 27.3, 619),
                    ('Bacon & Cheese Pie', 1, 'pc', 10, 500),
                    ('Strudel (Cherry)', 1, 'pc', 10, 1000),
                    ('Brussel Sprouts', 100, 'g', 3.2, 44),
                    ('Burrito', 1, 'pc', 80, 2000),
                    ('Onion (M)', 1, 'pc', 1.2, 50),
                    ('Kolbasa Salad', 100, 'g', 6, 200),
                    ('Cream Yogurt (Chocolate Chip)', 1, 'pc', 3.4, 157),
                    ('Spinach Rice', 100, 'g', 2.8, 100),
                    ('Thin Chips (Lemon & Chili)', 100, 'g', 7, 510),
                    ('Sweet Potato Chips', 100, 'g', 5.8, 480),
                    ('Chicken Wings (Honey)', 100, 'g', 18, 110),
                    ('Chicken Wings (Herbs)', 100, 'g', 18, 119),
                    ('Chicken Wings (Paprika)', 100, 'g', 18, 125),
                    ('Frozen Chicken Wings (BBQ)', 100, 'g', 20, 192),
                    ('Red Wine (Dry)', 100, 'ml', 0, 85),
                    ('Fish Pizza', 1, 'pizza', 34, 763),
                    ('Fish Pizza', 100, 'g', 9.8, 218),
                    ('Oval Pizza', 1, 'pizza', 37, 964),
                    ('Oval Pizza', 100, 'g', 9.7, 254),
                    ('Pita Pizza', 1, 'pizza', 11, 519),
                    ('Pita Pizza', 100, 'g', 5.8, 273),
                    ('Sour Cream 12%', 100, 'g', 3, 135),
                    ('Banana (Small)', 1, 'pc', 1.3, 110),
                    ('Ravioli (Bacon)', 100, 'g', 9.6, 317),
                    ('Frozen Fish (Filet)', 100, 'g', 14, 151),
                    ('Korean Spicy Cakes', 1, 'serv', 5, 310),
                    ('Sandwich (Pickles)', 100, 'g', 11.7, 228),
                    ('Sandwich (Pickles)', 1, 'pc', 30.4, 593),
                    ('Kit Kat (White)', 1, 'pc', 2.5, 209),
                    ('Frozen Fish (With bones)', 100, 'g', 15.7, 78),
                    ('Chicken Thighs', 100, 'g', 18, 127),
                    ('Pasta', 100, 'g', 12, 358),
                    ('Mushroom soup', 1, 'serv', 2.5, 108),
                    ('Chanterelle Mushroom soup', 1, 'serv', 2.5, 125),
                    ('Thick Sausages (Garlic)', 100, 'g', 11.8, 422),
                    ('Thick Sausages (Paprika)', 100, 'g', 11.5, 394),
                    ('Pana Cotta (Cherry)', 1, 'pc', 5.3, 143),
                    ('Peach pie', 1, 'pc', 10, 600),
                    ('Gin (37.5%)', 100, 'ml', 0, 300),
                    ('Tomato Paste', 100, 'g', 1.5, 25),
                    ('Bolognase Mix', 100, 'g', 5.5, 323),
                    ('Minced Pork (Fatty)', 100, 'g', 20, 270),
                    ('Toffifee', 100, 'g', 6, 521),
                    ('Toffifee', 1, 'pc', 0.5, 44),
                    ('Protein Bread', 100, 'g', 10, 220),
                    ('Fresh Pizza', 1, 'pizza', 40, 2000),
                    ('Bolonese sauce', 100, 'g', 13, 200),
                    ('Cherry Beer (Pecsi)', 1, 'can', 0 , 220),
                    ('Quadrupel Beer', 1, 'bottle', 0 , 300),
                    ('Cointreau', 100, 'ml', 0 , 310),
                    ('Tomato', 100, 'g', 1, 20),
                    ('Chicken Cuts', 100, 'g', 16 , 85);
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
