#pragma once
#include <models/nodeparamlistmodel.h>
namespace Pipeline::Runtime
{
    class NodeParamListDialogModel: public NodeParamListModel
    {
            Q_OBJECT
            Q_PROPERTY(NodeParamListModel* referenceModel READ referenceModel WRITE setReferenceModel NOTIFY referenceModelChanged)
        public:
            Q_INVOKABLE void resetData();
            Q_INVOKABLE void saveData();
        public:
            explicit NodeParamListDialogModel(QObject*parent = nullptr);
            void setReferenceModel(NodeParamListModel*referenceModel);

            NodeParamListModel* referenceModel() const
            {
                return m_referenceModel;
            }
        signals:
            void referenceModelChanged();
        private:
            NodeParamListModel* m_referenceModel;
    };
}

