#pragma once

#include <exception>

namespace tls
{
namespace error
{
constexpr size_t max_filename_length = 128;

struct base : std::exception
{
    virtual void format_msg() const = 0;
    [[nodiscard]] const char* what() const noexcept override
    {
        format_msg();
        return msg_buffer;
    }

    mutable char msg_buffer[512];
};

struct with_code_file
{
    void set_code_file(const char* filename)
    {
        if (!filename)
            this->filename[0] = '\0';
        else
        {
            strncpy(this->filename, filename, max_filename_length);
            this->filename[max_filename_length - 1] = '\0';
        }
    }
    char filename[max_filename_length]{};
};

struct with_line_n
{
    void set_line_n(size_t line_n) { this->line_n = line_n; }

    size_t line_n{};
};
struct with_interval
{
    void set_interval(interval interval) { this->interval = interval; }
    interval interval{};
};

struct timescale_issue
  : base
  , with_code_file
  , with_line_n
  , with_interval
{
    void format_msg() const override
    {
        std::snprintf(msg_buffer,
                      sizeof(msg_buffer),
                      R"(%s:%zu\nEntity time scale issue with [start, end] := [%d, %d]\n)",
                      filename,
                      line_n,
                      interval.start,
                      interval.end);
    }
};
}; // namespace error

} // namespace tls