CREATE TABLE IF NOT EXISTS users (
	user_id INTEGER PRIMARY KEY,
	username TEXT NOT NULL UNIQUE,
	password TEXT NOT NULL
);

INSERT INTO users (username, password) 
    VALUES ('khemio', '123wa456tfc');

INSERT INTO users (username, password) 
    VALUES ('john', 'qwerty');