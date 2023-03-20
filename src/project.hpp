#pragma once

#include "error.hpp"
#include "project.h"

#include <sqlite3.h>

#include <filesystem>
#include <unordered_map>

#include <entities/blueprint.hpp>
#include <entities/folder.hpp>

struct Project {
    std::filesystem::path path;
    sqlite3 * db;
};
