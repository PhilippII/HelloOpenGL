#include "import_mesh.h"

#include <fstream>
#include <sstream>
#include <regex>

#include "debug_utils.h"


using std::string, std::vector;
using std::cout, std::cerr;
using std::ifstream, std::istringstream, std::ostream, std::getline;


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


template <typename Index>
static CPUIndexBuffer<Index> applyTriangleFan(const CPUIndexBuffer<Index>& ib) {
    using ib_count_t = typename std::vector<Index>::size_type;
    CPUIndexBuffer<Index> res = {std::vector<Index>{},
                          GL_TRIANGLES,
                          false,
                          std::numeric_limits<Index>::max()};
    myAssert(ib.primitiveType == GL_TRIANGLE_FAN);
    myAssert(ib.primitiveRestart);

    ib_count_t i = 0;
    while (i < ib.indices.size()) {
        if (i + 2 >= ib.indices.size()) {
            cerr << "warning polygon with less than 3 vertices detected.\n";
            continue;
        }
        Index baseVert = ib.indices[i];
        i += 2;
        while (i < ib.indices.size() && ib.indices[i] != ib.primitiveRestartIndex) {
            res.indices.push_back(baseVert);
            res.indices.push_back(ib.indices[i-1]);
            res.indices.push_back(ib.indices[i]);
            ++i;
        }
        if (i < ib.indices.size()) { // restart index encountered
            ++i;
        }
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

std::vector<CPUMesh<GLuint>> readOBJ(std::string filepath, bool invert_z)
{
    ifstream ifs {filepath};
    if (!ifs) {
        cerr << "error opening file " << filepath << '\n';
        return std::vector<CPUMesh<GLuint>>();
    }

    vector<std::regex> mi_pats = {  // indices should align to WavefrontObject::MultiIndexFormat
        std::regex{R"((\d+))"}, // V
        std::regex{R"((\d+)/(\d+))"}, // V_VT
        std::regex{R"((\d+)//(\d+))"}, // V_VN
        std::regex{R"((\d+)/(\d+)/(\d+))"} // V_VT_VN
    };

    vector<CPUMesh<GLuint>> results;

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
                    return vector<CPUMesh<GLuint>>();
                }
                if (invert_z) {
                    v[2] = -v[2];
                }
                obj.verts_v.push_back(v);
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
                    return vector<CPUMesh<GLuint>>();
                }
                obj.verts_vt.push_back(vt);
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
                    return vector<CPUMesh<GLuint>>();
                }
                if (invert_z) {
                    vn[2] = -vn[2];
                }
                obj.verts_vn.push_back(vn);
                float value;
                if (iss >> value) {
                    cerr << "error normal should only have three dimensions:\n";
                    cerr << '\t' << line << '\n';
                    myAssert(false);
                    return vector<CPUMesh<GLuint>>();
                }
            } else if (opcodeStr == "f") {
                string multIndStr;
                int n = 1;
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
                        cerr << "\tin face: " << line << '\n';
                        myAssert(false);
                        return vector<CPUMesh<GLuint>>();
                    }
                    GLuint v, vt, vn;
                    switch (obj.miFormat) {
                    case WavefrontObject::MultiIndexFormat::V:
                        v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                            // regular expression does not allow negative numbers here anyway
                            // so we can cast to unsigned
                        obj.mib_v.indices.push_back({v});
                        break;
                    case WavefrontObject::MultiIndexFormat::V_VT:
                        v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                        vt = static_cast<GLuint>(std::stoi(matches[2].str())) - 1;
                        obj.mib_v_vt.indices.push_back({v, vt});
                        break;
                    case WavefrontObject::MultiIndexFormat::V_VN:
                        v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                        vn = static_cast<GLuint>(std::stoi(matches[2].str())) - 1;
                        obj.mib_v_vn.indices.push_back({v, vn});
                        break;
                    case WavefrontObject::MultiIndexFormat::V_VT_VN:
                        v = static_cast<GLuint>(std::stoi(matches[1].str())) - 1;
                        vt = static_cast<GLuint>(std::stoi(matches[2].str())) - 1;
                        vn = static_cast<GLuint>(std::stoi(matches[3].str())) - 1;
                        obj.mib_v_vt_vn.indices.push_back({v, vt, vn});
                        break;
                    default:
                        myAssert(false);
                        break;
                    }
                    ++n;
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
            } else if (opcodeStr == "o") {
                if (!(iss >> nextName) || nextName.empty()) {
                    cerr << "error parsing name of object\n";
                }
                startNext = true;
            } else {
                cerr << "warning: unknown opcode " << opcodeStr << '\n';
            }
        }

        cout << "finished parsing object " << obj.name
             << " from file " << filepath << '\n';
        // cout << obj;

        VertexBufferLayout layout_v;
        layout_v.append<float>(3);
        VertexBufferLayout layout_vt;
        layout_vt.append<float>(2);
        VertexBufferLayout layout_vn;
        layout_vn.append<float>(3);
        switch (obj.miFormat) {
        case WavefrontObject::MultiIndexFormat::UNKNOWN:
            if (!obj.name.empty()) {
                cout << "error no faces specified\n";
            }
            break;
        case WavefrontObject::MultiIndexFormat::V:
            {
                CPUVertexArray va_v {layout_v,
                                    toByteVector(obj.verts_v)};
                CPUMultiIndexMesh<GLuint, 1> miMesh {obj.mib_v, std::array<CPUVertexArray, 1>{va_v}};
                CPUMesh<GLuint> res = unifyIndexBuffer(miMesh);
                res.ib = applyTriangleFan(res.ib);
                results.push_back(res);
            }
            break;
        case WavefrontObject::MultiIndexFormat::V_VT:
            {
                CPUVertexArray va_v {layout_v,
                                     toByteVector(obj.verts_v)};
                CPUVertexArray va_vt {layout_vt,
                                      toByteVector(obj.verts_vt)};
                CPUMultiIndexMesh<GLuint, 2> miMesh {obj.mib_v_vt, std::array<CPUVertexArray, 2>{va_v, va_vt}};
                CPUMesh<GLuint> res = unifyIndexBuffer(miMesh);
                res.ib = applyTriangleFan(res.ib);
                results.push_back(res);
            }
            break;
        case WavefrontObject::MultiIndexFormat::V_VN:
            {
                CPUVertexArray va_v {layout_v,
                                     toByteVector(obj.verts_v)};
                CPUVertexArray va_vn {layout_vn,
                                      toByteVector(obj.verts_vn)};
                CPUMultiIndexMesh<GLuint, 2> miMesh {obj.mib_v_vn, std::array<CPUVertexArray, 2>{va_v, va_vn}};
                CPUMesh<GLuint> res = unifyIndexBuffer(miMesh);
                res.ib = applyTriangleFan(res.ib);
                results.push_back(res);
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
                CPUMesh<GLuint> res = unifyIndexBuffer(miMesh);
                res.ib = applyTriangleFan(res.ib);
                results.push_back(res);
            }
            break;
        default:
            myAssert(false);
            break;
        }
        // TODO: should res also store obj.name?
    }

    return results;
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
