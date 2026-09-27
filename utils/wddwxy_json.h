#include <string>
#include "../third_party/nlohmann/json.hpp"

namespace WDDWXY {
    class Json {
    private:
        nlohmann::json j;
    public:
        Json() = default;
        Json(const nlohmann::json _j);
        ~Json() = default;
        Json &operator[](const char index[]);
        Json &operator[](size_t index);
        template<typename T>
        T get(const T &deflt=T()) {
            try {
                return this->j.get<T>();
            } catch (const std::exception &e) {
                return deflt;
            }
            return deflt;
        }
        template<>
        int get<int>(const int &deflt) {
            if (this->j.is_number_integer()) {
                return this->j.get<int>();
            }
            return deflt;
        }
        operator int() const;
        template<>
        unsigned int get<unsigned int>(const unsigned int &deflt) {
            if (this->j.is_number_unsigned()) {
                return this->j.get<unsigned int>();
            }
            return deflt;
        }
        operator unsigned int() const;
        template<>
        double get<double>(const double &deflt) {
            if (this->j.is_number_float()) {
                return this->j.get<double>();
            }
            return deflt;
        }
        operator double() const;
        template<>
        bool get<bool>(const bool &deflt) {
            if (this->j.is_boolean()) {
                return this->j.get<bool>();
            }
            return deflt;
        }
        operator bool() const;
        template<>
        std::string get<std::string>(const std::string &deflt) {
            if (this->j.is_string()) {
                return this->j.get<std::string>();
            }
            return deflt;
        }
        operator std::string() const;
        template<>
        Json get<Json>(const Json &deflt) {
            if (this->j.is_object()) {
                return this->j.get<Json>();
            }
            return deflt;
        }
        friend void to_json(nlohmann::json &nlohmann_json_j,
                            const Json &nlohmann_json_t) {
            nlohmann_json_j = nlohmann_json_t.j;
        }
        friend void from_json(const nlohmann::json &nlohmann_json_j,
                              Json &nlohmann_json_t) {
            nlohmann_json_j.get_to(nlohmann_json_t.j);
        }
    };
}
