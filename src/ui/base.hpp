#pragma once

#include "../types.hpp"

#include <bitset>
#include <cassert>
namespace tls
{
struct gui;

namespace spec
{
constexpr sz max_components = 256;
}
namespace ui
{
struct base;

// context "is"
struct context
{
    rect bounds;
    mouse_move mouse;
    gui* gui{ nullptr };
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
    template<typename Component, typename... Args>
    sz add(context&& ctx, Args&&... args)
    {
        sz index = 0;
        if (freelist.empty())
        {
            components.emplace_back(std::make_unique<Component>( std::forward<Args>(args)...));
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
            components[index] = std::make_unique<Component>(std::forward<Args>(args)...);
            contexts[index] = std::move(ctx);
        }

        size = slots.size();
        return index;
    }
    bool remove(sz id)
    {
        if (id >= size ||
            (!freelist.empty() && std::find_if(freelist.begin(),
                                               freelist.end(),
                                               [&id](struct handle& handle)
                                               { return id == handle.index; }) == freelist.end()))
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
    composite_base(struct base& base)
      : base(base)
    {
    }

    // TODO : tree of indices matching components and contexts
    // ...

    struct base& base;
};

} // namespace ui
} // namespace tls