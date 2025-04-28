#include "../types.hpp"

#include <bitset>
#include <cassert>
namespace tls
{
namespace spec
{
constexpr sz max_components = 256;
}
namespace ui
{

// context "is"
struct context
{
    rect bounds;
    mouse_move mouse;
};

// component "does"
struct component
{
    virtual ~component() = default;
    virtual std::string_view name() const = 0;
    virtual bool click(const context& ctx) { return false; };
    virtual bool cursor(const context& ctx) { return false; };
    virtual bool hit_test(const context& ctx, point p) { return false; };
    virtual void draw(const context& ctx) {};
    virtual void refresh(const context& ctx) {};
    virtual void layout(const context& ctx) {};
};

// struct component : component_ifc
// {
//     sz id;
// };

struct handle
{
    sz index = 0;
    sz version = 0;
    bool alive = false;
};
// SoA structure for caching and mapping
struct base
{
    base()
    {
        components.reserve(spec::max_components);
        contexts.reserve(spec::max_components);
    }
    template <typename Component>
    sz add(Component new_component, context&& ctx)
    {
        sz index = 0;
        if (freelist.empty())
        {
            components.emplace_back(std::make_unique<Component>(std::move(new_component)));
            contexts.emplace_back(std::move(ctx));

            auto& slot = slots.emplace_back(std::move(handle{ slots.size(), 0, true }));
            index = slot.index;
        }
        else
        {
            auto slot = freelist.back();
            freelist.pop_back();

            slot.version += 1;
            slot.alive = true;
            index = slot.index;

            slots[index] = std::move(slot);
            components[index] = std::make_unique<Component>(std::move(new_component));
            contexts[index] = std::move(ctx);
        }

        size = slots.size();
        return index;
    }
    bool remove(sz id)
    {
        if (id >= size || (!freelist.empty() &&
                           std::find_if(freelist.begin(), freelist.end(), [&id](struct handle& handle){ return id == handle.index; }) == freelist.end()))
            return false;

        auto slot = slots[id];
        slot.alive = false;
        freelist.emplace_back(std::move(slot));

        return true;
    }

    sz size = 0;
    std::vector<handle> freelist;
    std::vector<handle> slots;
    std::vector<std::unique_ptr<component>> components;
    std::vector<context> contexts;
};
struct composite_base
  : component
  , storage<component>
{
    composite_base(base& base)
      : base(base)
    {
    }

    // TODO : tree of indices matching components and contexts
    // ...

    base& base;
};

struct time_indicator : component
{
    std::string_view name() const override { return "time_indicator"; }
    void draw(const context& ctx) override
    {
        point start{ coord, bounds.y + bounds.h };
        point end{ coord, 0 };
        draw_line(start, end, colors::broken_blue, 3);
    }
    void refresh(const context& ctx) override { coord = ctx.mouse.x; }
    void layout(const context& ctx) override { bounds = ctx.bounds; }

    rect bounds;
    float coord = 0;
};
} // namespace ui
} // namespace tls