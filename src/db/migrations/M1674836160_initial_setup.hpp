#pragma once

#include <db/db.hpp>
#include <db/migrations.hpp>
#include <ikarus/id.h>

class M1674836160_initial_setup final : public Migration {
public:
    M1674836160_initial_setup() = default;

public:
    inline Result<void, int> up(sqlite3 * db) override {
        LOG_VERBOSE("creating entities table");

        TRY(db::exec(
            db,
            nullptr,
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
            nullptr,
            "CREATE VIRTUAL TABLE `entities_fts` USING fts5(\n"
            "    `name`, `information`,\n"
            "    content='entities', content_rowid='id',\n"
            "    tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
            ");"
        ));

        LOG_VERBOSE("creating folders table");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE TABLE `folders`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating tree table");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE TABLE `entity_tree`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            // the scope is defined as belonging to a certain tree. For example, all blueprints & instances belong to the project scope
            // (where this will be null), but attributes are scoped to their blueprint. Note that scopes have no foreign key restraint, even
            // though they might exist in different tables
            "    `scope` INT,\n"
            "    `parent_id` INT,\n"
            "    `position` INT NOT NULL,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
            "    FOREIGN KEY (`parent_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating indices");

        LOG_VERBOSE("creating tree_parent index");

        TRY(db::exec(db, nullptr, "CREATE INDEX `tree_parent_idx` ON `entity_tree`(`parent_id`);"));

        LOG_VERBOSE("creating tree_position index");

        TRY(db::exec(db, nullptr, "CREATE INDEX `tree_position_idx` ON `entity_tree`(`position`);"));

        LOG_VERBOSE("creating tree_parent_position index");

        TRY(db::exec(db, nullptr, "CREATE UNIQUE INDEX `tree_parent_position_idx` ON `entity_tree`(`parent_id`, `position`);"));

        LOG_VERBOSE("creating tree_scope index");

        TRY(db::exec(db, nullptr, "CREATE INDEX `tree_scope_idx` ON `entity_tree`(`scope`)"));

        LOG_VERBOSE("creating blueprints table");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE TABLE `blueprints`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating attributes table");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE TABLE `attributes`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    `blueprint_id` INT NOT NULL,\n"
            "    `type` INT NOT NULL,\n"
            "    `settings` TEXT NOT NULL,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
            "    FOREIGN KEY (`blueprint_id`) REFERENCES `blueprints`(`id`) ON DELETE CASCADE\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating attribute blueprint index");

        TRY(db::exec(db, nullptr, "CREATE INDEX `attribute_blueprint_id_idx` ON `attributes`(`blueprint_id`);"));

        LOG_VERBOSE("creating attribute attribute_type index");

        TRY(db::exec(db, nullptr, "CREATE INDEX `attributes_type_idx` ON `attributes`(`type`);"));

        LOG_VERBOSE("creating attribute FTS index");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE VIRTUAL TABLE `attributes_fts` USING fts5(\n"
            "    `settings`, content='attributes', content_rowid='entity_id', tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
            ");"
        ));

        LOG_VERBOSE("creating instances table");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE TABLE `instances`(\n"
            "    `entity_id` INT PRIMARY KEY,\n"
            "    `blueprint_id` INT NOT NULL,\n"
            "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
            "    FOREIGN KEY (`blueprint_id`) REFERENCES `blueprints`(`id`) ON DELETE CASCADE\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating instance blueprint index");

        TRY(db::exec(db, nullptr, "CREATE INDEX `instance_blueprint_id_idx` ON `instances`(`blueprint_id`);"));

        LOG_VERBOSE("creating values table");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE TABLE `values`(\n"
            "    `attribute_id` INT NOT NULL,\n"
            "    `instance_id` INT NOT NULL,\n"
            "    `value` TEXT NOT NULL,\n"
            "    PRIMARY KEY (`attribute_id`, `instance_id`)"
            "    FOREIGN KEY (`attribute_id`) REFERENCES `attributes`(`id`) ON DELETE CASCADE,\n"
            "    FOREIGN KEY (`instance_id`) REFERENCES `instances`(`id`) ON DELETE CASCADE"
            ") WITHOUT ROWID, STRICT;"
        ));

        LOG_VERBOSE("creating values FTS index");

        TRY(db::exec(
            db,
            nullptr,
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
