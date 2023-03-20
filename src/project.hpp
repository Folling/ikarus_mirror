#pragma once

#include <sqlite3.h>

#include <filesystem>

struct Project {
    std::filesystem::path path;
    sqlite3 * db;
};
