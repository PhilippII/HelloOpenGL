#include "import_mesh.h"

#include "cpu_mesh_utils.h"

#include <fstream>
#include <sstream>
#include <regex>

#include "debug_utils.h"


using std::string, std::vector;
using std::cout, std::cerr;
using std::ifstream, std::istringstream, std::ostream, std::getline;


// used by wavefrontObjectToMesh(...)
template <typename Elem_Src>
static std::vector<GLbyte> toByteVector(const std::vector<Elem_Src>& v) {
    using res_size_t = std::vector<GLbyte>::size_type;
    res_size_t resSize = v.size() * sizeof(Elem_Src);
    std::vector<GLbyte> res;
    const GLbyte* p = reinterpret_cast<const GLbyte*>(v.data());
    for (res_size_t i = 0; i < resSize; ++i) {
        res.push_back(p[i]);
    }
    return res;
}



struct WavefrontObject {
    string name;

    enum class MultiIndexFormat {
        UNKNOWN = -1,
        V = 0,
        V_VT = 1,
        V_VN = 2,
        V_VT_VN = 3
    };

    MultiIndexFormat miFormat;

    // we keep verts_* as vectors instead of using CPUVertexArrays directly
    // to avoid casting to vectors of GLbyte:
    std::vector<std::array<float, 3>> verts_v;
    std::vector<std::array<float, 2>> verts_vt;
    std::vector<std::array<float, 3>> verts_vn;

    CPUMultiIndexBuffer<GLuint, 1> mib_v;
    CPUMultiIndexBuffer<GLuint, 2> mib_v_vt;
    CPUMultiIndexBuffer<GLuint, 2> mib_v_vn;
    CPUMultiIndexBuffer<GLuint, 3> mib_v_vt_vn;
};

ostream& operator<<(ostream& os, const WavefrontObject& obj) {
    cout << "vertex positions:\n";
    for (auto& v : obj.verts_v) {
        cout << "x=" << v[0]
             << " y=" << v[1]
             << " z=" << v[2] << '\n';
    }
    cout << '\n';
    cout << "texture coordinates:\n";
    for (auto& vt : obj.verts_vt) {
        cout << "u=" << vt[0]
             << " v=" << vt[1] << '\n';
    }
    cout << '\n';
    cout << "normals:\n";
    for (auto& vn : obj.verts_vn) {
        cout << "nx=" << vn[0]
             << " ny=" << vn[1]
             << " nz=" << vn[2] << '\n';
    }
    cout << '\n';
    cout << "faces:\n";
    switch (obj.miFormat) {
    case WavefrontObject::MultiIndexFormat::UNKNOWN:
        cout << "error no faces specified\n";
        break;
    case WavefrontObject::MultiIndexFormat::V:
        for (auto& v : obj.mib_v.indices) {
            cout << "vertex: v=" << v[0] << '\n';
        }
        break;
    case WavefrontObject::MultiIndexFormat::V_VT:
        for (auto& v : obj.mib_v_vt.indices) {
            cout << "vertex: v=" << v[0]
                 << " vt=" << v[1]
                 << '\n';
        }
        break;
    case WavefrontObject::MultiIndexFormat::V_VN:
        for (auto& v : obj.mib_v_vn.indices) {
            cout << "vertex: v=" << v[0]
                 << " vn=" << v[1]
                 << '\n';
        }
        break;
    case WavefrontObject::MultiIndexFormat::V_VT_VN:
        for (auto& v : obj.mib_v_vt_vn.indices) {
            cout << "vertex: v=" << v[0]
                 << " vt=" << v[1]
                 << " vn=" << v[2]
                 << '\n';
        }
        break;
    default:
        break;
    }
    return os;
}

inline bool parsePosition(WavefrontObject& obj, istringstream& iss, bool invert_z) {
    std::array<float, 3> v;
    unsigned int i;
    for (i = 0; i < v.size() && iss >> v[i]; ++i)
        {}
    if (i < v.size()) {
        cerr << "error reading vertex position\n";
        myAssert(false);
        return false;
    }
    if (invert_z) {
        v[2] = -v[2];
    }
    obj.verts_v.push_back(v);
    float value;
    if (iss >> value) {
        cout << "warning: only 3 dimensional positions are supported by this implementation\n";
    }
    return true;
}

