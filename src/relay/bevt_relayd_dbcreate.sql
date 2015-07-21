CREATE TABLE `credentials` (
    `id`    INTEGER PRIMARY KEY UNIQUE,
    `register`  INTEGER,
    `subscribe` INTEGER,
    `notify`    INTEGER,
    `reg_prio`  INTEGER,
    `sub_prio`  INTEGER
);

CREATE TABLE `cred_log` (
    `id`    INTEGER,
    `date`  TEXT,
    `operation` TEXT,
    `priority`  INTEGER
);

CREATE TRIGGER update_sub_log UPDATE OF subscribe ON credentials  WHEN new.subscribe!=0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "subscribe", new.sub_prio);
END;

CREATE TRIGGER update2_sub_log UPDATE OF sub_prio ON credentials WHEN old.subscribe!=0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "subscribe update", new.sub_prio);
END;

CREATE TRIGGER update_unsub_log UPDATE OF subscribe ON credentials  WHEN new.subscribe==0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "unsubscribe", new.sub_prio);
END;

CREATE TRIGGER update_reg_log UPDATE OF register ON credentials WHEN new.register!=0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "register", new.reg_prio);
END;

CREATE TRIGGER update2_reg_log UPDATE OF reg_prio ON credentials WHEN old.register!=0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "register update", new.reg_prio);
END;

CREATE TRIGGER update_unreg_log UPDATE OF register ON credentials WHEN new.register==0
BEGIN
    INSERT INTO cred_log VALUES (old.id, datetime(strftime('%s','now'), 'unixepoch'), "unregister", new.reg_prio);
END;
