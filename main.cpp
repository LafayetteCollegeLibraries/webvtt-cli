#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "csvrow.h"

using namespace std;

ifstream inFile;
ofstream output;

int lineNum = 0;
// TODO: - Figure out a way to detect invalid lines such as when there has been a line break in the text column.

string setOutputName(string &arg)
{
    string outputName;
    size_t found = arg.find_last_of('/');
    for (int i = found + 1; i < arg.find_last_of("."); i++)
    {
        outputName += arg[i];
    }

    return outputName;
}

int main(int argc, char *argv[])
{
    vector<int> invalidLines;

    if (argc < 2)
    {
        cout << "No input files provided." << endl;
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++)
    {
        string fileName = argv[i];
        inFile.open(fileName);
        
        // Set the file name of the VTT to be that of the given CSV.
        string outputName = setOutputName(fileName);

        if (!outputName.empty()) {
            outputName += ".vtt";
            output.open(outputName);
        } else {
            cout << "Could not determine VTT file name." << endl;
        }

        cout << "Writing " << fileName << " to " << outputName + "..." << endl;

        if (inFile.is_open())
        {
            output << "WEBVTT\n\n";

            // Skip the first line containing the column names in the CSV.
            getline(inFile, outputName);

            while (!inFile.eof())
            {
                lineNum++;

                CSVRow row(inFile);
                if (row.getTimeStamp() != NULL) {
                    output << *row.getTimeStamp();
                }
                if (row.getSpeaker() != NULL && row.getText() != NULL) {
                    output << "<v " << *row.getSpeaker() << ">" << *row.getText() << "</v>" << "\n\n";
                }
            }

            cout << "Done writing VTT." << endl;
            inFile.close();
            output.close();

            if (!invalidLines.empty())
            {
                for (int l : invalidLines)
                {
                    cout << "Invalid line " << l << endl;
                }
            }
        }
        else
        {
            cout << "Error opening file: " << strerror(errno) << endl;
            return EXIT_FAILURE;
        }
    }

    return 0;
}