inline bool parseTexCoord(WavefrontObject& obj, istringstream& iss) {
    std::array<float, 2> vt;
    unsigned int i;
    for (i = 0; i < vt.size() && iss >> vt[i]; ++i)
        {}
    if (i < vt.size()) {
        cerr << "error reading texture coordinate\n";
        myAssert(false);
        return false;
    }
    obj.verts_vt.push_back(vt);
    float value;
    if (iss >> value) {
        cout << "warning: only 2-dimensional texture coordinates are supported by this implementation\n";
    }
    return true;
}

inline bool parseNormal(WavefrontObject& obj, istringstream& iss, bool invert_z) {
    std::array<float, 3> vn;
    unsigned int i;
    for (i = 0; i < vn.size() && iss >> vn[i]; ++i)
        {}
    if (i < vn.size()) {
        cerr << "error reading normal\n";
        myAssert(false);
        return false;
    }
    if (invert_z) {
        vn[2] = -vn[2];
    }
    obj.verts_vn.push_back(vn);
    float value;
    if (iss >> value) {
        cerr << "error normal should only have three dimensions\n";
        myAssert(false);
        return false;
    }
    return true;
}

struct VertexCountSum {
    GLuint v;
    GLuint vt;
    GLuint vn;
};

inline bool parseFace(WavefrontObject& obj, istringstream& iss, const VertexCountSum& count_sum, const vector<std::regex>& mi_pats) {
    string multIndStr;
    debugDo(int n = 1);
    while (iss >> multIndStr) {
        std::smatch matches;
        bool success = false;
        if (obj.miFormat == WavefrontObject::MultiIndexFormat::UNKNOWN) {
            for (unsigned int i = 0; i < mi_pats.size() && !success; ++i) {
                if (regex_match(multIndStr, matches, mi_pats[i])) {
                    success = true;
                    obj.miFormat = static_cast<WavefrontObject::MultiIndexFormat>(i); // or static_cast ?
                }
            }
        } else {
            success = regex_match(multIndStr, matches,
                                  mi_pats[static_cast<int>(obj.miFormat)]);
        }
        if (!success) {
            cerr << "error parsing vertex: " << multIndStr << '\n';
            myAssert(false);
            return false;
        }

        GLuint v, vt, vn;
        int i = 1;
        { // if (true)
            GLint v_raw = static_cast<GLuint>(std::stoi(matches[i++].str()));
            if (v_raw < 0) {
                v = static_cast<GLuint>(obj.verts_v.size()) + v_raw;
            } else if (static_cast<GLuint>(v_raw) <= count_sum.v) {
                cerr << "error: sharing vertices between different objects is not supported by this implementation\n";
                myAssert(false);
                return false;
            } else {
                v = v_raw - count_sum.v - 1;
            }
        }

        if (obj.miFormat == WavefrontObject::MultiIndexFormat::V_VT
                || obj.miFormat == WavefrontObject::MultiIndexFormat::V_VT_VN) {
            GLint vt_raw = static_cast<GLuint>(std::stoi(matches[i++].str()));
            if (vt_raw < 0) {
                vt = static_cast<GLuint>(obj.verts_vt.size()) + vt_raw;
            } else if (static_cast<GLuint>(vt_raw) <= count_sum.vt) {
                cerr << "error: sharing vertices between different object is not supported by this implementation\n";
                myAssert(false);
                return false;
            } else {
                vt = vt_raw - count_sum.vt - 1;
            }
        }

        if (obj.miFormat == WavefrontObject::MultiIndexFormat::V_VN
                || obj.miFormat == WavefrontObject::MultiIndexFormat::V_VT_VN) {
            GLint vn_raw = static_cast<GLuint>(std::stoi(matches[i++].str()));
            if (vn_raw < 0) {
                vn = static_cast<GLuint>(obj.verts_vn.size()) + vn_raw;
            } else if (static_cast<GLuint>(vn_raw) <= count_sum.vn) {
                cerr << "error: sharing vertices between different object is not supported by this implementation\n";
                myAssert(false);
                return false;
            } else {
                vn = vn_raw - count_sum.vn - 1;
            }
        }

        switch (obj.miFormat) {
        case WavefrontObject::MultiIndexFormat::V:
            obj.mib_v.indices.push_back({v});
            break;
        case WavefrontObject::MultiIndexFormat::V_VT:
            obj.mib_v_vt.indices.push_back({v, vt});
            break;
        case WavefrontObject::MultiIndexFormat::V_VN:
            obj.mib_v_vn.indices.push_back({v, vn});
            break;
        case WavefrontObject::MultiIndexFormat::V_VT_VN:
            obj.mib_v_vt_vn.indices.push_back({v, vt, vn});
            break;
        default:
            myAssert(false);
            break;
        }
        debugDo(++n);
    }
    switch (obj.miFormat) {
    case WavefrontObject::MultiIndexFormat::V:
        obj.mib_v.indices.push_back(obj.mib_v.primitiveRestartMultiIndex);
        break;
    case WavefrontObject::MultiIndexFormat::V_VT:
        obj.mib_v_vt.indices.push_back(obj.mib_v_vt.primitiveRestartMultiIndex);
        break;
    case WavefrontObject::MultiIndexFormat::V_VN:
        obj.mib_v_vn.indices.push_back(obj.mib_v_vn.primitiveRestartMultiIndex);
        break;
    case WavefrontObject::MultiIndexFormat::V_VT_VN:
        obj.mib_v_vt_vn.indices.push_back(obj.mib_v_vt_vn.primitiveRestartMultiIndex);
        break;
    default:
        myAssert(false);
        break;
    }
    myAssert(n >= 3);
    return true;
}

