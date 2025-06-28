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
  struct optChr C;  /* (column_separator) CHR .. use CHR, not comma as the column separator */
  struct optChr c;  /* (comment_delimiter) CHR .. use CHR, not hash as the comment delimiter */
  struct optFlg D;  /* (debug) ...... enable debug output mode */
  struct optInt d;  /* (decimal_places) INT .. provide INT decimal places on output stats - where 0 <= INT <= 30 */
  struct optFlg H;  /* (header) ...... enable Header output mode */
  struct optFlg h;  /* (help) ...... this help / usage information */
  struct optFlg M;  /* (median) ...... disable Median value output */
  struct optFlg N;  /* (negetive) ...... disable most Negetive value output */
  struct optFlg n;  /* (newline) ...... enable extra blank output lines */
  struct optStr o;  /* (output_file) TXT .. send the output to a file named 'TXT' */
  struct optFlg P;  /* (positive) ...... disable most Positive value output */
  struct optFlg p;  /* (pearson) ...... disable Pearson's skew value output */
  struct optFlg R;  /* (range) ...... disable Range (i.e. most Pos - most Neg) value output */
  struct optFlg r;  /* (row) ...... enable output in Row format */
  struct optFlg S;  /* (stddev) ...... disable Standard Deviation value output */
  struct optInt s;  /* (skip) INT .. skip INT lines at the start of data sources - where 0 <= INT <= 1000 */
  struct optFlg v;  /* (verbose) ...... enable more verbose information output */
  struct optFlg V;  /* (version) ...... enable version information output */
};

// getopt() option string
#define OPTIONS ":AC:c:Dd:HhMNno:PpRrSs:vV"

void  usage ( struct config *  optStructPtr, char *  exeName );
void  initConfiguration ( struct config *  optStructPtr );
int  setConfiguration ( int  argc, char *  argv[], struct config *  optStructPtr );
void  configuration_status( struct config *  opt );

#endif
