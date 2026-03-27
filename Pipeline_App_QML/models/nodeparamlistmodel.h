#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVariant>
#include <QUrl>
#include <vector>
#include <constants.h>
#include <data/nodeparameter.h>
#include <data/nodeparameterlist.h>
namespace Pipeline::Runtime
{
    class NodeParamListModel : public QAbstractListModel
    {
            Q_OBJECT
            Q_PROPERTY(bool editable READ getEditable WRITE setEditable NOTIFY editableChanged FINAL)
        public:
            explicit NodeParamListModel(QObject* parent = nullptr);

            // --------------------
            // Basic Overrides
            // --------------------
            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
            QVariant data(const QString&name, int role = Qt::DisplayRole) const;
            bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
            bool setData(const QString&name, const QVariant& value, int role = Qt::EditRole);
            Qt::ItemFlags flags(const QModelIndex& index) const override;
            QHash<int, QByteArray> roleNames() const override;
            // --------------------
            // API
            // --------------------
            Q_INVOKABLE void addParameter(const QString& name, int type, const QVariant& value = {});
            Q_INVOKABLE void removeParameter(int index);
            Q_INVOKABLE void removeParameter(const QString& name);

            bool getEditable() const;
            void setEditable(bool newEditable);

            void serialize(std::vector<uint8_t> &buffer) const;
            void deserialize(const uint8_t* data, size_t size);

        signals:
            void editableChanged();
        private:

            inline PVariant fromQVariant(const QVariant& v) const
            {
                switch (v.typeId())
                {
                case QMetaType::Int:
                    return PVariant(v.toInt());

                case QMetaType::Double:
                    return PVariant(v.toDouble());

                case QMetaType::Float:
                    return PVariant(v.toFloat());

                case QMetaType::Bool:
                    return PVariant(v.toBool());

                case QMetaType::QString:
                    return PVariant(v.toString().toStdString());

                case QMetaType::QUrl:
                    return PVariant(v.toUrl().toLocalFile().toStdString());

                default:
                    throw std::runtime_error("Unsupported QVariant type");
                }
            }

            inline QVariant toQVariant(const PVariant& v) const
            {
                return std::visit([](auto&& val) -> QVariant {

                    using T = std::decay_t<decltype(val)>;

                    if constexpr (std::is_same_v<T, int>)
                        return QVariant(val);

                    else if constexpr (std::is_same_v<T, double>)
                        return QVariant(val);

                    else if constexpr (std::is_same_v<T, float>)
                        return QVariant(val);

                    else if constexpr (std::is_same_v<T, bool>)
                        return QVariant(val);

                    else if constexpr (std::is_same_v<T, std::string>)
                        return QVariant(QString::fromStdString(val));
                    else if constexpr (std::is_same_v<T, std::vector<std::string>>)
                    {
                        QStringList list;
                        list.reserve(val.size());
                        for (const auto& s : val)
                            list << QString::fromStdString(s);

                        return list; // QVariant implicit oluşur
                    }

                }, v.data());
            }

        private:
            NodeParameterList m_nodeParamList;
            bool m_editable;
    };
}

