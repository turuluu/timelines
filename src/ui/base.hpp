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
    virtual bool click(const context& ctx){};
    virtual bool cursor(const context& ctx){};
    virtual bool hit_test(const context& ctx, point p){};
    virtual void draw(const context& ctx){};
    virtual void refresh(const context& ctx){};
    virtual void layout(const context& ctx){};
};

struct component : component_ifc
{
    sz id;
};

// SoA structure for caching and mapping
struct base
{
    sz add(component&& new_component)
    {
        sz index = 0;
        auto it = std::find(component_table.begin(), component_table.end(), false);
        if (it != component_table.end())
        {
            index = std::distance(component_table.begin(), it);
            component_table.set(index);
        }
        else
        {
            assert(("component table full", false));
        }

        if (index == component_table.size())
        {
            components.emplace_back(std::move(new_component));
            contexts.emplace_back({});
        }
        else
        {
            components[index] = std::move(new_component);
            contexts[index] = {};
        }
        ++size;
        return index;
    }
    bool remove(sz id)
    {
        if (component_table[id])
        {
            component_table.set(id, false);
            --size;
        }
    }

    sz size = 0;
    std::bitset<spec::max_components> component_table;
    std::vector<component> components;
    std::vector<context> contexts;
};
struct composite_base : component, storage<component>
{
    composite_base(base& base) : base(base) {}

    // TODO : tree of indices matching components and contexts
    // ...

    base& base;
};
}
}