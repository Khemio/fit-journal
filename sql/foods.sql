DROP TABLE foods;

CREATE TABLE foods (
	food_id INTEGER PRIMARY KEY,
	food_name TEXT NOT NULL,
	food_quantity INTEGER NOT NULL,
	food_quantity_type TEXT NOT NULL,
	protein NUMERIC NOT NULL,
	calories NUMERIC NOT NULL
);

INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Milk 1.5%', 100, 'ml', 3, 44);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Turo Rudi', 1, 'pc', 3.3, 141 );
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Canned Corn', 100, 'g', 2.4, 80);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Canned Tuna', 100, 'g', 23, 263);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Egg L', 1, 'pc', 6.3, 75);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Apple', 1, 'pc', 0.4, 110);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Onions', 100, 'g', 0.8, 40);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Pickles', 100, 'g', 0.5, 28);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Honey', 1, 'ts', 0, 30);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Oil', 100,  'ml', 0, 884);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Sausage', 1, 'pc', 9, 165);
INSERT INTO foods (food_name, food_quantity, food_quantity_type, protein, calories)
    VALUES ('Milk Cofee', 1, 'cup', 0.9, 13.2);
