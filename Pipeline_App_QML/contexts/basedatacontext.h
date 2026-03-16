#pragma once

#include <QObject>
namespace Pipeline::Runtime
{
    class BaseDataContext : public QObject
    {
            Q_OBJECT
        public:
            explicit BaseDataContext(QObject *parent = nullptr);
    };
}

