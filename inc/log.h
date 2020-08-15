#ifndef _LOG_H_
#define _LOG_H_
#include "print.h"

//TODO printf like logging
#define LOG(fmt, ...) do{log("INFO: "fmt"\n", ##__VA_ARGS__);}while(0)
#define LOG_WRN(fmt, ...) do{log("WRN: "fmt"\n", ##__VA_ARGS__);}while(0)
#define LOG_ERR(fmt, ...) do{log("ERR: "fmt"\n", ##__VA_ARGS__);}while(0)

#ifdef _DEBUG_
#define LOG_DBG(fmt, ...) do{log("DBG: "fmt"\n", ##__VA_ARGS__);}while(0)
#else
#define LOG_DBG(fmt, ...)
#endif

#endif
