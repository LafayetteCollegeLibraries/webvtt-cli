#include <gtest/gtest.h>

#include "../include/csvrow.h"

TEST(TestError, MissingInfo) {
    string errLine = "00:00:00–00:01:00,Neil deGrasse Tyson";
    CSVRow *row = new CSVRow(errLine, 1);
    vector<VTTError *> errors = row->getErrors();

    auto is_missinginfo = [](VTTError *err){ return err->getCode() == MISSINGINFO; };
    auto result = find_if(errors.begin(), errors.end(), is_missinginfo);

    ASSERT_NE(result, errors.end());
}

TEST(TestError, EmptyLine) {
    string errLine = "";
    CSVRow *row = new CSVRow(errLine, 9);

    vector<VTTError *> errors = row->getErrors();

    auto is_emptyline = [](VTTError *err){ return err->getCode() == EMPTYLINE; };
    auto result = find_if(errors.begin(), errors.end(), is_emptyline);

    ASSERT_NE(result, errors.end());
}

TEST(TestError, MaxSeconds) {
    string errLine = "00:07:63–00:08:01,CN,I'm going to go ahead and turn it over to Nora now for a repository tour. I'll stop sharing.";
    CSVRow *row = new CSVRow(errLine, 5);

    vector<VTTError *> errors = row->getErrors();

    auto is_maxseconds = [](VTTError *err){ return err->getCode() == MAXSECONDS; };
    auto result = find_if(errors.begin(), errors.end(), is_maxseconds);

    ASSERT_NE(result, errors.end());
}

TEST(TestError, MaxMinutes) {
    string errLine = "00:62:04–00:08:07,NE,Great. Thank you so much Charlotte.";
    CSVRow *row = new CSVRow(errLine, 6);

    vector<VTTError *> errors = row->getErrors();

    auto is_maxminutes = [](VTTError *err){ return err->getCode() == MAXMINUTES; };
    auto result = find_if(errors.begin(), errors.end(), is_maxminutes);

    ASSERT_NE(result, errors.end());
}

TEST(TestError, NoSeparator) {
    string errLine = "00:07:4500:07:48,CN,develop this entirely new repository infrastructure.";
    CSVRow *row = new CSVRow(errLine, 3);

    vector<VTTError *> errors = row->getErrors();

    auto is_noseparator = [](VTTError *err){ return err->getCode() == NOTIMESEPARATOR; };
    auto result = find_if(errors.begin(), errors.end(), is_noseparator);

    ASSERT_NE(result, errors.end());
}