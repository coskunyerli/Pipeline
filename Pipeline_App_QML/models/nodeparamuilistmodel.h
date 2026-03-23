#pragma once
#include <QAbstractListModel>
#include <vector>
#include <QString>
#include <models/nodeparamlistmodel.h>
#include <constants.h>
namespace Pipeline::Runtime
{
    enum class ParamUIType
    {
        Label,
        Input,
        Button
    };

    struct UIItem
    {
        QString text;      // Label veya initial value
        ParamUIType type;  // Label, Input, Button
        int paramIndex;    // hangi NodeParameter
        bool fillWidth;    // TextEdit fillWidth
        int span;
    };

    class NodeParamUIListModel : public QAbstractListModel
    {
            Q_OBJECT
            Q_PROPERTY(int maxColumns READ maxColumns NOTIFY maxColumnsChanged)
            Q_PROPERTY(NodeParamListModel* parameterModel READ getParameterModel WRITE setParameterModel NOTIFY parameterModelChanged FINAL)
        public:
            explicit NodeParamUIListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

            enum Roles
            {
                TextRole = Qt::UserRole + 1,
                UITypeRole,
                ParamIndexRole,
                FillWidthRole,
                ColumnSpanRole
            };

            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            QVariant data(const QModelIndex &index, int role) const override;
            bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
            QHash<int, QByteArray> roleNames() const override;
            NodeParamListModel* getParameterModel() const;
            void setParameterModel(NodeParamListModel *newParameterModel);
            int maxColumns() const;

        signals:
            void parameterModelChanged();
            void maxColumnsChanged();

        private:
            std::vector<UIItem> m_items;
            NodeParamListModel* m_parameterModel = nullptr;
            int m_maxColumns;
    };
}
