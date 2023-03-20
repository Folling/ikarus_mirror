#pragma once

#include "db/db.hpp"
#include "db/migrations.hpp"

class M1674836160_initial_setup final : public Migration {
public:
    M1674836160_initial_setup() = default;

public:
    inline Result<void, int> up(sqlite3 * db) override {
        LOG_VERBOSE("creating entities table");

        TRY(db::exec(
            db,
            "CREATE TABLE `entities`(\n"
            // INT is important here - INTEGER would make it an alias for the rowid
            "    `id` INT PRIMARY KEY,\n"
            "    `name` TEXT NOT NULL,\n"
            "    `information` TEXT NOT NULL\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating entities FTS index");

        TRY(db::exec(
            db,
            "CREATE VIRTUAL TABLE `entities_fts` USING fts5(\n"
            "    `name`, `information`,\n"
            "    content='entities', content_rowid='id',\n"
            "    tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
            ");"
        ));

        LOG_VERBOSE("creating folders table");

        TRY(db::exec(
            db,
            "CREATE TABLE `folders`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE RESTRICT\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating tree table");

        TRY(db::exec(
            db,
            "CREATE TABLE `tree`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    `parent_id` INT,"
            "    `position` INT NOT NULL,"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE RESTRICT,\n"
            "    FOREIGN KEY (`parent_id`) REFERENCES `entities`(`id`) ON DELETE RESTRICT\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating tree_parent_position index");

        TRY(db::exec(db, "CREATE UNIQUE INDEX `tree_parent_position_idx` ON `tree`(`parent_id`, `position`);"));

        LOG_VERBOSE("creating blueprints table");

        TRY(db::exec(
            db,
            "CREATE TABLE `blueprints`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE RESTRICT\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating attributes table");

        TRY(db::exec(
            db,
            "CREATE TABLE `attributes`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    `type` INT NOT NULL,\n"
            "    `settings` TEXT NOT NULL,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE RESTRICT\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating attribute_type index");

        TRY(db::exec(db, "CREATE INDEX `attributes_type_idx` ON `attributes`(`type`);"));

        LOG_VERBOSE("creating attribute FTS index");

        TRY(db::exec(
            db,
            "CREATE VIRTUAL TABLE `attributes_fts` USING fts5(\n"
            "    `settings`, content='attributes', content_rowid='entity_id', tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
            ");"
        ));

        LOG_VERBOSE("creating instances table");

        TRY(db::exec(
            db,
            "CREATE TABLE `instances`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE RESTRICT\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating values table");

        TRY(db::exec(
            db,
            "CREATE TABLE `values`(\n"
            "    `attribute_id` INT NOT NULL,\n"
            "    `instance_id` INT NOT NULL,\n"
            "    `value` TEXT NOT NULL,\n"
            "    PRIMARY KEY (`attribute_id`, `instance_id`)"
            "    FOREIGN KEY (`attribute_id`) REFERENCES `attributes`(`id`) ON DELETE RESTRICT,\n"
            "    FOREIGN KEY (`instance_id`) REFERENCES `instances`(`id`) ON DELETE RESTRICT"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating values FTS index");

        TRY(db::exec(
            db,
            "CREATE VIRTUAL TABLE `values_fts` USING fts5(\n"
            "    `value`, content='values', tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
            ");"
        ));

        return ok();
    }

    [[nodiscard]] inline u64 get_version() const override {
        return 1674836160;
    }
};
