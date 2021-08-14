#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "csvrow.h"
#include "vtterror.h"
#include "errcode.h"

using namespace std;

vector<string> tokenize(string const &str, string &delim)
{
    vector<string> out;

    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != string::npos)
    {
        end = str.find_first_of(delim, start);
        out.push_back(str.substr(start, end - start));
    }

    return out;
}

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

CSVRow *rowFromLine(string &inLine, vector<VTTError> &invalidLines, int lineNum) {
    CSVRow *row = new CSVRow(inLine, lineNum);

    vector<VTTError *> errors = row->getErrors();
    if (!errors.empty())
    {
        for (VTTError *err : errors)
        {
            invalidLines.push_back(*err);
        }
    }

    return row;
}

void writeRowToFile(CSVRow *row, ofstream &output) {
    string timeStamp = row->getTimeStamp();
    string speaker = row->getSpeaker();
    string text = row->getText();

    output << timeStamp;
    output << "<v " << speaker << ">" << text << "</v>"
           << "\n\n";
}

void processErrors(vector<VTTError> &invalidLines) {
    for (VTTError err : invalidLines)
    {
        switch (err.getCode())
        {
        case MISSINGINFO:
            cout << "Line " << err.getLineNum() << ": missing timestamp, speaker, and text." << endl;
            break;
        case NOTIMESEPARATOR:
            cout << "Line " << err.getLineNum() << ": missing timestamp separator." << endl;
            break;
        case MAXMINUTES:
            cout << "Line " << err.getLineNum() << ": minutes are greater than 60." << endl;
            break;
        case MAXSECONDS:
            cout << "Line " << err.getLineNum() << ": seconds are greater than 60." << endl;
            break;
        case EMPTYLINE:
            cout << "Line " << err.getLineNum() << " is empty." << endl;
            break;
        case INVALIDTIMESTART:
            cout << "Line " << err.getLineNum() << ": start timestamp must be greater than or equal to previous timestamp." << endl;
            break;
        case INVALIDTIMEEND:
            cout << "Line " << err.getLineNum() << ": end timestamp must be greater than start." << endl;
            break;

        default:
            break;
        }
    }
}

int normalizeTimeStamp(vector<string> &pieces) {
    if (pieces.size() == 3) {
        int hoursAsSecs = stoi(pieces.front()) * 60 * 60;
        int minsAsSecs = stoi(pieces.at(1)) * 60;
        int secs = stoi(pieces.back());

        return hoursAsSecs + minsAsSecs + secs;
    }
    return NULL;
}

bool startValid(CSVRow *row, vector<CSVRow *> rows) {
    if (rows.size() > 1)
    {
        // Compare this row's timestamp with the previous to check if it is less than the previous.

        CSVRow *prev = rows.back();
        string timeStampSeparator = "--> ";

        vector<string> prevTimeStamps = tokenize(prev->getTimeStamp(), timeStampSeparator);
        vector<string> currTimeStamps = tokenize(row->getTimeStamp(), timeStampSeparator);
        string pieceSeparator = ":";

        map<string, vector<string>> pieces { {"start1", tokenize(prevTimeStamps.front(), pieceSeparator)}, {"start2", tokenize(currTimeStamps.front(), pieceSeparator)}};

        int normStart1 = normalizeTimeStamp(pieces["start1"]);
        int normStart2 = normalizeTimeStamp(pieces["start2"]);

        return normStart2 >= normStart1;
    }
    return true;
}

bool endValid(CSVRow *row) {
    string timeStampSeparator = "--> ";
    string pieceSeparator = ":";

    vector<string> timeStamps = tokenize(row->getTimeStamp(), timeStampSeparator);

    vector<string> startPieces = tokenize(timeStamps.front(), pieceSeparator);
    vector<string> endPieces = tokenize(timeStamps.back(), pieceSeparator);

    int normStart = normalizeTimeStamp(startPieces);
    int normEnd = normalizeTimeStamp(endPieces);

    return normEnd > normStart;
}

int main(int argc, char *argv[])
{
    vector <CSVRow *> rows;
    vector<VTTError> invalidLines;
    ifstream inFile;
    ofstream output;

    int lineNum = 1;

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

        if (!outputName.empty())
        {
            outputName += ".vtt";
            output.open(outputName);
        }
        else
        {
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

                string line;
                getline(inFile, line);

                CSVRow *row = rowFromLine(line, invalidLines, lineNum);
                if (row->getErrors().empty()) {
                    if (!startValid(row, rows))
                        invalidLines.push_back(VTTError(INVALIDTIMESTART, lineNum));

                    if (!endValid(row))
                        invalidLines.push_back(VTTError(INVALIDTIMEEND, lineNum));

                    rows.push_back(row);
                    writeRowToFile(row, output);
                }
            }

            cout << "Done writing VTT." << endl;
            inFile.close();
            output.close();

            if (!invalidLines.empty())
            {
                processErrors(invalidLines);
            }
        }
        else
        {
            perror("Error opening file");
            return EXIT_FAILURE;
        }
    }

    return 0;
}
