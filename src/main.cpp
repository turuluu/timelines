#include "../deps/csv/csv.h"
#include "app.hpp"
#include "entities.hpp"
#include "rendering.hpp"
#include "sdl/events.hpp"
#include "sdl/graphics.hpp"

#include <filesystem>

namespace util
{
void
populate_entities_from_csv(tls::core& dst, const std::filesystem::path& path)
{
    io::CSVReader<3> in(path.string());
    in.read_header(io::ignore_extra_column, "name", "start", "end");

    tls::entity tmp("", 0, 0);
    while (in.read_row(tmp.name, tmp.interval.start, tmp.interval.end))
    {
        utlz::dbg("name: ", tmp.name, " s: ", tmp.interval.start, " e: ", tmp.interval.end);
        dst.add_entity(tmp);
    }
}

auto
get_project_path()
{
    auto basepath = std::filesystem::path(std::string(SDL_GetBasePath()));
    auto project_path = basepath.parent_path().parent_path();

    return project_path;
}

bool
is_path_valid(const std::filesystem::path& path)
{
    return std::filesystem::exists(path) &&
           (std::filesystem::is_regular_file(path) || std::filesystem::is_symlink(path));
}
} // namespace util

int
main(int argc, char** argv)
{
    using namespace tls;

    std::filesystem::path csv_path;
    if (argc < 2)
        csv_path = util::get_project_path() / "example.csv";
    else
        csv_path = argv[1];
    if (!util::is_path_valid(csv_path))
    {
        std::cout << csv_path << " is not a valid file\n";
        exit(1);
    }

    core core;
    core.data.reserve(20);
    util::populate_entities_from_csv(core, csv_path);

    scoped_graphics sc_g(spec::screen_w, spec::screen_h);

    application app(core);
    auto& ui = app.make<rendering_controller>(core);
    {
        auto& renderer_h = ui.make<horizontal>();
        renderer_h.rendering_interval.start = -430;
        renderer_h.rendering_interval.end = 0;

        auto& renderer_v = ui.make<vertical>();
        renderer_v.rendering_interval.start = -430;
        renderer_v.rendering_interval.end = 0;

        ui.timer = std::make_unique<timer>();
        ui.set_refresh_rate(60);
        ui.set_current(renderer_v);

        // init scene
        ui.render();
    }

    app.make<event_handler>();
    app.loop();

    return 0;
}
