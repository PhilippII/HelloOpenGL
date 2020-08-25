#include "import_mesh.h"

#include <fstream>
#include <sstream>
#include <regex>

#include "debug_utils.h"


using std::string, std::vector;
using std::cout, std::cerr;
using std::ifstream, std::istringstream, std::getline;


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


CPUMesh<GLuint> readOBJ(std::string filepath, bool invert_z)
{
    ifstream ifs {filepath};
    if (!ifs) {
        cerr << "error opening file " << filepath << '\n';
        return CPUMesh<GLuint>{};
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

    std::vector<std::array<float, 3>> verts_v;
    std::vector<std::array<float, 2>> verts_vt;
    std::vector<std::array<float, 3>> verts_vn;

    // TODO: allocate CPUMultiIndexBuffer directly here already
    //          instead of raw arrays?
    std::vector<std::array<GLuint, 1>> indices_v;
    std::vector<std::array<GLuint, 2>> indices_v_vt;
    std::vector<std::array<GLuint, 2>> indices_v_vn;
    std::vector<std::array<GLuint, 3>> indices_v_vt_vn;
    GLuint restartIndex = std::numeric_limits<GLuint>::max();

    string line;
    while (getline(ifs, line)) {
        // cout << line << '\n';
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
            std::array<float, 3> v;
            unsigned int i;
            for (i = 0; i < v.size() && iss >> v[i]; ++i)
                {}
            if (i < v.size()) {
                cerr << "error reading vertex position:\n";
                cerr << '\t' << line << "\n";
                myAssert(false);
                return CPUMesh<GLuint>{};
            }
            if (invert_z) {
                v[2] = -v[2];
            }
            verts_v.push_back(v);
            float value;
            if (iss >> value) {
                cout << "warning: only 3 dimensional positions are not supported by this implementation\n";
                cout << '\t' << line << "\n";
            }
        } else if (opcodeStr == "vt") {
            std::array<float, 2> vt;
            unsigned int i;
            for (i = 0; i < vt.size() && iss >> vt[i]; ++i)
                {}
            if (i < vt.size()) {
                cerr << "error reading texture coordinate:\n";
                cerr << '\t' << line << "\n";
                myAssert(false);
                return CPUMesh<GLuint>{};
            }
            verts_vt.push_back(vt);
            float value;
            if (iss >> value) {
                cout << "warning: only 2-dimensional texture coordinates are supported by this implementation\n";
            }
        } else if (opcodeStr == "vn") {
            std::array<float, 3> vn;
            unsigned int i;
            for (i = 0; i < vn.size() && iss >> vn[i]; ++i)
                {}
            if (i < vn.size()) {
                cerr << "error reading normal:\n";
                cerr << '\t' << line << '\n';
                myAssert(false);
                return CPUMesh<GLuint>{};
            }
            if (invert_z) {
                vn[2] = -vn[2];
            }
            verts_vn.push_back(vn);
            float value;
            if (iss >> value) {
                cerr << "error normal should only have three dimensions:\n";
                cerr << '\t' << line << '\n';
                myAssert(false);
                return CPUMesh<GLuint>{};
            }
        } else if (opcodeStr == "f") {
            string multIndStr;
            int n = 1;
            while (iss >> multIndStr) {
                std::smatch matches;
                bool success = false;
                if (miFormat == MultiIndexFormat::UNKNOWN) {
                    for (unsigned int i = 0; i < mi_pats.size() && !success; ++i) {
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
                    return CPUMesh<GLuint>{};
                }
                GLuint v, vt, vn;
                switch (miFormat) {
                case MultiIndexFormat::V:
                    v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                        // regular expression does not allow negative numbers here anyway
                        // so we can cast to unsigned
                    indices_v.push_back({v});
                    break;
                case MultiIndexFormat::V_VT:
                    v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                    vt = static_cast<GLuint>(std::stoi(matches[2].str())) - 1;
                    indices_v_vt.push_back({v, vt});
                    break;
                case MultiIndexFormat::V_VN:
                    v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                    vn = static_cast<GLuint>(std::stoi(matches[2].str())) - 1;
                    indices_v_vn.push_back({v, vn});
                    break;
                case MultiIndexFormat::V_VT_VN:
                    v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                    vt = static_cast<GLuint>(std::stoi(matches[2].str())) - 1;
                    vn = static_cast<GLuint>(std::stoi(matches[3].str())) - 1;
                    indices_v_vt_vn.push_back({v, vt, vn});
                    break;
                default:
                    myAssert(false);
                    break;
                }
                ++n;
            }
            switch (miFormat) {
            case MultiIndexFormat::V:
                indices_v.push_back({restartIndex});
                break;
            case MultiIndexFormat::V_VT:
                indices_v_vt.push_back({restartIndex, restartIndex});
                break;
            case MultiIndexFormat::V_VN:
                indices_v_vn.push_back({restartIndex, restartIndex});
                break;
            case MultiIndexFormat::V_VT_VN:
                indices_v_vt_vn.push_back({restartIndex, restartIndex, restartIndex});
                break;
            default:
                myAssert(false);
                break;
            }
            myAssert(n >= 3);
        } else {
            cerr << "warning: unknown opcode " << opcodeStr << '\n';
        }
    }

    cout << "finished parsing OBJfile " << filepath << '\n';
    cout << "vertex positions:\n";
    for (auto& v : verts_v) {
        cout << "x=" << v[0]
             << " y=" << v[1]
             << " z=" << v[2] << '\n';
    }
    cout << '\n';
    cout << "texture coordinates:\n";
    for (auto& vt : verts_vt) {
        cout << "u=" << vt[0]
             << " v=" << vt[1] << '\n';
    }
    cout << '\n';
    cout << "normals:\n";
    for (auto& vn : verts_vn) {
        cout << "nx=" << vn[0]
             << " ny=" << vn[1]
             << " nz=" << vn[2] << '\n';
    }
    cout << '\n';
    cout << "faces:\n";
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

    VertexBufferLayout layout_v;
    layout_v.append<float>(3);
    VertexBufferLayout layout_vt;
    layout_vt.append<float>(2);
    VertexBufferLayout layout_vn;
    layout_vn.append<float>(3);
    switch (miFormat) {
    case MultiIndexFormat::UNKNOWN:
        cout << "error no faces specified\n";
        break;
    case MultiIndexFormat::V:
        {
            CPUVertexArray va_v {layout_v,
                                toByteVector(verts_v)};
            CPUMultiIndexBuffer<GLuint, 1> mib {indices_v, GL_TRIANGLE_FAN, true,
                                                std::array<GLuint, 1>{restartIndex}};
            CPUMultiIndexMesh<GLuint, 1> miMesh {mib, std::array<CPUVertexArray, 1>{va_v}};
            return unifyIndexBuffer(miMesh);
        }
        break;
    case MultiIndexFormat::V_VT:
        {
            CPUVertexArray va_v {layout_v,
                                 toByteVector(verts_v)};
            CPUVertexArray va_vt {layout_vt,
                                  toByteVector(verts_vt)};
            CPUMultiIndexBuffer<GLuint, 2> mib {indices_v_vt, GL_TRIANGLE_FAN, true,
                                                std::array<GLuint, 2>{restartIndex, restartIndex}};
            CPUMultiIndexMesh<GLuint, 2> miMesh {mib, std::array<CPUVertexArray, 2>{va_v, va_vt}};
            return unifyIndexBuffer(miMesh);
        }
        break;
    case MultiIndexFormat::V_VN:
        {
            CPUVertexArray va_v {layout_v,
                                 toByteVector(verts_v)};
            CPUVertexArray va_vn {layout_vn,
                                  toByteVector(verts_vn)};
            CPUMultiIndexBuffer<GLuint, 2> mib {indices_v_vn, GL_TRIANGLE_FAN, true,
                                                std::array<GLuint, 2>{restartIndex, restartIndex}};
            CPUMultiIndexMesh<GLuint, 2> miMesh {mib, std::array<CPUVertexArray, 2>{va_v, va_vn}};
            return unifyIndexBuffer(miMesh);
        }
        break;
    case MultiIndexFormat::V_VT_VN:
        {
            CPUVertexArray va_v {layout_v,
                                 toByteVector(verts_v)};
            CPUVertexArray va_vt {layout_vt,
                                  toByteVector(verts_vt)};
            CPUVertexArray va_vn {layout_vn,
                                  toByteVector(verts_vn)};
            CPUMultiIndexBuffer<GLuint, 3> mib {indices_v_vt_vn, GL_TRIANGLE_FAN, true,
                                                std::array<GLuint, 3>{restartIndex, restartIndex, restartIndex}};
            CPUMultiIndexMesh<GLuint, 3> miMesh {mib, std::array<CPUVertexArray, 3>{va_v, va_vt, va_vn}};
            return unifyIndexBuffer(miMesh);
        }
        break;
    default:
        break;
    }
    return CPUMesh<GLuint>{};
}

template <typename T>
static void printVec(std::ostream& os, GLint dimCount, T* p) {
    os << "{";
    if (dimCount > 0) {
        os << p[0];
    }
    for (int i = 1; i < dimCount; ++i) {
        os << ", " << p[i];
    }
    os << "}";
}

static void printAttribute(std::ostream& os, GLint dimCount, GLenum componentType, const GLvoid* data) {
    switch (componentType) {
      case GL_HALF_FLOAT:
        printVec(os, dimCount, reinterpret_cast<const GLhalf*>(data));
        break;
      case GL_FLOAT:
        printVec(os, dimCount, reinterpret_cast<const GLfloat*>(data));
        break;
      case GL_DOUBLE:
        printVec(os, dimCount, reinterpret_cast<const GLdouble*>(data));
        break;
      case GL_FIXED:
        printVec(os, dimCount, reinterpret_cast<const GLfixed*>(data));
        break;
      case GL_BYTE:
        printVec(os, dimCount, reinterpret_cast<const GLbyte*>(data));
        break;
      case GL_UNSIGNED_BYTE:
        printVec(os, dimCount, reinterpret_cast<const GLubyte*>(data));
        break;
      case GL_SHORT:
        printVec(os, dimCount, reinterpret_cast<const GLshort*>(data));
        break;
      case GL_UNSIGNED_SHORT:
        printVec(os, dimCount, reinterpret_cast<const GLushort*>(data));
        break;
      case GL_INT:
        printVec(os, dimCount, reinterpret_cast<const GLint*>(data));
        break;
      case GL_UNSIGNED_INT:
        printVec(os, dimCount, reinterpret_cast<const GLuint*>(data));
        break;
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
        // myAssert(dimCount == 4 || dimCount == GL_BGRA);
        cerr << "printing of types GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV\n";
        cerr << "and GL_UNSIGNED_INT_10F_11F_11F_REV not supported\n";
        myAssert(false);
        break;
      case GL_UNSIGNED_INT_10F_11F_11F_REV:
        // myAssert(dimCount == 3);
        cerr << "printing of types GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV\n";
        cerr << "and GL_UNSIGNED_INT_10F_11F_11F_REV not supported\n";
        myAssert(false);
        break;
      default:
        myAssert(false);
        break;
    }
}


std::ostream &operator<<(std::ostream &os, const CPUVertexArray &va)
{
    using va_size_t = std::vector<GLbyte>::size_type;
    using attrs_count_t = VertexBufferLayout::count_type;

    GLsizei stride = va.layout.getStride();
    va_size_t count = va.data.size() / stride;
    for (va_size_t i_v = 0; i_v < count; ++i_v) {
        os << "vertex " << i_v << ":\n{";
        const auto& attrs = va.layout.getAttributes();
        if (!attrs.empty()) {
            printAttribute(os, attrs[0].dimCount, attrs[0].componentType, &(va.data[i_v * stride + attrs[0].offset]));
        }
        for (attrs_count_t i_a = 1; i_a < attrs.size(); ++i_a) {
            os << ", ";
            printAttribute(os, attrs[i_a].dimCount, attrs[i_a].componentType, &(va.data[i_v * stride + attrs[i_a].offset]));
        }
        os << "}\n";
    }
    return os;
}

bool areBytesEqual(const GLbyte *a, const GLbyte *b, unsigned int size)
{
    unsigned int i;
    for (i = 0; i < size && a[i] == b[i]; ++i) {}
    return i == size;
}
