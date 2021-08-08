#include "csvrow.h"

vector<string> CSVRow::tokenize(string const &str, string &delim)
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

void CSVRow::handleCommaText(string &line, vector<string> &store)
{
    string commaLine;

    string delim = ",";
    vector<string> split = tokenize(line, delim);

    if (split.size() >= 2)
    {
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
        if (!commaLine.empty())
            store.push_back(commaLine);
    }
}

vector<string> CSVRow::getNextLineAndSplitIntoTokens(istream &str)
{
    vector<string> result;
    string line;
    getline(str, line);

    string delim = ",";
    vector<string> split = tokenize(line, delim);

    // Only do things with this line if it has at least three elements. This should skip invalid lines.
    if (split.size() >= 3)
    {
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
    }
    else
    {
        error = new VTTError(MISSINGINFO, lineNum);
    }

    return result;
}

string CSVRow::makeTimestamp(string &timestamp)
{
    string result;

    string timeDelim = "-–—";
    vector<string> timePieces = tokenize(timestamp, timeDelim);

    if (timePieces.size() == 2)
    {
        string start = timePieces.at(0);
        start += ".000";

        string end = timePieces.at(1);
        end += ".000";

        result += start + " ";
        result += "--> ";
        result += end + "\n";
    }

    return result;
}

CSVRow::CSVRow(istream &in, int line)
{
    error = NULL;
    lineNum = line;

    vector<string> row = getNextLineAndSplitIntoTokens(in);

    if (row.size() == 3)
    {
        timeStamp = makeTimestamp(row.front());
        speaker = row.at(1);
        text = row.back();
    }
}

CSVRow::~CSVRow()
{
    delete error;
}