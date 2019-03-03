/**
 *   This routine reads the data from a .prm forcefield file and stores it in
 *   dynamically allocated memory. This allows for fast searches of the
 *   file.
 *
 */

#include "msi2lmp.h"
#include "PrmData.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#if defined(_DEBUG)
#define DEBUG_ReadPrmFile
#endif // _DEBUG


#ifndef MAX_LINE_C
#define MAX_LINE_C 0x10000
#endif
#ifndef MAX_LINE_TYPE_C
#define MAX_LINE_TYPE_C 0x100
#endif

#define ALIAS_TEMPLATE_FUNCTION(identifier, function_name, ...)     \
template <class... T>                                               \
inline auto identifier(T&&... t) {                                  \
    return function_name<__VA_ARGS__>(std::forward<T>(t)...);       \
}

PrmData _prm_data_global;

static int _file_line_global = 0;

template <int _EOF_Return>
char* get_line_(FILE* fp) {
    static char buf[MAX_LINE_C];
    if (fgets(buf, sizeof(buf), fp) == NULL) {
        if (feof(fp)) {
            if (_EOF_Return) {
                return NULL;
            }
            fprintf(stderr, "Unexpected end of file\n");
        } else if (ferror(fp)) {
            fprintf(stderr, "I/O error when reading\n");
        } else {
            fprintf(stderr, "Unexpected error occurred when reading\n");
        }
        exit(1);
    }
    ++_file_line_global;
    // printf("%d %s", _file_line_global, buf);
    return buf;
}

ALIAS_TEMPLATE_FUNCTION(get_line_eof, get_line_, 1)
ALIAS_TEMPLATE_FUNCTION(get_line, get_line_, 0)

const char* skip_blank(const char* str) {
    while (*str && std::isspace(static_cast<unsigned char>(*str))) {
        ++str;
    }
    return str;
}

const char* find_blank(const char* str) {
    while (*str && !std::isspace(static_cast<unsigned char>(*str))) {
        ++str;
    }
    return str;
}

int strstr_blank(const char* blank, const char* str) {
    blank = skip_blank(blank);
    int len = strlen(str);
    return strncmp(blank, str, len) == 0 && *skip_blank(blank + len) == '\0';
}

int is_comment(const char* str) {
    str = skip_blank(str);
    return *str == '\0' || *str == '!';
}

int is_alpha(const char* str) {
    return std::isalpha(static_cast<unsigned char>(*skip_blank(str)));
}

const char* get_alpha_line(FILE* fp) {
    const char* buf;
    do {
        buf = get_line_eof(fp);
        // printf("skip %d %s\n", _file_line_global, buf);
    // } while (buf && !is_alpha(buf));
    } while (buf && is_comment(buf));
    // printf("skip %d %s\n", _file_line_global, buf);
    return buf;
}

template <int NType, int NParam>
const char* get_parameter(FILE* fp, PrmData::parameter_map& vec) {
    while (1) {
        const char* _buf = get_alpha_line(fp), *buf = _buf;
        if (buf == NULL) {
            return buf;
        }
        std::vector<std::string> types(NType);

        for (int i = 0; i < NType; ++i) {
            if (*buf == '\0') {
                return _buf;
            }
            const char* start = skip_blank(buf);
            buf = find_blank(start);
            types[i].assign(start, buf - start);
        }

        Parameter param;
        for (int i = 0; i < NParam; ++i) {
            char* end;
            param.param[i] = strtod(buf, &end);
            if (end == buf) {
                return _buf;
            }
            buf = end;
        }

        for (int i = 0; i < NType; ++i) {
            types[i] = TypeKey::get_replace_type(types[i]);
        }
        TypeKey key(types);
        if (vec.count(key)) {
            fprintf(stdout, "[Warning] line repeated(same keys), using new line: %s", _buf);
        }
        vec.emplace(key, param);
    }
}

#ifdef USING_MARCO
#define GET_PARAMETER_FUNC(func, vec, format, type_n, param_n, ...)     \
const char* get_ ## func (FILE* fp) {                                   \
    char type_id[type_n][MAX_LINE_TYPE_C];                              \
    double param[param_n];                                              \
    while (1) {                                                         \
        const char* buf = get_alpha_line(fp);                           \
        if (buf == NULL ||                                              \
            sscanf(buf, format, __VA_ARGS__) != (type_n + param_n)) {   \
            return buf;                                                 \
        }                                                               \
        vec.emplace_back();                                             \
        Parameter& data = vec.back();                                   \
        for (int i = 0; i < type_n; ++i) {                              \
            data.type[i] = type_id[i];                                  \
        }                                                               \
        for (int i = 0; i < param_n; ++i) {                             \
            data.param[i] = param[i];                                   \
        }                                                               \
    }                                                                   \
}

GET_PARAMETER_FUNC(bond, _prm_data_global.bond, "%255s%255s%lf%lf",
    2, 2, type_id, type_id + 1, param, param + 1);
GET_PARAMETER_FUNC(angle, _prm_data_global.angle, "%255s%255s%255s%lf%lf",
    3, 2, type_id, type_id + 1, type_id + 2, param, param + 1);
GET_PARAMETER_FUNC(dihedral, _prm_data_global.dihedral, "%255s%255s%255s%255s%lf%lf%lf",
    4, 3, type_id, type_id + 1, type_id + 2, type_id + 3, param, param + 1, param + 2);
