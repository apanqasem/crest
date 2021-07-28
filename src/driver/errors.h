#ifndef ERRORS_H
#define ERRORS_H

typedef enum ERROR_TYPE {FATAL, PROGRAMMER, NONFATAL, MEMORY} errorType;

void error(const char * msg, errorType code);

#endif 
