#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

ifstream inFile;
ofstream output("output.vtt");

int lineNum = 0;
// TODO: - Figure out a way to detect invalid lines such as when there has been a line break in the text column.

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
{
    std::vector<std::string> result;
    std::string line;
    std::getline(str,line);

    std::stringstream lineStream(line);
    std::string cell;

    while(std::getline(lineStream,cell, ','))
    {
        result.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        result.push_back("");
    }
    return result;
}

void tokenize(std::string const &str, string &delim,
            std::vector<std::string> &out)
{
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

void writeTimestamp(string &timestamp) {
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

int main (int argc, char *argv[]) {
    vector<int> invalidLines;

    if (argc < 2) {
        cout << "No input files provided." << endl;
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        cout << "Converting " << argv[i] << " to a VTT..." << endl;
        inFile.open(argv[i]);

        if (inFile.is_open()) {
            output << "WEBVTT\n\n";

            // Skip the first line containing the column names in the CSV.
            getNextLineAndSplitIntoTokens(inFile);

            while (!inFile.eof()) {
                lineNum++;
                vector<string> row = getNextLineAndSplitIntoTokens(inFile);

                string timestamp = row.at(0);
                string speaker = row.at(1);
                string text = row.at(2);

                writeTimestamp(timestamp);

                output << "<v " << speaker << ">";

                // Get rid of any double quotes in the text.
                text.erase(remove(text.begin(), text.end(), '\"'), text.end());

                output << text;

                output << "</v>";
                output << "\n\n";
            }

            cout << "Done writing VTT." << endl;
            inFile.close();
            output.close();

            if (!invalidLines.empty()) {
                for (int l: invalidLines) {
                    cout << "Invalid line " << l << endl;
                }
            }
        } else {
            cout << "Unable to open file." << endl;
            return EXIT_FAILURE;
        }
    }

    return 0;
}


