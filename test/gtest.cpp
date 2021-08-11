#include <gtest/gtest.h>

#include "../include/csvrow.h"

TEST(TestError, MissingInfo) {
    string errLine = "";
    CSVRow *row = new CSVRow(errLine, 1);

    ASSERT_EQ(row->getError()->getCode(), MISSINGINFO);
}

TEST(TestError, MaxSeconds) {
    string errLine = "00:07:63–00:08:01,CN,I'm going to go ahead and turn it over to Nora now for a repository tour. I'll stop sharing.";
    CSVRow *row = new CSVRow(errLine, 5);

    ASSERT_EQ(row->getError()->getCode(), MAXSECONDS);
}

TEST(TestError, MaxMinutes) {
    string errLine = "00:62:04–00:08:07,NE,Great. Thank you so much Charlotte.";
    CSVRow *row = new CSVRow(errLine, 6);

    ASSERT_EQ(row->getError()->getCode(), MAXMINUTES);
}

TEST(TestError, NoSeparator) {
    string errLine = "00:07:4500:07:48,CN,develop this entirely new repository infrastructure.";
    CSVRow *row = new CSVRow(errLine, 3);

    ASSERT_EQ(row->getError()->getCode(), NOTIMESEPARATOR);
}