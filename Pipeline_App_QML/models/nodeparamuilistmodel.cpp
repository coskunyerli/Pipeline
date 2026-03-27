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
                case ValueRole:
                    {
                        if (!i.value.isNull())
                        {
                            return i.value;
                        }
                        else
                        {
                            auto paramModelIndex = this->m_parameterModel->index(i.paramIndex, 0);
                            return paramModelIndex.data(i.role);
                        }
                    }

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
            if (!m_parameterModel)
            {
                return false;
            }

            const auto &i = m_items[index.row()];
            auto paramIndex = index.data(ParamIndexRole).toInt();
            return this->m_parameterModel->setData(this->m_parameterModel->index(paramIndex, 0), value, i.role);
        }

        QHash<int, QByteArray> NodeParamUIListModel::roleNames() const
        {
            return
            {
                {ValueRole, "value"},
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
                return;

            if (m_parameterModel)
            {
                // Önce eski model sinyallerini disconnect et
                disconnect(m_parameterModel, nullptr, this, nullptr);
            }

            if (!newParameterModel) return;

            m_parameterModel = newParameterModel;
            beginResetModel();
            m_items.clear();
            m_maxColumns = 0;
            const int rowCount = newParameterModel->rowCount();

            // 1️⃣ maxColumns hesapla
            for (int i = 0; i < rowCount; ++i)
            {
                int type = newParameterModel->data(newParameterModel->index(i, 0), ParameterRoles::TypeRole).toInt();
                int columnsNeeded = (type == static_cast<int>(ParamType::Browse)) ? 3 : 2;

                if (columnsNeeded > m_maxColumns) m_maxColumns = columnsNeeded;
            }

            // 2️⃣ Her parametreyi UIItem’e dönüştür
            for (int i = 0; i < rowCount; ++i)
            {
                addRowToUI(i);
            }

            endResetModel();
            emit maxColumnsChanged();
            emit this->parameterModelChanged();
            // 3️⃣ Yeni eleman eklendiğinde veya silindiğinde yakala
            connect(m_parameterModel, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex & parent, int first, int last)
            {
                Q_UNUSED(parent)
                beginResetModel();

                for (int row = first; row <= last; ++row)
                    addRowToUI(row);

                endResetModel();
                emit maxColumnsChanged();
            });
            // connect(m_parameterModel, &QAbstractItemModel::rowsRemoved, this, [this](const QModelIndex & parent, int first, int last)
            // {
            // 	Q_UNUSED(parent)

            // 	// basit çözüm: tüm m_items’dan sil
            // 	for (int row = first; row <= last; ++row)
            // 	{
            // 		auto it = std::remove_if(m_items.begin(), m_items.end(), [row](const UIItem & item)
            // 		{
            // 			return item.sourceRow == row;
            // 		});
            // 		m_items.erase(it, m_items.end());
            // 	}

            // 	emit maxColumnsChanged();
            // 	// model değiştiği için view refresh gerekebilir
            // 	beginResetModel();
            // 	endResetModel();
            // });
        }

        int NodeParamUIListModel::maxColumns() const
        {
            return m_maxColumns;
        }


        void NodeParamUIListModel::addRowToUI(int row)
        {
            QModelIndex idx = m_parameterModel->index(row, 0);
            ParamType type = static_cast<ParamType>(m_parameterModel->data(idx, ParameterRoles::TypeRole).toInt());
            int textColumnSpan = m_maxColumns - 1;
            // Label
            m_items.push_back({{}, ParamUIType::Label, row, false, 1, ParameterRoles::NameRole});

            switch (type)
            {
                case ParamType::Browse:
                    textColumnSpan = m_maxColumns - 2;
                    break;

                default:
                    break;
            }

            switch (type)
            {
                case ParamType::String:
                case ParamType::Float:
                case ParamType::Int:
                    m_items.push_back({{}, ParamUIType::Input, row, true, textColumnSpan,  ParameterRoles::ValueRole});
                    break;

                case ParamType::Bool:
                    m_items.push_back({{}, ParamUIType::CheckBox, row, false, textColumnSpan, ParameterRoles::ValueRole});
                    break;

                case ParamType::Browse:
                    m_items.push_back({{}, ParamUIType::Input, row, true, textColumnSpan, ParameterRoles::ValueRole});
                    m_items.push_back({"...", ParamUIType::Button, row, false, 1, 0});
                    break;

                default:
                    break;
            }
        }

    }
}
