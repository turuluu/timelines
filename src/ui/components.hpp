
namespace tls
{
namespace ui
{
struct time_indicator : component
{
    time_indicator();

    std::string_view name() const override { return "time_indicator"; }
    void draw(const context& ctx) override;

    void refresh(const context& ctx) override { coord = ctx.mouse.x; }
    void layout(const context& ctx) override { bounds = ctx.bounds; }

    rect bounds;
    float coord = 0;
};
}
} // namespace tls