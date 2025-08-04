#include "treeitem.h"

namespace Pipeline
{
    namespace UI
    {

        TreeItem::TreeItem()
            : m_parent(nullptr) {}

        TreeItem::TreeItem(TreeItem *parent)
            : m_parent(parent) {}

        TreeItem::~TreeItem()
        {
        }

        size_t TreeItem::getChildCount() const
        {
            return m_children.size();
        }

        TreeItem* TreeItem::getChild(size_t index)
        {
            if (index >= m_children.size()) return nullptr;

            return m_children[index];
        }

        TreeItem* TreeItem::getParent() const
        {
            return m_parent;
        }

        void TreeItem::addChild(TreeItem *child)
        {
            if (child)
            {
                child->m_parent = this;
                m_children.push_back(child);
            }
        }

        void TreeItem::removeChild(TreeItem *child)
        {
            if (!child) return;

            auto it = std::find(m_children.begin(), m_children.end(), child);

            if (it != m_children.end())
            {
                m_children.erase(it);
            }
        }

        TreeItem* TreeItem::removeChild(size_t index)
        {
            if (index >= m_children.size()) return nullptr;

            TreeItem* child = m_children[index];
            m_children.erase(m_children.begin() + index);
            return child;
        }

        void TreeItem::insertChild(size_t index, TreeItem *child)
        {
            if (!child) return;

            if (index > m_children.size()) index = m_children.size();

            child->m_parent = this;
            m_children.insert(m_children.begin() + index, child);
        }

        size_t TreeItem::indexOf(TreeItem *child, bool &has)
        {
            auto it = std::find(m_children.begin(), m_children.end(), child);

            if (it != m_children.end())
            {
                has = true;
                return static_cast<size_t>(std::distance(m_children.begin(), it));
            }

            has = false;
            return static_cast<size_t>(-1);
        }
        void TreeItem::setData(const QVariant &data)
        {
            m_data = data;
        }

    }
}
