#pragma once
#include <QAbstractItemModel>
#include <models/nodetablemodel.h>
namespace Pipeline::Runtime
{
    class HierarchicalTableData;
    class NodeTableDialogModel: public NodeTableModel
    {
            Q_OBJECT
            Q_PROPERTY(NodeTableModel* referenceModel READ referenceModel WRITE setReferenceModel NOTIFY referenceModelChanged)
        public:
            Q_INVOKABLE void resetData();
            Q_INVOKABLE void saveData();
        public:
            explicit NodeTableDialogModel(QObject*parent = nullptr);
            void setReferenceModel(NodeTableModel*referenceModel);

            NodeTableModel* referenceModel() const
            {
                return m_referenceModel;
            }
        signals:
            void referenceModelChanged();
        private:
            NodeTableModel* m_referenceModel;
    };
}
