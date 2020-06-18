#include <string>
#include <stdio.h>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds)
{
    char cstring[10];
    int nH = seconds / 3600;
    int rest = seconds % 3600;
    int nM = rest / 60;
    int nS = rest % 60;
    sprintf(cstring, "%.2d:%.2d:%.2d", nH, nM, nS);
    return cstring;
}