#include <iostream>
#include <thread>

#include <ikarus/entities/blueprint.h>
#include <ikarus/entities/instance.h>
#include <ikarus/project.h>
#include <util/logger.hpp>
#include <util/structs/option.hpp>
#include <util/structs/result.hpp>

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    set_log_level(LogLevel::Fatal);

    Project * project;

    std::string name = "/tmp/ikaproj_XXXXXX";

    auto path = mktemp(name.data());

    if (path == nullptr) {
        LOG_ERROR("unable to create temporary path");
        return 1;
    }

    LOG_INFO("opening project at {}", path);

    if (IkarusProjectCreateResult rt = ikarus_project_create_v1(path, static_cast<IkarusProjectCreateV1Flags>(0));
        rt.status_code != StatusCode_Ok) {
        LOG_ERROR("error: {}", rt.status_code);
        return 1;
    } else {
        project = rt.project;
    }

    Id blueprint;
    if (IkarusBlueprintCreateResult rt =
            ikarus_blueprint_create_v1(project, id_null(), 0, "Test Blueprint", IkarusBlueprintCreateV1Flags_None);
        rt.status_code != StatusCode_Ok) {
        LOG_ERROR("error: {}", rt.status_code);
        return 1;
    } else {
        blueprint = rt.blueprint;
    }

    if (IkarusProjectCloseResult rt = ikarus_project_close_v1(project, static_cast<IkarusProjectCloseV1Flags>(0));
        rt.status_code != StatusCode_Ok) {
        LOG_ERROR("error: {}", rt.status_code);
        return 1;
    }
}