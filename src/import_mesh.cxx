#include "import_mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>
// #include <regex>

using std::string;
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
        } else {
            cerr << "warning: unknown opcode " << opcodeStr << '\n';
        }
    }
}
