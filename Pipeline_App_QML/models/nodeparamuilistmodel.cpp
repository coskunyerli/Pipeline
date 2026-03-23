#include "nodeparamuilistmodel.h"

namespace Pipeline
{
    namespace Runtime
    {
        int NodeParamUIListModel::rowCount(const QModelIndex &parent) const
        {
            if (parent.isValid()) return 0;

            return static_cast<int>(m_items.size());
        }

        QVariant NodeParamUIListModel::data(const QModelIndex &index, int role) const
        {
            if (!index.isValid()) return {};

            const auto &i = m_items[index.row()];

            switch (role)
            {
                case TextRole:
                    return i.text;

                case UITypeRole:
                    return static_cast<int>(i.type);

                case ParamIndexRole:
                    return i.paramIndex;

                case FillWidthRole:
                    return i.fillWidth;

                case ColumnSpanRole:
                    return i.span;
            }

            return {};
        }

        bool NodeParamUIListModel::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            if(!m_parameterModel)
            {
                return false;
            }
            auto paramIndex = index.data(ParamIndexRole).toInt();
            return this->m_parameterModel->setData(this->m_parameterModel->index(paramIndex,0), value, ParameterRoles::ValueRole);
        }

        QHash<int, QByteArray> NodeParamUIListModel::roleNames() const
        {
            return
            {
                {TextRole, "text"},
                {UITypeRole, "uiType"},
                {ParamIndexRole, "paramIndex"},
                {FillWidthRole, "fillWidth"},
                {ColumnSpanRole, "columnSpan"},
            };
        }


        NodeParamListModel* NodeParamUIListModel::getParameterModel() const
        {
            return m_parameterModel;
        }

        void NodeParamUIListModel::setParameterModel(NodeParamListModel *newParameterModel)
        {
            if (m_parameterModel == newParameterModel)
            {
                return;
            }

            if (!newParameterModel) return;

            m_parameterModel = newParameterModel;
            beginResetModel();
            m_items.clear();
            m_maxColumns = 0;
            // 1️⃣ Önce maxColumns hesapla
            const int rowCount = newParameterModel->rowCount();

            for (int i = 0; i < rowCount; ++i)
            {
                int type = newParameterModel->data(newParameterModel->index(i, 0), ParameterRoles::TypeRole).toInt();
                int columnsNeeded = (type == static_cast<int>(ParamType::Browse)) ? 3 : 2;

                if (columnsNeeded > m_maxColumns) m_maxColumns = columnsNeeded;
            }

            // 2️⃣ Her parametreyi UIItem’e dönüştür
            for (int i = 0; i < rowCount; ++i)
            {
                QModelIndex idx = newParameterModel->index(i, 0);
                QString name = newParameterModel->data(idx, ParameterRoles::NameRole).toString();
                int type = newParameterModel->data(idx, ParameterRoles::TypeRole).toInt();
                QVariant value = newParameterModel->data(idx, ParameterRoles::ValueRole);
                bool isBrowse = (type == static_cast<int>(ParamType::Browse));
                int textColumnSpan = m_maxColumns - 1;       // Label + TextEdit

                if (isBrowse) textColumnSpan = m_maxColumns - 2; // Label + TextEdit + Button

                // Label → 1 kolon
                m_items.push_back({name, ParamUIType::Label, i, false, 1});
                // Input → kalan kolonları kaplasın
                m_items.push_back({value.toString(), ParamUIType::Input, i, true, textColumnSpan});

                // Button sadece Browse için → 1 kolon
                if (isBrowse)
                {
                    m_items.push_back({"...", ParamUIType::Button, i, false, 1});
                }
            }

            endResetModel();
            emit maxColumnsChanged();
            emit this->parameterModelChanged();
        }

        int NodeParamUIListModel::maxColumns() const
        {
            return m_maxColumns;
        }

    }
}
