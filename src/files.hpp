#include <csv/csv.h>
#include <filesystem>

namespace file
{
using path = std::filesystem::path;

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
    return std::filesystem::path(CMAKE_ROOT);
}

bool
is_path_valid(const std::filesystem::path& path)
{
    return std::filesystem::exists(path) &&
           (std::filesystem::is_regular_file(path) || std::filesystem::is_symlink(path));
}
} // namespace util
