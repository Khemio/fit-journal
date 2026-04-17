--? Might need to add index for foreign key

CREATE TABLE IF NOT EXISTS foods (
	food_id INTEGER PRIMARY KEY,
	food_name TEXT NOT NULL,
	food_quantity INTEGER NOT NULL,
	food_quantity_type TEXT NOT NULL,
	protein NUMERIC NOT NULL,
	calories NUMERIC NOT NULL
);

CREATE TABLE IF NOT EXISTS users (
	user_id INTEGER PRIMARY KEY,
	username TEXT NOT NULL UNIQUE,
	password TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS journals (
	journal_id INTEGER PRIMARY KEY,
	owner_id INTEGER NOT NULL,
    is_active INTEGR NOT NULL DEFAULT 0,
    type TEXT NOT NULL,
    FOREIGN KEY (owner_id)
        REFERENCES users (user_id)
        ON DELETE CASCADE
);

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