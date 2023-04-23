#include "M1674836160_initial_setup.hpp"

#include <cppbase/logger.hpp>

#include <sqlitecpp/database.hpp>

#include <sys/shm.h>

namespace db {

cppbase::Result<void, int> M1674836160_initial_setup::up(sqlitecpp::Database * db) {
    LOG_VERBOSE("creating entities table");

    TRY(
        db->exec("CREATE TABLE `entities`(\n"
                 // INT is important here - INTEGER would make it an alias for the rowid
                 "    `id` INT PRIMARY KEY,\n"
                 "    `name` TEXT NOT NULL,\n"
                 "    `information` TEXT NOT NULL\n"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating entities_fts index");

    TRY(
        db->exec("CREATE VIRTUAL TABLE `entities_fts` USING fts5(\n"
                 "    `name`, `information`,\n"
                 "    content='entities', content_rowid='id',\n"
                 "    tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
                 ");")
    );

    LOG_VERBOSE("creating folders table");

    TRY(
        db->exec("CREATE TABLE `folders`(\n"
                 "    `entity_id` INT PRIMARY KEY,\n"
                 "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating entity_tree table");

    TRY(
        db->exec("CREATE TABLE `entity_tree`(\n"
                 "    `entity_id` INT PRIMARY KEY,\n"
                 "    `parent_id` INT,\n"
                 "    `position` INT NOT NULL,\n"
                 "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
                 "    FOREIGN KEY (`parent_id`) REFERENCES `folders`(`id`) ON DELETE CASCADE\n"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating entity_tree_parent index");

    TRY(db->exec("CREATE INDEX `entity_tree_parent_idx` ON `entity_tree`(`parent_id`);"));

    LOG_VERBOSE("creating entity_tree_position index");

    TRY(db->exec("CREATE INDEX `entity_tree_position_idx` ON `entity_tree`(`position`);"));

    LOG_VERBOSE("creating entity_tree_parent_position index");

    TRY(db->exec("CREATE UNIQUE INDEX `entity_tree_parent_position_idx` ON `entity_tree`(`parent_id`, `position`);"));

    // IMPLEMENTATION_DETAIL_TREE_LAYOUT
    // this table is required since some root folders are scoped to a certain entity
    // for example, each template has its own property tree, so does each page
    // so if we wanted to fetch that specific subtree, we would need to know which root-folder is associated
    // with that blueprint. This is what this table is for

    LOG_VERBOSE("creating root_folder_entity_mapping table");

    TRY(
        db->exec("CREATE TABLE `root_folder_entity_mapping`(\n"
                 "    `entity_id` INT PRIMARY KEY,\n"
                 "    `folder_id` INT NOT NULL UNIQUE,\n"
                 "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
                 "    FOREIGN KEY (`folder_id`) REFERENCES `folders`(`id`) ON DELETE CASCADE\n"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating templates table");

    TRY(
        db->exec("CREATE TABLE `templates`(\n"
                 "    `entity_id` INT PRIMARY KEY,\n"
                 "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE\n"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating properties table");

    TRY(
        db->exec("CREATE TABLE `properties`(\n"
                 "    `entity_id` INT PRIMARY KEY,\n"
                 "    `template_id` INT NOT NULL,\n"
                 "    `type` INT NOT NULL,\n"
                 "    `settings` TEXT NOT NULL,\n"
                 "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
                 "    FOREIGN KEY (`template_id`) REFERENCES `templates`(`id`) ON DELETE CASCADE\n"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating property template index");

    TRY(db->exec("CREATE INDEX `property_template_id_idx` ON `properties`(`template_id`);"));

    LOG_VERBOSE("creating property property_type index");

    TRY(db->exec("CREATE INDEX `properties_type_idx` ON `properties`(`type`);"));

    LOG_VERBOSE("creating property settings FTS index");

    TRY(db->exec(
        "CREATE VIRTUAL TABLE `property_settings_fts` USING fts5(\n"
        "    `settings`, content='properties', content_rowid='entity_id', tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
        ");"
    ));

    LOG_VERBOSE("creating pages table");

    TRY(
        db->exec("CREATE TABLE `pages`(\n"
                 "    `entity_id` INT PRIMARY KEY,\n"
                 "    `template_id` INT NOT NULL,\n"
                 "    FOREIGN KEY (`entity_id`) REFERENCES `entities`(`id`) ON DELETE CASCADE,\n"
                 "    FOREIGN KEY (`template_id`) REFERENCES `templates`(`id`) ON DELETE CASCADE\n"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating page template index");

    TRY(db->exec("CREATE INDEX `page_template_id_idx` ON `pages`(`template_id`);"));

    LOG_VERBOSE("creating values table");

    TRY(
        db->exec("CREATE TABLE `values`(\n"
                 "    `property_id` INT NOT NULL,\n"
                 "    `page_id` INT NOT NULL,\n"
                 "    `value` TEXT NOT NULL,\n"
                 "    PRIMARY KEY (`property_id`, `page_id`)"
                 "    FOREIGN KEY (`property_id`) REFERENCES `properties`(`id`) ON DELETE CASCADE,\n"
                 "    FOREIGN KEY (`page_id`) REFERENCES `pages`(`id`) ON DELETE CASCADE"
                 ") WITHOUT ROWID, STRICT;")
    );

    LOG_VERBOSE("creating values FTS index");

    TRY(
        db->exec("CREATE VIRTUAL TABLE `values_fts` USING fts5(\n"
                 "    `value`, content='values', tokenize=\"unicode61 remove_diacritics 2 tokenchars '-_'\"\n"
                 ");")
    );

    return cppbase::ok();
}

cppbase::u64 M1674836160_initial_setup::get_version() const {
    return 1'674'836'160;
}

}
