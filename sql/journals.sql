CREATE TABLE IF NOT EXISTS journals (
	journal_id INTEGER PRIMARY KEY,
	owner_id INTEGER NOT NULL,
	journal_name TEXT NOT NULL UNIQUE,
    type TEXT NOT NULL,
    description TEXT,
    FOREIGN KEY (owner_id)
        REFERENCES users (user_id)
        ON DELETE CASCADE
);

-- Might need to add index for foreign key

INSERT INTO journals (owner_id, journal_name, type)
    VALUES (1, 'Feels', 'life');

INSERT INTO journals (owner_id, journal_name, type)
    VALUES (1, 'Eating', 'food');

INSERT INTO journals (owner_id, journal_name, type)
    VALUES (2, 'How is it going', 'life');

INSERT INTO journals (owner_id, journal_name, type)
    VALUES (2, 'Therapy', 'food');