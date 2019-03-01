#include <string>
#include <vector>

typedef struct Parameter {
    static const int MAX_TYPE = 4;
    static const int MAX_PARAM = 6;

    std::string type[MAX_TYPE];
    double param[MAX_PARAM];

    Parameter() = default;
} Parameter;

typedef struct PrmData {
    std::vector<Parameter> bond;
    std::vector<Parameter> angle;
    std::vector<Parameter> dihedral;
    std::vector<Parameter> improper;
} PrmData;

extern PrmData _prm_data_global;
extern void ReadPrmFile();
