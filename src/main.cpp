#include <csv/csv.h>
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

    std::string name;
    int start_year, end_year;
    while (in.read_row(name, start_year, end_year))
    {
        auto start = tls::clock::from_years(start_year);
        auto end = tls::clock::from_years(end_year);
        utlz::dbg("name: ",
                  name,
                  " s: ",
                  start.time_since_epoch().count(),
                  " e: ",
                  end.time_since_epoch().count());
        dst.add_entity(name, start, end);
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
        csv_path = util::get_project_path() / "examples/example_big.csv";
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
        auto start = clock::from_iso(-430);
        auto end = clock::from_iso(0);
        auto& renderer_h = ui.make<horizontal>();
        renderer_h.rendering_interval.start = start;
        renderer_h.rendering_interval.end = end;

        auto& renderer_v = ui.make<vertical>();
        renderer_v.rendering_interval.start = start;
        renderer_v.rendering_interval.end = end;

        ui.timer = std::make_unique<timer>();
        ui.set_refresh_rate(60);
        ui.set_current(renderer_h);

        // init scene
        ui.render();
    }

    app.make<event_handler>();
    app.loop();

    return 0;
}
