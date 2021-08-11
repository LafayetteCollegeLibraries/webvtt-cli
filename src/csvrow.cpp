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

vector<string> CSVRow::getNextLineAndSplitIntoTokens(string &line)
{
    vector<string> result;

    // Only do things with this line if it has at least three elements and if it starts with a timestamp. This should skip invalid lines.
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

CSVRow::CSVRow(string &inLine, int lineNum)
{
    error = NULL;
    this->lineNum = lineNum;

    string commaDelim = ",";
    vector<string> row = tokenize(inLine, commaDelim);

    // Do something with this line only if it has at least three elements which should correspond to timestamp, speaker, and text.
    if (row.size() >= 3)
    {
        string timePieces = row.front();

        string hourMinSecDelim = ":";
        row = tokenize(timePieces, hourMinSecDelim);

        // Checks for hh:mm:ss
        if (row.size() >= 3)
        {
            // Check that the timestamp column contains a number.
            if (row.front().find_first_of("0123456789") != string::npos)
            {
                // Check that minutes and seconds are not above 60.
                int minutes = stoi(row.at(1));
                int seconds = stoi(row.at(2));

                string separator = "-–—";
                if (inLine.find_first_of(separator) != string::npos)
                {
                    if (minutes >= 60)
                    {
                        // Minutes are too large.
                        error = new VTTError(MAXMINUTES, lineNum);
                    }
                    else if (seconds >= 60)
                    {
                        // Seconds are too large.
                        error = new VTTError(MAXSECONDS, lineNum);
                    }
                    else
                    {
                    }
                }
                else
                {
                    error = new VTTError(NOTIMESEPARATOR, lineNum);
                }
            }
            else
            {
                // This is meant to handle lines that can be split in three, but still do not have the information needed.
                error = new VTTError(MISSINGINFO, lineNum);
            }
        }
        else
        {
            // Throw an error because no separating colons could be found.
            error = new VTTError(NOTIMESEPARATOR, lineNum);
        }
    }
    else
    {
        // The timestamp, speaker, and text are missing. Could be one of them, could be all three of them!
        error = new VTTError(MISSINGINFO, lineNum);
    }
}

CSVRow::~CSVRow()
{
    delete error;
}