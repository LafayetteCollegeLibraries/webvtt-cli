#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

int main (int argc, char *argv[]) {
    ifstream inFile;
    string line;

    if (argc < 2) {
        cout << "No input files provided." << endl;
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        cout << "Attempting to open file " << argv[i] << endl;
        inFile.open(argv[i]);

        if (inFile.is_open()) {
            while (getline(inFile, line)) {
                cout << line << endl;
            }
        } else {
            cout << "Unable to open file." << endl;
            return EXIT_FAILURE;
        }
    }

    return 0;
}