// does not return valid CPUMesh if obj.miFormat is ...::UNKNOWN
inline CPUMesh<GLuint> wavefrontObjectToMesh(const WavefrontObject obj, bool* success) {
    VertexBufferLayout layout_v;
    layout_v.append<float>(3);
    VertexBufferLayout layout_vt;
    layout_vt.append<float>(2);
    VertexBufferLayout layout_vn;
    layout_vn.append<float>(3);
    *success = true;
    CPUMesh<GLuint> res(
        [&](){
            switch (obj.miFormat) {
            case WavefrontObject::MultiIndexFormat::UNKNOWN:
                break;
            case WavefrontObject::MultiIndexFormat::V:
                {
                    CPUVertexArray va_v {layout_v,
                                        toByteVector(obj.verts_v)};
                    CPUMultiIndexMesh<GLuint, 1> miMesh {obj.mib_v, std::array<CPUVertexArray, 1>{va_v}};
                    return unifyIndexBuffer(miMesh);
                }
                break;
            case WavefrontObject::MultiIndexFormat::V_VT:
                {
                    CPUVertexArray va_v {layout_v,
                                         toByteVector(obj.verts_v)};
                    CPUVertexArray va_vt {layout_vt,
                                          toByteVector(obj.verts_vt)};
                    CPUMultiIndexMesh<GLuint, 2> miMesh {obj.mib_v_vt, std::array<CPUVertexArray, 2>{va_v, va_vt}};
                    return unifyIndexBuffer(miMesh);
                }
                break;
            case WavefrontObject::MultiIndexFormat::V_VN:
                {
                    CPUVertexArray va_v {layout_v,
                                         toByteVector(obj.verts_v)};
                    CPUVertexArray va_vn {layout_vn,
                                          toByteVector(obj.verts_vn)};
                    CPUMultiIndexMesh<GLuint, 2> miMesh {obj.mib_v_vn, std::array<CPUVertexArray, 2>{va_v, va_vn}};
                    return unifyIndexBuffer(miMesh);
                }
                break;
            case WavefrontObject::MultiIndexFormat::V_VT_VN:
                {
                    CPUVertexArray va_v {layout_v,
                                         toByteVector(obj.verts_v)};
                    CPUVertexArray va_vt {layout_vt,
                                          toByteVector(obj.verts_vt)};
                    CPUVertexArray va_vn {layout_vn,
                                          toByteVector(obj.verts_vn)};
                    CPUMultiIndexMesh<GLuint, 3> miMesh {obj.mib_v_vt_vn, std::array<CPUVertexArray, 3>{va_v, va_vt, va_vn}};
                    return unifyIndexBuffer(miMesh);
                }
                break;
            default:
                myAssert(false);
                break;
            }
            *success = false;
            return CPUMesh<GLuint>();
        }());
    if (!(*success)) {
        return CPUMesh<GLuint>();
    }
    // TODO: should CPUMesh also store obj.name?
    res.ib = applyTriangleFan(res.ib);
    res.va.layout.setDefaultLocations(); // concatenating the separate layouts for
                                         // v, vt and vn in unifyIndexBuffer(..) leads to
                                         // repetition of vertex attribute locations
    return res;
}

