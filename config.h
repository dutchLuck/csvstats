/*
 * C O N F I G . H
 *
 * Last Modified on Mon Mar 10 15:08:59 2025
 *
 */

#include <stdio.h>

#ifndef  CONFIG_H
#define  CONFIG_H

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!(FALSE))
#endif

struct positionParam {
  char *  paramNameStr;
  char *  helpStr;
};

struct optFlg {
  int active;
  char *  optID;
  char *  helpStr;
};

struct optChr {
  int active;
  char *  optID;
  char *  helpStr;
  int  optionChr;
};

struct optStr {
  int active;
  char *  optID;
  char *  helpStr;
  char *  optionStr;
};

struct optInt {
  int active;
  char *  optID;
  char *  helpStr;
  int defaultVal;
  int mostPosLimit;
  int mostNegLimit;
  int optionInt;
};

struct optLng {
  int active;
  char *  optID;
  char *  helpStr;
  long defaultVal;
  long mostPosLimit;
  long mostNegLimit;
  long optionLng;
};

struct optDbl {
  int active;
  char *  optID;
  char *  helpStr;
  double defaultVal;
  double mostPosLimit;
  double mostNegLimit;
  double optionDbl;
};

// Command Line Options Configuration Data
struct config {
  struct positionParam posParam1;  /* (posParam1) Optional Name(s) of File(s) to process */
  struct optFlg A;  /* (average) ...... disable Arithmetic Mean (i.e. average ) output. */
  struct optFlg D;  /* (debug) ...... enable debug output mode */
  struct optFlg H;  /* (header) ...... enable Header output mode */
  struct optFlg h;  /* (help) ...... this help / usage information */
  struct optFlg M;  /* (median) ...... disable Median value output */
  struct optFlg N;  /* (negetive) ...... disable most Negetive value output */
  struct optFlg n;  /* (newline) ...... disable the linefeed terminator on the output */
  struct optStr o;  /* (output_file) TXT .. send the output to a file named 'TXT' */
  struct optFlg P;  /* (positive) ...... disable most Positive value output */
  struct optFlg p;  /* (pearson) ...... disable Pearson's skew value output */
  struct optFlg R;  /* (range) ...... disable Range (i.e. most Pos - most Neg) value output */
  struct optFlg r;  /* (row) ...... enable output in Row format */
  struct optFlg S;  /* (stddev) ...... disable Standard Deviation value output */
  struct optFlg v;  /* (verbose) ...... enable more verbose information output */
  struct optFlg V;  /* (version) ...... enable version information output */
};

// getopt() option string
#define OPTIONS ":ADHhMNno:PpRrSvV"

void  usage ( struct config *  optStructPtr, char *  exeName );
void  initConfiguration ( struct config *  optStructPtr );
int  setConfiguration ( int  argc, char *  argv[], struct config *  optStructPtr );
void  configuration_status( struct config *  opt );

#endif
