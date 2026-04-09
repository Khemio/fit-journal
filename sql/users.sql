CREATE TABLE IF NOT EXISTS users (
	user_id INTEGER PRIMARY KEY,
	username TEXT NOT NULL UNIQUE,
	password TEXT NOT NULL
);

INSERT INTO users (username, password) 
    VALUES ('alex', '$2b$10$PM2LiLe6QoEHrVgLneFrW.cFL8M3JDzd4OtjX6on41VlcZA10TF0e'); -- qwerty

INSERT INTO users (username, password) 
    VALUES ('john', '$2b$10$PM2LiLe6QoEHrVgLneFrW.0CyT0uoxYTJB7bfXrZXgptU8RPAvH16'); -- password123