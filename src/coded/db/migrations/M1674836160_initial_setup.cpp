#include "M1674836160_initial_setup.hpp"

#include "coded/db/database.hpp"

namespace db {

Result<void, int> M1674836160_initial_setup::up(db::Database& db) {
    LOG_VERBOSE("creating entities table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `entities`(\n"
        // INT is important here - INTEGER would make it an alias for the rowid
        "    `id` INT PRIMARY KEY,\n"
        "    `name` TEXT NOT NULL,\n"
        "    `information` TEXT NOT NULL\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating entities_fts index");

    TRY(db.exec(
        nullptr,
        "CREATE VIRTUAL TABLE `entities_fts` USING fts5(\n"
        "    `name`, `information`,\n"
        "    content='entities', content_rowid='id',\n"
        "    tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
        ");"
    ));

    LOG_VERBOSE("creating folders table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `folders`(\n"
        "    `entity_id` INT PRIMARY KEY,\n"
        "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating template_tree table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `template_tree`(\n"
        "    `template_id` INT PRIMARY KEY,\n"
        "    `parent_id` INT,\n"
        "    `position` INT NOT NULL,\n"
        "    FOREIGN KEY (`templates_id`) REFERENCES `templates`(`id`) ON DELETE CASCADE,\n"
        "    FOREIGN KEY (`parent_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating indices");

    LOG_VERBOSE("creating template_tree_parent index");

    TRY(db.exec(nullptr, "CREATE INDEX `template_tree_parent_idx` ON `template_tree`(`parent_id`);"));

    LOG_VERBOSE("creating template_tree_position index");

    TRY(db.exec(nullptr, "CREATE INDEX `template_tree_position_idx` ON `template_tree`(`position`);"));

    LOG_VERBOSE("creating template_tree_parent_position index");

    TRY(db.exec(nullptr, "CREATE UNIQUE INDEX `template_tree_parent_position_idx` ON `template_tree`(`parent_id`, `position`);"));

    LOG_VERBOSE("creating page_tree table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `page_tree`(\n"
        "    `page_id` INT PRIMARY KEY,\n"
        "    `parent_id` INT,\n"
        "    `position` INT NOT NULL,\n"
        "    FOREIGN KEY (`pages_id`) REFERENCES `pages`(`id`) ON DELETE CASCADE,\n"
        "    FOREIGN KEY (`parent_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating indices");

    LOG_VERBOSE("creating page_tree_parent index");

    TRY(db.exec(nullptr, "CREATE INDEX `page_tree_parent_idx` ON `page_tree`(`parent_id`);"));

    LOG_VERBOSE("creating page_tree_position index");

    TRY(db.exec(nullptr, "CREATE INDEX `page_tree_position_idx` ON `page_tree`(`position`);"));

    LOG_VERBOSE("creating page_tree_parent_position index");

    TRY(db.exec(nullptr, "CREATE UNIQUE INDEX `page_tree_parent_position_idx` ON `page_tree`(`parent_id`, `position`);"));

    LOG_VERBOSE("creating property_tree table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `property_tree`(\n"
        "    `property_id` INT PRIMARY KEY,\n"
        "    `template_id` INT NOT NULL,\n"
        "    `parent_id` INT,\n"
        "    `position` INT NOT NULL,\n"
        "    FOREIGN KEY (`template_id`) REFERENCES `templates`(`id`) ON DELETE CASCADE,\n"
        "    FOREIGN KEY (`properties_id`) REFERENCES `properties`(`id`) ON DELETE CASCADE,\n"
        "    FOREIGN KEY (`parent_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating indices");

    LOG_VERBOSE("creating property_tree_template index");

    TRY(db.exec(nullptr, "CREATE INDEX `property_tree_template_idx` ON `property_tree`(`template_id`);"));

    LOG_VERBOSE("creating property_tree_parent index");

    TRY(db.exec(nullptr, "CREATE INDEX `property_tree_parent_idx` ON `property_tree`(`parent_id`);"));

    LOG_VERBOSE("creating property_tree_position index");

    TRY(db.exec(nullptr, "CREATE INDEX `property_tree_position_idx` ON `property_tree`(`position`);"));

    LOG_VERBOSE("creating property_tree_parent_position index");

    TRY(db.exec(nullptr, "CREATE UNIQUE INDEX `property_tree_parent_position_idx` ON `property_tree`(`parent_id`, `position`);"));

    LOG_VERBOSE("creating templates table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `templates`(\n"
        "    `entity_id` INT PRIMARY KEY,\n"
        "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating properties table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `properties`(\n"
        "    `entity_id` INT PRIMARY KEY,\n"
        "    `template_id` INT NOT NULL,\n"
        "    `type` INT NOT NULL,\n"
        "    `settings` TEXT NOT NULL,\n"
        "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
        "    FOREIGN KEY (`template_id`) REFERENCES `templates`(`id`) ON DELETE CASCADE\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating property template index");

    TRY(db.exec(nullptr, "CREATE INDEX `property_template_id_idx` ON `properties`(`template_id`);"));

    LOG_VERBOSE("creating property property_type index");

    TRY(db.exec(nullptr, "CREATE INDEX `properties_type_idx` ON `properties`(`type`);"));

    LOG_VERBOSE("creating property FTS index");

    TRY(db.exec(
        nullptr,
        "CREATE VIRTUAL TABLE `properties_fts` USING fts5(\n"
        "    `settings`, content='properties', content_rowid='entity_id', tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
        ");"
    ));

    LOG_VERBOSE("creating pages table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `pages`(\n"
        "    `entity_id` INT PRIMARY KEY,\n"
        "    `template_id` INT NOT NULL,\n"
        "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
        "    FOREIGN KEY (`template_id`) REFERENCES `templates`(`id`) ON DELETE CASCADE\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating page template index");

    TRY(db.exec(nullptr, "CREATE INDEX `page_template_id_idx` ON `pages`(`template_id`);"));

    LOG_VERBOSE("creating values table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `values`(\n"
        "    `property_id` INT NOT NULL,\n"
        "    `page_id` INT NOT NULL,\n"
        "    `value` TEXT NOT NULL,\n"
        "    PRIMARY KEY (`property_id`, `page_id`)"
        "    FOREIGN KEY (`property_id`) REFERENCES `properties`(`id`) ON DELETE CASCADE,\n"
        "    FOREIGN KEY (`page_id`) REFERENCES `pages`(`id`) ON DELETE CASCADE"
        ") WITHOUT ROWID, STRICT;"
    ));

    LOG_VERBOSE("creating values FTS index");

    TRY(db.exec(
        nullptr,
        "CREATE VIRTUAL TABLE `values_fts` USING fts5(\n"
        "    `value`, content='values', tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
        ");"
    ));

    return ok();
}

u64 M1674836160_initial_setup::get_version() const {
    return 1674836160;
}

}
