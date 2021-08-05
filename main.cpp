#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

ifstream inFile;
ofstream output;

int lineNum = 0;
// TODO: - Figure out a way to detect invalid lines such as when there has been a line break in the text column.

void tokenize(string const &str, string &delim,
              vector<string> &out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

void handleCommaText(string &line, vector<string> &store)
{
    string commaLine;

    string delim = ",";
    vector<string> split;
    tokenize(line, delim, split);

    store.push_back(split.front());
    store.push_back(split.at(1));

    // Since lines with commas in them have double quotes, skip to the first occurence of a double quote in this line.
    size_t found = line.find_first_of('\"');

    // Exclude the double quote by skipping forward one character. Grab all characters until the ending quote AKA everything between the quotes.
    for (int i = found + 1; i < line.find_last_of('\"'); i++)
    {
        commaLine += line[i];
    }

    // Add everything extracted between the quotes to the CSV vector.
    store.push_back(commaLine);
}

vector<string> getNextLineAndSplitIntoTokens(istream &str)
{
    vector<string> result;
    string line;
    getline(str, line);

    stringstream lineStream(line);
    string cell;

    if (line.find('\"') == string::npos)
    {
        while (getline(lineStream, cell, ','))
        {
            result.push_back(cell);
        }
    }
    else
    {
        // This text contains double quotes, meaning there must be commas.
        handleCommaText(line, result);
    }

    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        result.push_back("");
    }
    return result;
}

void writeTimestamp(string &timestamp)
{
    string timeDelim = "–";
    vector<string> timePieces;
    tokenize(timestamp, timeDelim, timePieces);

    string start = timePieces.at(0);
    start += ".000";

    string end = timePieces.at(1);
    end += ".000";

    string timeToAdd;
    timeToAdd += start + " ";
    timeToAdd += "--> ";
    timeToAdd += end + "\n";

    output << timeToAdd;
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
        
        string outputName = setOutputName(fileName);
        outputName += ".vtt";
        output.open(outputName);

        cout << "Writing " << fileName << " to " << outputName + "..." << endl;

        if (inFile.is_open())
        {
            output << "WEBVTT\n\n";

            // Skip the first line containing the column names in the CSV.
            getNextLineAndSplitIntoTokens(inFile);

            while (!inFile.eof())
            {
                lineNum++;
                vector<string> row = getNextLineAndSplitIntoTokens(inFile);

                if (row.size() == 3)
                {
                    string timestamp = row.at(0);
                    string speaker = row.at(1);
                    string text = row.at(2);

                    writeTimestamp(timestamp);

                    output << "<v " << speaker << ">";

                    output << text;

                    output << "</v>";
                    output << "\n\n";
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
