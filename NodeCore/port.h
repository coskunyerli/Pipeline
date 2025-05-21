#pragma once

namespace Pipeline
{
    namespace Core
    {
        class Connection;
        class Port
        {
            public:
                explicit Port();
                virtual Connection* connect(Port *other);
                bool hasConnection() const;
                Connection* getConnection() const
                {
                    return m_connection;
                }

            private:
                Connection* m_connection;
        };
    }
}
