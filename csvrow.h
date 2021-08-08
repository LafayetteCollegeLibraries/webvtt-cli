#ifndef CSVROW_H
#define CSVROW_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "vtterror.h"
#include "errcode.h"

using namespace std;

class CSVRow
{
private:
    string timeStamp;
    string speaker;
    string text;
    
    VTTError *error;
    int lineNum;

    string makeTimestamp(string &timestamp);
    void handleCommaText(string &line, vector<string> &store);
    vector<string> getNextLineAndSplitIntoTokens(istream &str);
    vector<string> tokenize(string const &str, string &delim);
public:
    CSVRow(istream &in, int line);
    ~CSVRow();

    string getTimeStamp() { return timeStamp; }
    string getSpeaker() { return speaker; }
    string getText() { return text; }

    VTTError *getError() { return error; }
};

#endif