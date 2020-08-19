#include "import_mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <array>

#include "debug_utils.h"


using std::string, std::vector;
using std::cout, std::cerr;
using std::ifstream, std::istringstream, std::getline;




// TODO: change return type to CPUMesh
void readOBJtest(std::string filepath)
{
    ifstream ifs {filepath};
    if (!ifs) {
        cerr << "error opening file " << filepath << '\n';
        return;
    }

    enum class MultiIndexFormat {
        UNKNOWN = -1,
        V = 0,
        V_VT = 1,
        V_VN = 2,
        V_VT_VN = 3
    };

    MultiIndexFormat miFormat = MultiIndexFormat::UNKNOWN;

    vector<std::regex> mi_pats = {
        std::regex{R"((\d+))"}, // V
        std::regex{R"((\d+)/(\d+))"}, // V_VT
        std::regex{R"((\d+)//(\d+))"}, // V_VN
        std::regex{R"((\d+)/(\d+)/(\d+))"} // V_VT_VN
    };

    // std::vector<std::array<float, 3>> verts_v;
    // std::vector<std::array<float, 2>> verts_vt;
    // std::vector<std::array<float, 3>> verts_vn;

    std::vector<std::array<GLuint, 1>> indices_v;
    std::vector<std::array<GLuint, 2>> indices_v_vt;
    std::vector<std::array<GLuint, 2>> indices_v_vn;
    std::vector<std::array<GLuint, 3>> indices_v_vt_vn;

    string line;
    while (getline(ifs, line)) {
        cout << line << '\n';
        istringstream iss {line};
        string opcodeStr;
        if (!(iss >> opcodeStr)) {
            continue;
        }
        if (opcodeStr == "#") {
            string commentStr;
            getline(iss, commentStr);
            cout << "OBJ-file comment: " << commentStr << '\n';
        } else if (opcodeStr == "v") {
            cout << "vertex:\n";
            float value;
            if (iss >> value) {
                cout << "(" << value;
                while (iss >> value) {
                    cout << ", " << value;
                }
                cout << ")\n";
            }
        } else if (opcodeStr == "f") {
            string multIndStr;
            int n = 1;
            unsigned int i_first;
            while (iss >> multIndStr) {
                std::smatch matches;
                bool success = false;
                if (miFormat == MultiIndexFormat::UNKNOWN) {
                    for (int i = 0; i < mi_pats.size() && !success; ++i) {
                        if (regex_match(multIndStr, matches, mi_pats[i])) {
                            success = true;
                            miFormat = static_cast<MultiIndexFormat>(i); // or static_cast ?
                        }
                    }
                } else {
                    success = regex_match(multIndStr, matches,
                                          mi_pats[static_cast<int>(miFormat)]);
                }
                if (!success) {
                    cerr << "error parsing vertex: " << multIndStr << '\n';
                    cerr << "\tin face: " << line << '\n';
                    myAssert(false);
                    return;
                }
                GLuint v, vt, vn;
                switch (miFormat) {
                case MultiIndexFormat::V:
                    if (n == 1) {
                        i_first = indices_v.size();
                    } else if (n > 3) {
                        unsigned int i_last = indices_v.size() - 1;
                        indices_v.push_back(indices_v[i_first]);
                        indices_v.push_back(indices_v[i_last]);
                    }
                    v = static_cast<GLuint>(std::stoi(matches[1].str()));
                        // regular expression does not allow negative numbers here anyway
                        // so we can cast to unsigned
                    indices_v.push_back({v});
                    break;
                case MultiIndexFormat::V_VT:
                    if (n == 1) {
                        i_first = indices_v_vt.size();
                    } else if (n > 3) {
                        unsigned int i_last = indices_v_vt.size() - 1;
                        indices_v_vt.push_back(indices_v_vt[i_first]);
                        indices_v_vt.push_back(indices_v_vt[i_last]);
                    }
                    v = static_cast<GLuint>(std::stoi(matches[1].str()));
                    vt = static_cast<GLuint>(std::stoi(matches[2].str()));
                    indices_v_vt.push_back({v, vt});
                    break;
                case MultiIndexFormat::V_VN:
                    if (n == 1) {
                        i_first = indices_v_vn.size();
                    } else if (n > 3) {
                        unsigned int i_last = indices_v_vn.size() - 1;
                        indices_v_vn.push_back(indices_v_vn[i_first]);
                        indices_v_vn.push_back(indices_v_vn[i_last]);
                    }
                    v = static_cast<GLuint>(std::stoi(matches[1].str()));
                    vn = static_cast<GLuint>(std::stoi(matches[2].str()));
                    indices_v_vn.push_back({v, vn});
                    break;
                case MultiIndexFormat::V_VT_VN:
                    if (n == 1) {
                        i_first = indices_v_vt_vn.size();
                    } else if (n > 3) {
                        unsigned int i_last = indices_v_vt_vn.size() - 1;
                        indices_v_vt_vn.push_back(indices_v_vt_vn[i_first]);
                        indices_v_vt_vn.push_back(indices_v_vt_vn[i_last]);
                    }
                    v = static_cast<GLuint>(std::stoi(matches[1].str()));
                    vt = static_cast<GLuint>(std::stoi(matches[2].str()));
                    vn = static_cast<GLuint>(std::stoi(matches[3].str()));
                    indices_v_vt_vn.push_back({v, vt, vn});
                    break;
                default:
                    myAssert(false);
                    break;
                }
                ++n;
            }
            myAssert(n >= 3);
        } else {
            cerr << "warning: unknown opcode " << opcodeStr << '\n';
        }
    }

    cout << "finished parsing OBJfile " << filepath << '\n';
    switch (miFormat) {
    case MultiIndexFormat::UNKNOWN:
        cout << "error no faces specified\n";
        break;
    case MultiIndexFormat::V:
        for (auto& v : indices_v) {
            cout << "vertex: v=" << v[0] << '\n';
        }
        break;
    case MultiIndexFormat::V_VT:
        for (auto& v : indices_v_vt) {
            cout << "vertex: v=" << v[0]
                 << " vt=" << v[1]
                 << '\n';
        }
        break;
    case MultiIndexFormat::V_VN:
        for (auto& v : indices_v_vn) {
            cout << "vertex: v=" << v[0]
                 << " vn=" << v[1]
                 << '\n';
        }
        break;
    case MultiIndexFormat::V_VT_VN:
        for (auto& v : indices_v_vt_vn) {
            cout << "vertex: v=" << v[0]
                 << " vt=" << v[1]
                 << " vn=" << v[2]
                 << '\n';
        }
        break;
    default:
        break;
    }
}
