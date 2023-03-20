#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct Instance;

struct Attribute;
struct Blueprint;
struct Folder;
struct Value;
struct InstanceValue;

Instance * instance_create(Folder * parent, size_t idx, Blueprint * blueprint, char const * name, ErrorCode * err_out);

void instance_delete(Instance * instance, ErrorCode * err_out);

void instance_copy(Instance * attribute, Folder * parent, uint64_t idx, ErrorCode * err_out);

Blueprint * instance_get_blueprint(Instance const * instance, ErrorCode * err_out);
