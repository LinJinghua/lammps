#include <cassert>
#include <string>
#include <vector>
// #include <map>
#include <unordered_map>
#include <algorithm>
#include <type_traits>
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


typedef struct PrmData {
    static PrmData _prm_data;

    static constexpr int BOND_ID = 0;
    static constexpr int ANGLE_ID = 1;
    static constexpr int DIHEDRAL_ID = 2;
    static constexpr int IMPROPER_ID = 3;

    template <int _Id, int _NType, int _NParam>
    struct _ParameterType {
        typedef std::unordered_map<TypeKey, _ParameterType> map;

        static const int id = _Id;
        static const int NType = _NType;
        static const int NParam = _NParam;

        double param[_NParam];


        template <int COL>
        static int set_parameter(const char potential_types[][COL], double* params) {
            const int Ntype = PrmData::ParameterType<_Id>::type::NType;
            const int NParam = PrmData::ParameterType<_Id>::type::NParam;

            TypeKey key(std::vector<std::string>(potential_types,
                                                 potential_types + Ntype));
            // printf("[types] %s\n", key.types.c_str());
            const auto& vec = PrmData::_prm_data.get_map<_Id>();
            auto iter = vec.find(key);
            if (iter != vec.end()) {
                const auto& param = iter->second;
                for (int i = NParam; i--; ) {
                    params[i] = param.param[i];
                }
                return 1;
            }
            return 0;
        }
    };

    template <int _Id>
    struct ParameterType {
        //typedef std::conditional_t<_Id == 0, _ParameterType<BOND_ID, 2, 2>,
        //    std::conditional_t<_Id == 1, _ParameterType<ANGLE_ID, 3, 2>,
        //    std::conditional_t<_Id == 2, _ParameterType<DIHEDRAL_ID, 4, 3>,
        //    std::enable_if_t<_Id == 3, _ParameterType<IMPROPER_ID, 4, 2> >
        //    >
        //    >
        //>  type;
        typedef std::conditional_t < _Id == BOND_ID, _ParameterType<BOND_ID, 2, 2>,
            std::conditional_t<_Id == ANGLE_ID, _ParameterType<ANGLE_ID, 3, 2>,
            std::conditional_t<_Id == DIHEDRAL_ID, _ParameterType<DIHEDRAL_ID, 4, 3>,
            _ParameterType<IMPROPER_ID, 4, 2>
            > > > type;
    };

    template <int _Id>
    typename ParameterType<_Id>::type::map& get_map() {
        using map_type = typename ParameterType<_Id>::type::map;
        if (_Id == BOND_ID) {
            return *reinterpret_cast<map_type*>(&(this->bond));
        } else if (_Id == ANGLE_ID) {
            return *reinterpret_cast<map_type*>(&(this->angle));;
        } else if (_Id == DIHEDRAL_ID) {
            return *reinterpret_cast<map_type*>(&(this->dihedral));;
        } else {
            return *reinterpret_cast<map_type*>(&(this->improper));;
        }
    }

    static std::string get_raw_type(const std::string& type) {
        return PrmData::_prm_data.potential[type];
    }

    typedef ParameterType<BOND_ID>::type BondParameter;
    typedef ParameterType<ANGLE_ID>::type AngleParameter;
    typedef ParameterType<DIHEDRAL_ID>::type DihedralParameter;
    typedef ParameterType<IMPROPER_ID>::type ImproperParameter;
    typedef std::unordered_map<std::string, std::string> potential_map;
    typedef std::vector<std::string> atom_map;

    ParameterType<BOND_ID>::type::map bond;
    ParameterType<ANGLE_ID>::type::map angle;
    ParameterType<DIHEDRAL_ID>::type::map dihedral;
    ParameterType<IMPROPER_ID>::type::map improper;
    potential_map potential, replace;
    atom_map atom;
} PrmData;

extern void ReadPrmFile();
