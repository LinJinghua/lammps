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
    printf("skip %d %s\n", _file_line_global, buf);
    return buf;
}

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


FILE* get_file(const char* filename, const char* mode) {
    FILE* fp = fopen(filename, mode);
    if(!fp) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
        // return NULL;
    }
    return fp;
}

void print_prm_data(const char* id, const std::vector<Parameter>& vec,
    int type_n, int param_n) {
    int len = static_cast<int>(vec.size());
    fprintf(stdout, "[PrmData] %s %d\n", id, len);
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < type_n; ++j) {
            fprintf(stdout, "%16s\t", vec[i].type[j].c_str());
        }
        for (int j = 0; j < param_n; ++j) {
            fprintf(stdout, "%16lf\t", vec[i].param[j]);
        }
        fprintf(stdout, "\n");
    }
}

void print_prm_data() {
    print_prm_data("BONDS", _prm_data_global.bond, 2, 2);
    print_prm_data("ANGLES", _prm_data_global.angle, 3, 2);
    print_prm_data("DIHEDRALS", _prm_data_global.dihedral, 4, 3);
    print_prm_data("IMPROPER", _prm_data_global.improper, 4, 2);
}

int process(FILE* fp) {
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

    if (1) {
        print_prm_data();
    }

    return 0;
}

void ReadPrmFile() {
    FILE* fp = get_file((std::string(rootname) + ".prm").c_str(), "r");
    process(fp);
    if (fp != stdin) {
        fclose(fp);
    }
}
