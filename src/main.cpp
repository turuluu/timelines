// #include "test_runner.h"
#include "rendering.hpp"
#include "../deps/csv/csv.h"
#include "sdl/events.hpp"
#include "entities.hpp"

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

    Core core;
    core.data.reserve(20);
    Entity tmp("", 0, 0);
    while(in.read_row(tmp.name, tmp.interval.start, tmp.interval.end)){
        std::cout << "name: " << tmp.name << " s: " << tmp.interval.start << " e: " << tmp.interval.end << "\n";
        core.add_entity(tmp);
    }

    // TODO : auto& ui = app.create<RenderingController>();
    ScopedGraphics sc_g(spec::screen_w, spec::screen_h);
    auto ui = new RenderingController(core);
    {
        // TODO : auto& rh = ui.create<Horizontal>();
        auto renderer_h = ui->own(new Horizontal());
        renderer_h->rendering_interval.start = -430;
        renderer_h->rendering_interval.end = 0;

        auto renderer_v = ui->own(new Vertical());
        renderer_v->rendering_interval.start = -430;
        renderer_v->rendering_interval.end = 0;

        ui->timer = std::make_unique<sdl::Timer>();
        ui->set_refresh_rate(60);
        ui->set_current(renderer_v);

        // init scene
        ui->renderer->render_range(core.data, &ui->renderer->rendering_interval);
    }

    Application app(core);
    // TODO : app.create<sdl::EventHandler>();
    app.own(new EventHandler());
    app.own(ui);
    app.loop();

    return 0;
}
