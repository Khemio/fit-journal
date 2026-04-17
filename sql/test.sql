--? Might need to add index for foreign key

INSERT INTO users (username, password) 
    VALUES ('alex', '$2b$10$PM2LiLe6QoEHrVgLneFrW.cFL8M3JDzd4OtjX6on41VlcZA10TF0e'); -- qwerty

INSERT INTO users (username, password) 
    VALUES ('john', '$2b$10$PM2LiLe6QoEHrVgLneFrW.0CyT0uoxYTJB7bfXrZXgptU8RPAvH16'); -- password123

-- INSERT INTO journals (owner_id, journal_name, type)
--     VALUES (1, 'Feels', 'life');

-- INSERT INTO journals (owner_id, journal_name, type)
--     VALUES (1, 'Eating', 'food');

-- INSERT INTO journals (owner_id, journal_name, type)
--     VALUES (2, 'How is it going', 'life');

-- INSERT INTO journals (owner_id, journal_name, type)
--     VALUES (2, 'Therapy', 'food');

INSERT INTO food_entries (journal_id, entry_date, total_protein, total_calories)
    VALUES (2, '2026-04-09', 179, 2448);

INSERT INTO food_entries (journal_id, entry_date, total_protein, total_calories)
    VALUES (2, '2026-04-10', 218, 2739);


INSERT INTO food_entry_items (entry_id, item_name, quantity, quantity_type, protein, calories)
    VALUES (1, 'Canned Tuna', 130, 'g', 29.9, 234);

INSERT INTO food_entry_items (entry_id, item_name, quantity, quantity_type, protein, calories)
    VALUES (1, 'Canned Corn', 285, 'g', 6.84, 228);

INSERT INTO food_entry_items (entry_id, item_name, quantity, quantity_type, protein, calories)
    VALUES (1, 'Peas', 150, 'g', 7.5, 111);

INSERT INTO food_entry_items (entry_id, item_name, quantity, quantity_type, protein, calories)
    VALUES (1, 'Pickles', 50, 'g', 0.25, 14);