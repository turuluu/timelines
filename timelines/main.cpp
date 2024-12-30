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

    io::CSVReader<3> in(example_csv.string());

    // populate entities from csv
    in.read_header(io::ignore_extra_column, "name", "start", "end");
    std::string name;
    int startYear = 0;
    int endYear = 0;
    while(in.read_row(name, startYear, endYear)){
        std::cout << "name: " << name << " s: " << startYear << " e: " << endYear << "\n";
        auto* e = new Entity(name);
        *e | startYear | endYear;
    }

    auto ui = new RenderingController();
    {
        auto renderer_h = ui->own(new Horizontal());
        renderer_h->year_range.start = -430;
        renderer_h->year_range.end = 0;
        renderer_h->render_range(Global::instance().data, &renderer_h->year_range);

        auto renderer_v = ui->own(new Vertical());
        renderer_v->year_range.start = -430;
        renderer_v->year_range.end = 0;
        renderer_v->render_range(Global::instance().data, &renderer_v->year_range);

        ui->timer = std::make_unique<sdl::Timer>();
        ui->set_refresh_rate(60);
        ui->set_current(renderer_v);
    }

    auto app = Application::own(new sdl::EventHandler(), ui);
    app.loop();

    return 0;
}
