#ifndef VTTERROR_H
#define VTTERROR_H

#include "errcode.h"

class VTTError
{
private:
    ErrCode errCode;
public:
    VTTError(ErrCode code);
    ~VTTError();
};

#endif