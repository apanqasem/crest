#include<stdio.h>
#include<stdlib.h>

#include<errors.h>


void error(const char * msg, errorType code) {
  switch(code) {
  case FATAL:
    fprintf(stderr, "Fatal : %s\n", msg);
    exit(0);
    break;
  case NONFATAL: 
    fprintf(stderr, "%s\n", msg);
    break;
  case PROGRAMMER:
    fprintf(stderr, "PROGRAMMER: %s\n", msg);
    exit(0);
  default:
    fprintf(stderr, "Fatal : %s\n", msg);
    exit(0);
  }
}
