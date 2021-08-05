#ifndef CSVROW_H
#define CSVROW_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

class CSVRow
{
private:
    string *timeStamp;
    string *speaker;
    string *text;

    string makeTimestamp(string &timestamp);
    void handleCommaText(string &line, vector<string> &store);
    vector<string> getNextLineAndSplitIntoTokens(istream &str);
    void tokenize(string const &str, string &delim, vector<string> &out);
public:
    CSVRow(istream &in);
    ~CSVRow();

    string *getTimeStamp() { return timeStamp; }
    string *getSpeaker() { return speaker; }
    string *getText() { return text; }
};

#endif