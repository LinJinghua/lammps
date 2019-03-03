#include <cassert>
#include <string>
#include <vector>
// #include <map>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <initializer_list>

typedef struct TypeKey {
    static const int MAX_TYPE = 4;

    std::string types;
    TypeKey() = default;

    TypeKey(const std::vector<std::string>& vec) {
        assert(vec.size() > 0);
        std::vector<std::string> permutation(vec.begin(), vec.begin() + 1);
        if (vec.size() > 2) {
            this->types += vec[1];
            permutation.insert(permutation.end(), vec.begin() + 2, vec.end());
        } else {
            permutation.insert(permutation.end(), vec.begin() + 1, vec.end());
        }
        std::sort(permutation.begin(), permutation.end());
        for (auto&& i : permutation) {
            this->types += std::move(i);
        }
    }

    bool operator<(const TypeKey& t) const {
        return this->types < t.types;
    }

    bool operator==(const TypeKey& t) const {
        return this->types == t.types;
    }

    bool operator>(const TypeKey& t) const {
        return this->types > t.types;
    }

    static constexpr const char* delimiter = "\f";
    static std::string get_replace_type(const std::string& type);

} TypeKey;

namespace std {
    template<> struct hash<TypeKey> {
        std::size_t operator()(const TypeKey &t) const {
            return std::hash<std::string>{}(t.types);
        }
    };
}

typedef struct Parameter {
    static const int MAX_PARAM = 6;

    double param[MAX_PARAM];

    Parameter() = default;
} Parameter;

typedef struct PrmData {
    // typedef std::map<TypeKey, Parameter> parameter_map;
    typedef std::unordered_map<TypeKey, Parameter> parameter_map;
    typedef std::unordered_map<std::string, std::string> replace_map;

    parameter_map bond;
    parameter_map angle;
    parameter_map dihedral;
    parameter_map improper;
    replace_map replace;
} PrmData;

extern PrmData _prm_data_global;
extern void ReadPrmFile();
