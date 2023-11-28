#pragma once

#include <concepts>
namespace std
{
    template <typename NodeType>
    concept intrusive_list_node_type = requires(const NodeType const_t, NodeType var_t) {
        {
            const_t.get_next()
        } -> std::same_as<NodeType*>;
        {
            const_t.get_prev()
        } -> std::same_as<NodeType*>;
        var_t.set_next(&var_t);
        var_t.set_prev(&var_t);
    };

    template <intrusive_list_node_type NodeType>
    class intrusive_list
    {
        NodeType* front;
        inline auto reset_node(NodeType* node)
        {
            node->set_next(nullptr);
            node->set_prev(nullptr);
        }

    public:
        auto add_front(NodeType* node) -> NodeType*
        {
            reset_node(node);
            auto old_front = front;
            front = node;

            if (old_front)
            {
                old_front->set_prev(front);
                front->set_next(old_front);
            }

            return node;
        }

        auto add_after(NodeType* target, NodeType* node) -> NodeType*
        {
            reset_node(node);

            // |target| -> |node|
            // |      | <- |    |

            auto next = target->get_next();
            target->set_next(node);
            node->set_next(next);
            if (next)
            {
                next->set_prev(node);
            }

            return node;
        }

        auto remove(NodeType* node) -> NodeType*
        {
            // [prev] <-> node <-> [next]
            auto prev = node->get_prev();
            auto next = node->get_next();

            if (prev)
            {
                prev->get_next(next);
            }
            else
            {
                front = next;
            }

            if (prev)
            {
                next->set_prev(prev);
            }
            return node;
        }

        auto get_front() const -> NodeType* { return front; }
        auto pop_front() -> NodeType*
        {
            if (!front)
            {
                return front;
            }

            auto ret = front;
            front = front->get_next();
            if (front)
            {
                front->set_prev(nullptr);
            }

            return ret;
        }
    };
} // namespace std
