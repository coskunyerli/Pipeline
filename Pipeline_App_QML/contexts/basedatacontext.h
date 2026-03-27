#pragma once

#include <QObject>
namespace Pipeline::Runtime
{
    class NodeContextMetadata;
    class BaseDataContext : public QObject
    {
            Q_OBJECT
        public:
            explicit BaseDataContext(QObject *parent = nullptr);
    };
}

