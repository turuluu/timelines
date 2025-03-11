// #include "test_runner.h"
#include "../deps/csv/csv.h"
#include "entities.hpp"
#include "rendering.hpp"
#include "sdl/events.hpp"

#include <filesystem>

int
main(int argc, char** argv)
{
    // path to csv
    auto basepath = std::filesystem::path(std::string(SDL_GetBasePath()));
    auto project_path = basepath.parent_path().parent_path();
    auto example_csv = project_path / "example.csv";

    // populate entities from csv
    io::CSVReader<3> in(example_csv.string());
    in.read_header(io::ignore_extra_column, "name", "start", "end");

    using namespace tls;

    core core;
    core.data.reserve(20);
    entity tmp("", 0, 0);
    while(in.read_row(tmp.name, tmp.interval.start, tmp.interval.end)){
        std::cout << "name: " << tmp.name << " s: " << tmp.interval.start << " e: " << tmp.interval.end << "\n";
        core.add_entity(tmp);
    }

    // TODO : auto& ui = app.create<RenderingController>();
    scoped_graphics sc_g(spec::screen_w, spec::screen_h);

    application app(core);
    auto& ui = app.make<rendering_controller>(core);
    {
        // TODO : auto& rh = ui.create<Horizontal>();
        auto& renderer_h = ui.make<Horizontal>();
        renderer_h.rendering_interval.start = -430;
        renderer_h.rendering_interval.end = 0;

        auto& renderer_v = ui.make<Vertical>();
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
