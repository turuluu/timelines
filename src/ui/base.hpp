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
};

// component "does"
struct component_ifc
{
    component() {}
    virtual ~component() = default;
    virtual bool click(const context& ctx) {};
    virtual bool cursor(const context& ctx) {};
    virtual bool hit_test(const context& ctx, point p) {};
    virtual void draw(const context& ctx) {};
    virtual void refresh(const context& ctx) {};
    virtual void layout(const context& ctx) {};
};

struct component : component_ifc
{
    sz id;
};

struct handle
{
    sz index = 0;
    sz version = 0;
    bool alive = false;
};
// SoA structure for caching and mapping
struct base
{
    sz add(component&& new_component, context&& ctx)
    {
        sz index = 0;
        if (freelist.empty())
        {
            components.emplace_back(std::move(new_component));
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
            components[index] = std::move(new_component);
            contexts[index] = std::move(ctx);
        }

        size = slots.size();
        return index;
    }
    bool remove(sz id)
    {
        if (id >= size || (!freelist.empty() &&
                           std::find(freelist.begin(), freelist.end(), id) == freelist.end()))
            return false;

        auto slot = slots[id];
        slot.alive = false;
        freelist.emplace_back(std::move(slot));

        return true;
    }

    sz size = 0;
    std::vector<handle> freelist;
    std::vector<handle> slots;
    std::vector<component> components;
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
} // namespace ui
} // namespace tls