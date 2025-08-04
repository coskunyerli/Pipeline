#pragma once
#include <QObject>
#include <QSize>
#include <QPoint>

namespace Pipeline
{
    namespace UI
    {
        class NodeIUManager : public QObject
        {
                Q_OBJECT
                Q_PROPERTY(QSize nodeSize READ nodeSize WRITE setNodeSize NOTIFY nodeSizeChanged)
                Q_PROPERTY(QSize portSize READ portSize WRITE setPortSize NOTIFY portSizeChanged)
                Q_PROPERTY(int portSpacing READ portSpacing WRITE setPortSpacing NOTIFY portSpacingChanged)
                Q_PROPERTY(QPoint portStartPosition READ portStartPosition WRITE setPortStartPosition NOTIFY portStartPositionChanged)
            public:
                explicit NodeIUManager(QObject * parent = nullptr);
                QSize nodeSize() const
                {
                    return m_nodeSize;
                }

                QSize nodeSize(const QModelIndex&nodeIndex) const;
                QSize portSize() const
                {
                    return m_portSize;
                }

                int portSpacing() const
                {
                    return m_portSpacing;
                }

                QPoint portStartPosition() const
                {
                    return m_portStartPosition;
                }

                int bottomMargin() const
                {
                    return m_bottomMargin;
                }

                int topMargin() const
                {
                    return m_topMargin;
                }

                QPoint portPosition(const QModelIndex& portIndex);

                void setNodeSize(QSize size);
                void setPortSize(QSize size);
                void setPortSpacing(int spacing);
                void setPortStartPosition(QPoint position);
            signals:
                void nodeSizeChanged();
                void portSizeChanged();
                void portStartPositionChanged();
                void portSpacingChanged();
                void nodePositionChanged(QModelIndex);
            private:
                QSize m_nodeSize;
                QSize m_portSize;
                int m_portSpacing;
                QPoint m_portStartPosition;
                int m_leftMargin;
                int m_topMargin;
                int m_rightMargin;
                int m_bottomMargin;
        };
    }
}
