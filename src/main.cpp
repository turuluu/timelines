#include "app.hpp"
#include "entities.hpp"
#include "files.hpp"
#include "rendering.hpp"
#include "sdl/events.hpp"
#include "sdl/graphics.hpp"

int
main(int argc, char** argv)
{
    using namespace tls;

    file::path csv_path;
    if (argc < 2)
        csv_path = file::get_project_path() / "examples/example.csv";
    else
        csv_path = argv[1];
    if (!file::is_path_valid(csv_path))
    {
        std::cout << csv_path << " is not a valid file\n";
        exit(1);
    }

    core core;
    core.data.reserve(20);
    file::populate_entities_from_csv(core, csv_path);

    scoped_graphics sc_g(spec::screen_w, spec::screen_h);

    application app(core);
    auto& ui = app.make<gui>(core);
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
