#include "vtterror.h"

VTTError::VTTError(ErrCode code, int errLine)
{
    errCode = code;
    lineNum = errLine;
}

VTTError::~VTTError() {}