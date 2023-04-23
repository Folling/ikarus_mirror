#include <iostream>
#include <thread>

#include <cppbase/logger.hpp>
#include <cppbase/option.hpp>
#include <cppbase/result.hpp>

#include <ikarus/entities/page.h>
#include <ikarus/entities/template.h>
#include <ikarus/project.h>

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    set_log_level(cppbase::LogLevel::Verbose);

    Project * project;

    std::string name = "/tmp/ikaproj_XXXXXX";

    auto path = mktemp(name.data());

    if (path == nullptr) {
        LOG_ERROR("unable to create temporary path");
        return 1;
    }

    LOG_INFO("opening project at {}", path);
    //
    //    if (IkarusProjectCreateResult rt = ikarus_project_create_v1(path, static_cast<IkarusProjectCreateV1Flags>(0));
    //        rt.status_code != StatusCode_Ok) {
    //        LOG_ERROR("error: {}", rt.status_code);
    //        return 1;
    //    } else {
    //        project = rt.project;
    //    }
    //
    //    auto start = std::chrono::high_resolution_clock::now();
    //
    //    Id template;
    //    if (IkarusTemplateCreateResult rt =
    //            ikarus_template_create_v1(project, id_null(), 0, "Test Template", IkarusTemplateCreateV1Flags_None);
    //        rt.status_code != StatusCode_Ok) {
    //        LOG_ERROR("error: {}", rt.status_code);
    //        return 1;
    //    } else {
    //        template = rt.template;
    //    }
    //
    //    LOG_FATAL("{}", (std::chrono::high_resolution_clock::now() - start) / 100'000);
    //
    //    if (IkarusProjectCloseResult rt = ikarus_project_close_v1(project, static_cast<IkarusProjectCloseV1Flags>(0));
    //        rt.status_code != StatusCode_Ok) {
    //        LOG_ERROR("error: {}", rt.status_code);
    //        return 1;
    //    }
}
