#pragma once
#include <variant>
#include <vector>
#include <string>
#include <stdexcept>

namespace Pipeline::Runtime
{

    enum class ParamType
    {
        Int,
        Float,
        Bool,
        String,
        List,
        Table,
        Browse
    };


    class PVariant
    {
        public:
            using Value = std::variant <
                          int,
                          double,
                          float,
                          bool,
                          std::string,
                          std::vector<std::string>
                          >;

            PVariant() = default;

            //template<typename T>
            PVariant(int val) : value(val) {}
            PVariant(double val) : value(val) {}
            PVariant(float val) : value(val) {}
            PVariant(const std::string& val) : value(val) {}
            PVariant(bool val) : value(val) {}
            PVariant(Value val) : value(val) {}

            template<typename T>
            void set(T val)
            {
                value = val;
            }

            template<typename T>
            T get() const
            {
                return std::get<T>(value);
            }

            template<typename T>
            bool is() const
            {
                return std::holds_alternative<T>(value);
            }

            // QVariant tarzı API
            int toInt() const
            {
                return convert<int>();
            }
            float toFloat() const
            {
                return convert<float>();
            }
            double toDouble() const
            {
                return convert<double>();
            }
            bool toBool() const
            {
                return convert<bool>();
            }
            std::string toString() const
            {
                return convert<std::string>();
            }
            std::vector<std::string> toStringList() const
            {
                return convert<std::vector<std::string >> ();
            }
            const Value& data() const
            {
                return value;
            }
        private:
            Value value;

            template<typename T>
            T convert() const
            {
                return std::visit([](auto&& val) -> T
                {

                    using From = std::decay_t<decltype(val)>;

                    // 🔹 same type
                    if constexpr(std::is_same_v<T, From>)
                    {
                        return val;
                    }

                    // 🔹 vector<string> target
                    else if constexpr(std::is_same_v<T, std::vector<std::string>>)
                    {
                        if constexpr(std::is_same_v<From, std::string>)
                        {
                            return { val }; // tek elemanlı liste
                        }
                        else
                        {
                            return { std::to_string(val) }; // tek elemanlı liste
                        }
                    }

                    // 🔹 string target
                    else if constexpr(std::is_same_v<T, std::string>)
                    {
                        if constexpr(std::is_same_v<From, bool>)
                            return val ? "true" : "false";
                        else if constexpr(std::is_same_v<From, std::vector<std::string>>)
                        {
                            // basit join
                            std::string result;
                            for (size_t i = 0; i < val.size(); ++i)
                            {
                                result += val[i];
                                if (i + 1 < val.size())
                                    result += ",";
                            }
                            return result;
                        }
                        else
                        {
                            return std::to_string(val);
                        }
                    }

                    // 🔹 bool target
                    else if constexpr(std::is_same_v<T, bool>)
                    {
                        if constexpr(std::is_same_v<From, std::string>)
                        {
                            if (val == "true" || val == "1") return true;

                            if (val == "false" || val == "0") return false;

                            throw std::invalid_argument("string->bool");
                        }
                        else if constexpr(std::is_same_v<From, std::vector<std::string>>)
                        {
                            return false;
                        }
                        else
                        {
                            return static_cast<bool>(val);
                        }
                    }

                    // 🔹 numeric target
                    else if constexpr(
                        std::is_same_v<T, int> ||
                        std::is_same_v<T, float> ||
                        std::is_same_v<T, double>)
                    {
                        if constexpr(std::is_same_v<From, std::string>)
                        {
                            if constexpr(std::is_same_v<T, int>) return std::stoi(val);

                            if constexpr(std::is_same_v<T, float>) return std::stof(val);

                            if constexpr(std::is_same_v<T, double>) return std::stod(val);
                        }
                        else if constexpr(std::is_same_v<From, std::vector<std::string>>)
                        {
                            return 0;
                        }
                        else
                        {
                            return static_cast<T>(val);
                        }
                    }

                    else
                    {
                        static_assert(!sizeof(T), "Unsupported conversion");
                    }

                }, value);
            }
    };
    struct NodeParameter
    {
        std::string name;
        ParamType type;
        PVariant value;
        //QVariant value;

        // List için
        //QVariantList listValues;
    };
}
