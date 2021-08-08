#ifndef VTTERROR_H
#define VTTERROR_H

#include "errcode.h"

class VTTError
{
private:
    ErrCode errCode;
    int lineNum;
public:
    VTTError(ErrCode code, int errLine);
    ~VTTError();

    ErrCode getCode() { return errCode; }
    int getLineNum() { return lineNum; }
};

#endif