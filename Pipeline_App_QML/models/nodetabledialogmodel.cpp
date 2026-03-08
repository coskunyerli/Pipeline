#include "nodetabledialogmodel.h"
namespace Pipeline::Runtime
{
    void NodeTableDialogModel::resetData()
    {
        this->setRoot(QSharedPointer<PythonNodeResult>(m_referenceModel->getRoot()->copy()));
    }

    void NodeTableDialogModel::saveData()
    {
        m_referenceModel->setRoot(QSharedPointer<PythonNodeResult>(this->getRoot()->copy()));
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
        this->setRoot(QSharedPointer<PythonNodeResult>(m_referenceModel->getRoot()->copy()));
        emit referenceModelChanged();
    }
}
