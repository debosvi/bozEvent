CREATE TABLE `credentials` (
    `id`    INTEGER PRIMARY KEY UNIQUE,
    `register`  INTEGER,
    `subscribe` INTEGER,
    `notify`    INTEGER,
    `priority`  INTEGER
);

CREATE TABLE `cred_log` (
    `id`    INTEGER,
    `date`  TEXT,
    `operation` TEXT,
    `priority`  INTEGER
);

CREATE TRIGGER update_sub_log UPDATE OF subscribe ON credentials  WHEN new.subscribe!=0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "subscribe", new.priority);
END;

CREATE TRIGGER update_unsub_log UPDATE OF subscribe ON credentials  WHEN new.subscribe==0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "unsubscribe", new.priority);
END;

CREATE TRIGGER update_reg_log UPDATE OF register ON credentials WHEN new.register!=0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "register", new.priority);
END;

CREATE TRIGGER update_unreg_log UPDATE OF register ON credentials WHEN new.register==0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "unregister", new.priority);
END;
