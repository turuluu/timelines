// #include "test_runner.h"
#include "details/csv/csv.h"
#include "details/events.hpp"
#include "rendering.hpp"
#include "time_abstractions.hpp"

#include <filesystem>

int
main(int argc, char** argv)
{
#if 0
    doctest::Context context;

    // !!! THIS IS JUST AN EXAMPLE SHOWING HOW DEFAULTS/OVERRIDES ARE SET !!!

    // defaults
    context.addFilter("test-case-exclude", "*math*"); // exclude test cases with "math" in their name
    context.setOption("abort-after", 5);              // stop test execution after 5 failed assertions
    context.setOption("order-by", "name");            // sort the test cases by their name

    context.applyCommandLine(argc, argv);

    // overrides
    context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail

    int res = context.run(); // run

    if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests

    int client_stuff_return_code = 0;
#endif
#ifndef TESTS_ONLY
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

    auto controller = new RenderingController();
    {
        auto r1 = controller->own(new Horizontal());
        r1->year_range.start = -430;
        r1->year_range.end = 0;
        r1->render_range(Global::instance().data, &r1->year_range);

        auto r0 = controller->own(new Vertical());
        r0->year_range.start = -430;
        r0->year_range.end = 0;
        r0->render_range(Global::instance().data, &r0->year_range);

        controller->set_current(r0);
    }

    auto app = Application::own(new SdlEventHandler(), controller);
    app.loop();

#endif

  // return res + client_stuff_return_code; // the result from doctest is propagated here as well

  // return EXIT_SUCCESS;
}
