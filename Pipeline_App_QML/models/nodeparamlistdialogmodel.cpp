#include "nodeparamlistdialogmodel.h"
namespace Pipeline::Runtime
{

    void NodeParamListDialogModel::resetData()
    {
        if (!m_referenceModel)
        {
            return;
        }
        std::vector<uint8_t> buffer;
        m_referenceModel->serialize(buffer);
        this->deserialize(buffer.data(), buffer.size());
    }

    void NodeParamListDialogModel::saveData()
    {
        if (!m_referenceModel)
        {
            return;
        }
        std::vector<uint8_t> buffer;
        this->serialize(buffer);
        this->m_referenceModel->deserialize(buffer.data(), buffer.size());
    }

    NodeParamListDialogModel::NodeParamListDialogModel(QObject *parent)
        : NodeParamListModel(parent)
    {
    }

    void NodeParamListDialogModel::setReferenceModel(NodeParamListModel *referenceModel)
    {
        if (m_referenceModel == referenceModel)
        {
            return;
        }

        m_referenceModel = referenceModel;
        this->resetData();
        emit referenceModelChanged();
    }

}
