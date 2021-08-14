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
    
    vector<VTTError *> errors;
    int lineNum;

    string makeTimestamp(string &timestamp);
    void handleCommaText(string &line, vector<string> &store);
    vector<string> getNextLineAndSplitIntoTokens(string &line);
    vector<string> tokenize(string const &str, string &delim);
public:
    CSVRow(string &line, int lineNum);
    ~CSVRow();

    string getTimeStamp() { return timeStamp; }
    string getSpeaker() { return speaker; }
    string getText() { return text; }
    int getLineNum() { return lineNum; }

    vector<VTTError *> getErrors() { return errors; }
};

#endif