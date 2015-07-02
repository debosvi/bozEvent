CREATE TABLE `credentials` (
    `id`    INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
    `register`  INTEGER,
    `subscribe` INTEGER,
    `notify`    INTEGER,
    `priority`  INTEGER
);

CREATE TRIGGER update_sub_log UPDATE OF subscribe ON credentials 
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "subscribe", new.priority);
END;

CREATE TRIGGER update_reg_log UPDATE OF register ON credentials 
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "register", new.priority);
END;
