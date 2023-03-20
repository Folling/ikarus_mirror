#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct Folder;
struct Entity;

Folder * folder_create(Folder * parent, size_t idx, char const * name, ErrorCode * err_out);
void folder_delete(Folder * folder, bool keep_children, ErrorCode * err_out);

size_t get_children_count(Folder const * folder, ErrorCode * err_out);
void get_children(
    Folder const * folder,
    bool recurse,
    Entity const ** children_out,
    size_t children_out_size,
    size_t * total_size_out,
    ErrorCode * err_out
);
void get_children_mut(
    Folder const * folder, bool recurse, Entity ** children_out, size_t children_out_size, size_t * total_size_out, ErrorCode * err_out
);
size_t get_children_count(Folder const * folder, bool recurse, ErrorCode * err_out);

void clear(Folder * folder, ErrorCode * err_out);