GET_PARAMETER_FUNC(improper, _prm_data_global.improper, "%255s%255s%255s%255s%lf%lf",
    4, 2, type_id, type_id + 1, type_id + 2, type_id + 3, param, param + 1);

#else

#define GET_PARAMETER_FUNC(func, vec, type_n, param_n)                  \
inline const char* get_ ## func (FILE* fp) {                            \
    return get_parameter<type_n, param_n>(fp, vec);                     \
}

GET_PARAMETER_FUNC(bond, _prm_data_global.bond, 2, 2);
GET_PARAMETER_FUNC(angle, _prm_data_global.angle, 3, 2);
GET_PARAMETER_FUNC(dihedral, _prm_data_global.dihedral, 4, 3);
GET_PARAMETER_FUNC(improper, _prm_data_global.improper, 4, 2);

#endif // USING_MARCO

FILE* get_file(const char* filename, const char* mode) {
    FILE* fp = fopen(filename, mode);
    if (!fp) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
        // return NULL;
    }
    return fp;
}

void print_prm_data(const char* id, const PrmData::parameter_map& vec,
                    int param_n) {
    fprintf(stdout, "[PrmData] %s %d\n", id, static_cast<int>(vec.size()));
    for (const auto& i : vec) {
        fprintf(stdout, "%16s\t", i.first.types.c_str());
        for (int j = 0; j < param_n; ++j) {
            fprintf(stdout, "%10lf\t", i.second.param[j]);
        }
        fprintf(stdout, "\n");
    }
}

void print_prm_data() {
    print_prm_data("BONDS", _prm_data_global.bond, 2);
    print_prm_data("ANGLES", _prm_data_global.angle, 2);
    print_prm_data("DIHEDRALS", _prm_data_global.dihedral, 3);
    print_prm_data("IMPROPER", _prm_data_global.improper, 2);
}

int get_prm_data(FILE* fp) {
    const char* bond_line = "BONDS";
    const char* angle_line = "ANGLES";
    const char* dihedral_line = "DIHEDRALS";
    const char* improper_line = "IMPROPER";

    const char* buf = get_alpha_line(fp);
    while (buf) {
        if (strstr_blank(buf, bond_line)) {
            buf = get_bond(fp);
        } else if (strstr_blank(buf, angle_line)) {
            buf = get_angle(fp);
        } else if (strstr_blank(buf, dihedral_line)) {
            buf = get_dihedral(fp);
        } else if (strstr_blank(buf, improper_line)) {
            buf = get_improper(fp);
        } else {
            fprintf(stderr, "[ReadPrmFile] Warning: Unknown identifier \"%s\"\n", buf);
            buf = get_alpha_line(fp);
        }
    }

#if defined(DEBUG_ReadPrmFile)
        print_prm_data();
#endif // DEBUG_ReadPrmFile

    return 0;
}

std::string TypeKey::get_replace_type(const std::string& type) {
    static int id = 0;
    static std::unordered_map<std::string, std::string> replace;
    if (replace.count(type) == 0) {
        std::string replace_type = TypeKey::delimiter + std::to_string(++id);
        // sizeof(atom.potential) == 6
        if (replace_type.size() >= sizeof(Atom{}.potential)) {
            fprintf(stderr, "the type of atom should < %d\n", id);
            exit(118);
        }
        replace[type] = replace_type;
    }
    return replace[type];
}

const char* is_replace_type(const Atom& atom) {
    if (strcmp(atom.element, "C") == 0) {
        std::unordered_map<std::string, int> have;
        for (int i = atom.no_connect; i--; ) {
            ++have[atoms[atom.conn_no[i]].element];
        }
        if (have["F"]) {
            return "C4_c1_f3";
        }
        int N_num = have["N"];
        if (N_num == 0) {
            return "C3_c2";
        } else if (N_num == 1) {
            return "C3_c2_n1";
        } else if (N_num == 2) {
            return "C3_c1_n2";
        }
    } else if (strcmp(atom.element, "H") == 0) {
        return "H1_c";
    } else if (strcmp(atom.element, "Cu") == 0) {
        return "CU2_nn";
    } else if (strcmp(atom.element, "N") == 0) {
        return "N3_c2_cu1";
    } else if (strcmp(atom.element, "F") == 0) {
        return "F1_c";
    }
    return nullptr;
}

int replace_atom_type() {
    for (int i = 0; i < total_no_atoms; ++i) {
        Atom& atom = atoms[i];
        const char* type_str = is_replace_type(atom);
        if (type_str) {
            std::string type = TypeKey::get_replace_type(type_str);
            _prm_data_global.replace[type] = atom.potential;
#if defined(DEBUG_ReadPrmFile)
            fprintf(stdout, "[replace] %8s -> %8s(%s)\n", atom.potential, type_str, type.c_str());
#endif // DEBUG_ReadPrmFile
            strncpy(atom.potential, type.c_str(), sizeof(atom.potential) - 1);
        }
    }
    return 0;
}

void ReadPrmFile() {
    replace_atom_type();
    FILE* fp = get_file((std::string(rootname) + ".prm").c_str(), "r");
    get_prm_data(fp);
    if (fp != stdin) {
        fclose(fp);
    }
}

#undef MAX_LINE_C
#undef MAX_LINE_TYPE_C
#undef ALIAS_TEMPLATE_FUNCTION
#undef GET_PARAMETER_FUNC
