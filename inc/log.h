#ifndef _LOG_H_
#define _LOG_H_
#include "print.h"

//TODO printf like logging
#define LOG(str) do{log(str);}while(0)

#ifdef _DEBUG_
#define LOG_DEBUG(str) LOG(str)
#else
#define LOG_DEBUG(str)
#endif

#endif
