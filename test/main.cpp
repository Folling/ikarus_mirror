#include <project.h>

#include <iostream>
#include <thread>

#include <util/logger.hpp>
#include <util/structs/option.hpp>
#include <util/structs/result.hpp>

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    set_log_level(LogLevel::Trace);

    for (int i = 0; i < 1000000; ++i) {
        Id id = id_generate(EntityType::EntityType_Attribute);
    }

    ErrorCode ec{};
    Project * proj;

    std::string name = "/tmp/ikaproj_XXXXXX";

    auto path = mktemp(name.data());

    if (path == nullptr) {
        LOG_ERROR("unable to create temporary path");
        return 1;
    }

    LOG_INFO("opening project at {}", path);

    if (proj = ikarus_project_create_v1(path, static_cast<IkarusProjectCreateV1Flags>(0), &ec), proj == nullptr) {
        LOG_ERROR("error: {}", ec);
        return 1;
    }

    if (!ikarus_project_close_v1(proj, static_cast<IkarusProjectCloseV1Flags>(0), &ec)) {
        LOG_ERROR("error: {}", ec);
        return 1;
    }
}