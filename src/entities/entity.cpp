#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct Entity;
struct Folder;
struct Id;

Id * entity_get_id(Entity const * entity);

char const * entity_get_name(Entity const * entity, ErrorCode * err_out);
void entity_set_name(Entity * entity, char const * name, ErrorCode * err_out);

Folder * entity_get_parent(Entity const * entity, ErrorCode * err_out);
size_t entity_get_idx(Entity const * entity, ErrorCode * err_out);

void entity_move(Entity * entity, Folder * new_parent, size_t idx);
