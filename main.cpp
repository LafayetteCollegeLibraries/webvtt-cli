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

/**
 * @brief Sets the file name of the VTT. Uses name after last slash in path up to .csv to make name.
 * 
 * @param arg 
 * @return File name of VTT. 
 */
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

/**
 * @brief Forms a CSVRow pointer from a given string and line number.
 * 
 * @param inLine The string read from the CSV.
 * @param invalidLines A vector of errors to be added to if there are any problems with the line.
 * @param lineNum The line number in the CSV the input stream is currently on.
 * @return CSVRow* Return a pointer to the CSVRow instance created from this line.
 */
CSVRow *rowFromLine(string &inLine, vector<VTTError> &invalidLines, int lineNum)
{
    CSVRow *row = new CSVRow(inLine, lineNum);

    // Check if there are errors on this row.
    vector<VTTError *> errors = row->getErrors();
    if (!errors.empty())
    {
        // Loop through the errors on this row and add them to the vector.
        for (VTTError *err : errors)
        {
            invalidLines.push_back(*err);
        }
    }

    return row;
}

/**
 * @brief Writes the contents of a CSV row to the VTT.
 * 
 * @param row The row whose contents should be written to the VTT.
 * @param output The output stream that will write to the VTT.
 */
void writeRowToFile(CSVRow *row, ofstream &output)
{
    // Get everything needed for the VTT row.
    string timeStamp = row->getTimeStamp();
    string speaker = row->getSpeaker();
    string text = row->getText();

    // Begin by writing the timestamp since that always appears first in a WebVTT cue.
    output << timeStamp;

    // Wrap speaker and text in needed tags. Write newline chars to separate the cue.
    output << "<v " << speaker << ">" << text << "</v>"
           << "\n\n";
}

void processErrors(vector<VTTError> &invalidLines)
{
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
            cout << "Line " << err.getLineNum() << ": start timestamp must be greater than or equal to start of previous cue." << endl;
            break;
        case INVALIDTIMEEND:
            cout << "Line " << err.getLineNum() << ": end timestamp must be greater than start." << endl;
            break;

        default:
            break;
        }
    }
}
/**
 * @brief Converts all pieces of a timestamp into seconds for comparison.
 * 
 * @param pieces A vector of strings containing all of the components of a timestamp split by a colon.
 * @return int Returns the timestamp in seconds. Done so by adding the hours converted to seconds, minutes converted to seconds, and seconds.
 */
int normalizeTimeStamp(vector<string> &pieces)
{
    // Even though this will only execute if the row doesn't have any errors, I want to be extra safe and make sure hh:mm:ss are there.
    if (pieces.size() == 3)
    {
        // Since the pieces of a timestamp are strings, convert them to ints. After this, do some math to convert to seconds.
        int hoursAsSecs = stoi(pieces.front()) * 60 * 60;
        int minsAsSecs = stoi(pieces.at(1)) * 60;
        int secs = stoi(pieces.back());

        return hoursAsSecs + minsAsSecs + secs;
    }
    return NULL;
}

/**
 * @brief Checks if the start timestamp is valid. A start will be valid only if it is greater than or equal to the start of the previous cue.
 * 
 * @param row The row of the CSV.
 * @param rows The vector of rows used to compare with the previous row.
 * @return true Return true if there are no other rows to compare to
 * @return normStart2 >= normStart1 Return if the start of the current timestamp is greater than or equal to the previous timestamp.
 */
bool startValid(CSVRow *row, vector<CSVRow *> rows)
{
    if (rows.size() > 1)
    {
        // Get previous timestamp.
        CSVRow *prev = rows.back();
        string timeStampSeparator = "--> ";

        // Separate start and end timestamps of previous and current row.
        vector<string> prevTimeStamps = tokenize(prev->getTimeStamp(), timeStampSeparator);
        vector<string> currTimeStamps = tokenize(row->getTimeStamp(), timeStampSeparator);

        // Separate the start timestamps into hh:mm:ss. Store them in a map for reference.
        string pieceSeparator = ":";
        map<string, vector<string>> pieces{{"start1", tokenize(prevTimeStamps.front(), pieceSeparator)}, {"start2", tokenize(currTimeStamps.front(), pieceSeparator)}};

        // Normalize the start timestamps into seconds for easier comparison.
        int normStart1 = normalizeTimeStamp(pieces["start1"]);
        int normStart2 = normalizeTimeStamp(pieces["start2"]);

        return normStart2 >= normStart1;
    }
    return true;
}

/**
 * @brief Checks if an end timestamp is greater than the start timestamp of a row.
 * 
 * @param row The current row of the CSV.
 * @return normEnd > normStart Return if the end timestamp is greater than the start.
 */
bool endValid(CSVRow *row)
{
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
    vector<CSVRow *> rows;
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
                if (row->getErrors().empty())
                {
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

            for (CSVRow *row : rows)
            {
                delete row;
            }
            rows.clear();

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
