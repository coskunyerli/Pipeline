#include "nodetabledialogmodel.h"
namespace Pipeline::Runtime
{
    void NodeTableDialogModel::resetData()
    {
        this->setRoot(QSharedPointer<HierarchicalTableData>(m_referenceModel->getRoot()->copy()));
    }

    void NodeTableDialogModel::saveData()
    {
        m_referenceModel->setRoot(QSharedPointer<HierarchicalTableData>(this->getRoot()->copy()));
    }

    NodeTableDialogModel::NodeTableDialogModel(QObject *parent)
        : NodeTableModel(parent)
    {
    }

    void NodeTableDialogModel::setReferenceModel(NodeTableModel *referenceModel)
    {
        if (m_referenceModel == referenceModel)
        {
            return;
        }

        m_referenceModel = referenceModel;
        this->setRoot(QSharedPointer<HierarchicalTableData>(m_referenceModel->getRoot()->copy()));
        emit referenceModelChanged();
    }
}
