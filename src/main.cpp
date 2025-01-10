// #include "test_runner.h"
#include "details/csv/csv.h"
#include "details/events.hpp"
#include "rendering.hpp"
#include "time_abstractions.hpp"

#include <filesystem>

int
main(int argc, char** argv)
{
    using namespace tls;
    ScopedGraphics sc_g(spec::screen_w, spec::screen_h);

    // path to csv
    auto basepath = std::filesystem::path(std::string(SDL_GetBasePath()));
    auto project_path = basepath.parent_path().parent_path();
    auto example_csv = project_path / "example.csv";

    // populate entities from csv
    io::CSVReader<3> in(example_csv.string());
    in.read_header(io::ignore_extra_column, "name", "start", "end");

    Core core;
    core.data.reserve(20);
    Entity tmp("", 0, 0);
    while(in.read_row(tmp.name, tmp.start_year, tmp.end_year)){
        std::cout << "name: " << tmp.name << " s: " << tmp.start_year << " e: " << tmp.end_year << "\n";
        core.add(tmp);
    }

    // TODO : auto& ui = app.create<RenderingController>();
    auto ui = new RenderingController(core);
    {
        // TODO : auto& rh = ui.create<Horizontal>();
        auto renderer_h = ui->own(new Horizontal());
        renderer_h->year_range.start = -430;
        renderer_h->year_range.end = 0;

        auto renderer_v = ui->own(new Vertical());
        renderer_v->year_range.start = -430;
        renderer_v->year_range.end = 0;

        ui->timer = std::make_unique<sdl::Timer>();
        ui->set_refresh_rate(60);
        ui->set_current(renderer_v);

        // init scene
        ui->renderer->render_range(core.data, &ui->renderer->year_range);
    }

    Application app(core);
    // TODO : app.create<sdl::EventHandler>();
    app.own(new sdl::EventHandler());
    app.own(ui);
    app.loop();

    return 0;
}
