#pragma once
#include <vector>
#include <QVariant>

namespace Pipeline
{
    namespace UI
    {
        class TreeItem
        {
            public:
                TreeItem();
                explicit TreeItem(TreeItem* parent);
                virtual ~TreeItem();
                size_t getChildCount() const;
                TreeItem* getChild(size_t index);
                TreeItem* getParent() const;
                void addChild(TreeItem* child);
                void removeChild(TreeItem* child);
                TreeItem* removeChild(size_t index);
                void insertChild(size_t index, TreeItem* child);
                size_t indexOf(TreeItem* child, bool& has);
                void setData(const QVariant& data);

                template <typename T>
                const T getData() const
                {
                    return m_data.value<T>();
                }

                template <typename T>
                T getData()
                {
                    return m_data.value<T>();
                }
                template<typename T>
                bool canConvert() const
                {
                    return m_data.canConvert<T>();
                }
            private:
                TreeItem* m_parent = nullptr;
                std::vector<TreeItem*> m_children;
                QVariant m_data;
        };

    }
}
