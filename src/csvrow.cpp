#include "csvrow.h"

/**
 * @brief 
 * 
 * @param str 
 * @param delim 
 * @return vector<string> 
 */
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

/**
 * @brief 
 * 
 * @param line 
 * @param store 
 */
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

/**
 * @brief Extracts all values from the given CSV. Splits each row by commas that separate the columns.
 * 
 * @param line The given row of the CSV
 * @return vector<string> A vector of the row split by commas. This should include the time stamp, speaker, and text.
 */
vector<string> CSVRow::getNextLineAndSplitIntoTokens(string &line)
{
    vector<string> result;

    /* 
    Only do things with this line if it has at least three elements and if it starts with a timestamp. 
    This should skip invalid lines.
    */
    stringstream lineStream(line);
    string cell;

    // Check that there are no double quotes in this row. If there is not one, handle this row normally.
    if (line.find("\"") == string::npos)
    {
        while (getline(lineStream, cell, ','))
        {
            /*
            Handle trailing commas with no data after them by checking if a split string only contains white spaces.
            This will make it so that only parts after a comma that actually contain text will be written to the VTT. 
            */
            bool whiteSpacesOnly = all_of(cell.begin(), cell.end(), ::isspace);
            if (!whiteSpacesOnly)
                result.push_back(cell);
        }
    }
    else
    {
        // This text contains double quotes, meaning there must be commas in the caption text.
        handleCommaText(line, result);
    }

    return result;
}

/**
 * @brief Forms a time stamp from the timestamp column in the given CSV.
 * 
 * @param timestamp The timestamp column of the CSV
 * @return string The timestamps having the proper character added between them as well as .000 to denote 0 milliseconds.
 */
string CSVRow::makeTimestamp(string &timestamp)
{
    string result;

    // Search for hyphen that divides start and end time stamps.
    string timeDelim = "-–—";
    vector<string> timePieces = tokenize(timestamp, timeDelim);

    // Only execute if there are exactly two pieces, meaning there is a start and end time stamp.
    if (timePieces.size() == 2)
    {
        // Add to start and end times to denote 0 milliseconds.
        string start = timePieces.at(0);
        start += ".000";

        string end = timePieces.at(1);
        end += ".000";

        // Replace hyphen with standard VTT time stamp separator and add a newline character to the end.
        result += start + " ";
        result += "--> ";
        result += end + "\n";
    }

    return result;
}

/**
 * @brief Checks if the CSV row is missing a timestamp, speaker, or text.
 * 
 * @param row The current row of the CSV
 * @param errors The vector of errors to store any errors that come up in
 * @return true This row is missing some needed information.
 * @return false This row is not missing a timestamp, speaker, or text.
 */
bool CSVRow::missingInfo(vector<string> &row, vector<VTTError *> &errors)
{
    // Do something with this line only if it has at least three elements which should correspond to timestamp, speaker, and text.
    if (row.size() < 3)
    {
        errors.push_back(new VTTError(MISSINGINFO, lineNum));
        return true;
    }

    string timePieces = row.front();

    string hourMinSecDelim = ":";
    row = tokenize(timePieces, hourMinSecDelim);

    // Check for hh:mm:ss.
    if (row.size() < 3)
    {
        errors.push_back(new VTTError(MISSINGINFO, lineNum));
        return true;
    }

    // Check that this row starts with a time stamp.
    if (row.front().find_first_of("0123456789") == string::npos)
    {
        errors.push_back(new VTTError(MISSINGINFO, lineNum));
        return true;
    }
    return false;
}

bool CSVRow::maxMinSec(int minutes, int seconds)
{
    if (minutes >= 60)
    {
        errors.push_back(new VTTError(MAXMINUTES, lineNum));
        return true;
    }

    if (seconds >= 60)
    {
        errors.push_back(new VTTError(MAXSECONDS, lineNum));
        return true;
    }

    return false;
}

/**
 * @brief Construct a new CSVRow::CSVRow object
 * Check for any errors in the row. If there are none, write the row to the VTT.
 * 
 * @param inLine 
 * @param lineNum 
 */
CSVRow::CSVRow(string &inLine, int lineNum)
{
    this->lineNum = lineNum;

    if (!inLine.empty())
    {
        string commaDelim = ",";
        vector<string> row = tokenize(inLine, commaDelim);

        if (!missingInfo(row, errors))
        {
            int minutes = stoi(row.at(1));
            int seconds = stoi(row.at(2));

            string separator = "-–—";

            if (inLine.find_first_of(separator) == string::npos)
                errors.push_back(new VTTError(NOTIMESEPARATOR, lineNum));

            if (!maxMinSec(minutes, seconds))
            {
                row = getNextLineAndSplitIntoTokens(inLine);

                timeStamp = makeTimestamp(row.front());
                speaker = row.at(1);
                text = row.back();
            }
        }
    }
    else
    {
        errors.push_back(new VTTError(EMPTYLINE, lineNum));
    }
}

/**
 * @brief Destroy the CSVRow::CSVRow object. 
 * Deallocate space for all VTTError pointers in errors vector to prevent memory leaks.
 * 
 */
CSVRow::~CSVRow()
{
    for (VTTError *err : errors)
    {
        delete err;
    }
    errors.clear();
}