std::vector<CPUMesh<GLuint>> loadOBJfile(std::string filepath, bool invert_z)
{
    ifstream ifs {filepath};
    if (!ifs) {
        cerr << "error opening file " << filepath << '\n';
        return std::vector<CPUMesh<GLuint>>();
    }

    vector<std::regex> mi_pats = {  // indices should align to WavefrontObject::MultiIndexFormat
        std::regex{R"(([+-]?\d+))"}, // V
        std::regex{R"(([+-]?\d+)/([+-]?\d+))"}, // V_VT
        std::regex{R"(([+-]?\d+)//([+-]?\d+))"}, // V_VN
        std::regex{R"(([+-]?\d+)/([+-]?\d+)/([+-]?\d+))"} // V_VT_VN
    };

    vector<CPUMesh<GLuint>> results;

    VertexCountSum count_sum = {0, 0, 0};

    string nextName;

    while (ifs) {
        WavefrontObject obj;

        obj.name = nextName;

        obj.miFormat = WavefrontObject::MultiIndexFormat::UNKNOWN;

        obj.mib_v.primitiveType = GL_TRIANGLE_FAN;
        obj.mib_v.primitiveRestart = true;
        obj.mib_v.primitiveRestartMultiIndex = std::array<GLuint, 1>{std::numeric_limits<GLuint>::max()};
        obj.mib_v_vt.primitiveType = GL_TRIANGLE_FAN;
        obj.mib_v_vt.primitiveRestart = true;
        obj.mib_v_vt.primitiveRestartMultiIndex = std::array<GLuint, 2>{std::numeric_limits<GLuint>::max(),
                                                                        std::numeric_limits<GLuint>::max()};
        obj.mib_v_vn.primitiveType = GL_TRIANGLE_FAN;
        obj.mib_v_vn.primitiveRestart = true;
        obj.mib_v_vn.primitiveRestartMultiIndex = std::array<GLuint, 2>{std::numeric_limits<GLuint>::max(),
                                                                        std::numeric_limits<GLuint>::max()};
        obj.mib_v_vt_vn.primitiveType = GL_TRIANGLE_FAN;
        obj.mib_v_vt_vn.primitiveRestart = true;
        obj.mib_v_vt_vn.primitiveRestartMultiIndex = std::array<GLuint, 3>{std::numeric_limits<GLuint>::max(),
                                                                           std::numeric_limits<GLuint>::max(),
                                                                           std::numeric_limits<GLuint>::max()};

        bool startNext = false;
        string line;
        while (!startNext && getline(ifs, line)) {
            // cout << line << '\n';
            istringstream iss {line};
            string opcodeStr;
            if (!(iss >> opcodeStr)) {
                continue;
            }
            if (opcodeStr[0] == '#') {
                string commentStr;
                getline(iss, commentStr);
                cout << "OBJ-file comment: " << opcodeStr << " " << commentStr << '\n';
            } else if (opcodeStr == "v") {
                if (!parsePosition(obj, iss, invert_z)) {
                    return vector<CPUMesh<GLuint>>();
                }
            } else if (opcodeStr == "vt") {
                if (!parseTexCoord(obj, iss)) {
                    return vector<CPUMesh<GLuint>>();
                }
            } else if (opcodeStr == "vn") {
                if (!parseNormal(obj, iss, invert_z)) {
                    return vector<CPUMesh<GLuint>>();
                }
            } else if (opcodeStr == "f") {
                if (!parseFace(obj, iss, count_sum, mi_pats)) {
                    return vector<CPUMesh<GLuint>>();
                }
            } else if (opcodeStr == "o") {
                if (!(iss >> nextName) || nextName.empty()) {
                    cerr << "error parsing name of object\n";
                }
                startNext = true;
            } else {
                cerr << "warning: unknown opcode " << opcodeStr << '\n';
            }
        }

        // better use bigger type for sum (as all meshes together might have much more vertices
        // than each individual mesh on its own)?
        count_sum.v += static_cast<GLuint>(obj.verts_v.size());
        count_sum.vt += static_cast<GLuint>(obj.verts_vt.size());
        count_sum.vn += static_cast<GLuint>(obj.verts_vn.size());

        cout << "finished parsing object " << obj.name
             << " from file " << filepath << '\n';
        // cout << obj;

        // now convert parsed object to a CPUMesh:
        if (obj.miFormat == WavefrontObject::MultiIndexFormat::UNKNOWN) {
            if (!obj.name.empty()) {
                cerr << "error: object " << obj.name << " has no faces\n";
            } // else { no faces where specified before the first object declared with
              // opcode "o" }
              // we do not consider this latter case to be an error.
        } else {
            bool success;
            results.push_back(wavefrontObjectToMesh(obj, &success));
            myAssert(success);
        }
    }

    return results;
}

