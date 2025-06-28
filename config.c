/*
 * C O N F I G . C
 *
 * Last Modified on Mon Jun 23 19:14:05 2025
 *
 * 2025-Jun-22 Added Chr option handling
 */

#include <stdio.h>    /* printf() */
#include <stdlib.h>   /* floating point double, strtol */
#include <unistd.h>   /* getopt() */
#include <ctype.h>    /* isprint()*/
#include <limits.h>   /* INT_MIN INT_MAX LONG_MIN LONG_MAX */
#include <float.h>    /* DBL_MIN DBL_MAX */
#include <string.h>   /* strchr() */
#include "config.h"   /* struct config */
#include "genFun.h"   /* convertOptionStringToInteger() limitIntegerValueToEqualOrWithinRange() */


int  configureIntegerOption( struct optInt *  intStructPtr, char *  intString ) {
  intStructPtr->active = TRUE;
  intStructPtr->optionInt = convertOptionStringToInteger( intStructPtr->defaultVal, intString,
    intStructPtr->optID, &intStructPtr->active, TRUE );
  intStructPtr->optionInt = limitIntegerValueToEqualOrWithinRange( intStructPtr->optionInt,
    intStructPtr->mostNegLimit, intStructPtr->mostPosLimit );
  return( intStructPtr->active );
}


int  configureLongOption( struct optLng *  longStructPtr, char *  longString ) {
  longStructPtr->active = TRUE;
  longStructPtr->optionLng = convertOptionStringToLong( longStructPtr->defaultVal, longString,
    longStructPtr->optID, &longStructPtr->active, TRUE );
  longStructPtr->optionLng = limitLongValueToEqualOrWithinRange( longStructPtr->optionLng,
    longStructPtr->mostNegLimit, longStructPtr->mostPosLimit );
  return( longStructPtr->active );
}


int  configureDoubleOption( struct optDbl *  dblStructPtr, char *  dblString ) {
  dblStructPtr->active = TRUE;
  dblStructPtr->optionDbl = convertOptionStringToDouble( dblStructPtr->defaultVal, dblString,
    dblStructPtr->optID, &dblStructPtr->active, TRUE );
  dblStructPtr->optionDbl = limitDoubleValueToEqualOrWithinRange( dblStructPtr->optionDbl,
    dblStructPtr->mostNegLimit, dblStructPtr->mostPosLimit );
  return( dblStructPtr->active );
}


int  configureChrOption( struct optChr *  chrStructPtr, char *  chrString )  {
  size_t  len;
  char *  end;

  len = strlen( chrString );
  chrStructPtr->active = TRUE;
  chrStructPtr->optionChr = ( int ) *chrString;
  if ( strspn( chrString, "0123456789" ) > 0 )  {     /* could be decimal, hex as 0x.. or octal as 011 */
    chrStructPtr->optionChr = ( int ) strtol( chrString, &end, 0 );
    if ( *end != '\0' )  {    /* expect end to point to end of string if number was ok */
      chrStructPtr->optionChr = '\0';
    }
  }
  else if (( *chrString == '\\' ) && ( len > ( size_t ) 1 ) && ( chrString[ 1 ] != '\\'))  {
    switch ( chrString[ 1 ] )  {
      case '0' : {    /* Could be NULL string ('\0') or maybe octal (e.g. \009) */
        chrStructPtr->optionChr = ( int ) strtol( chrString + 1, &end, 0 );
        if ( *end != '\0' )  {    /* expect end to point to end of string if octal was ok */
          chrStructPtr->optionChr = '\0';
        }
        break;   /* NULL */
      }
      case 'a' : chrStructPtr->optionChr = '\a'; break;   /* audible bell */
      case 'b' : chrStructPtr->optionChr = '\b'; break;   /* backspace */
      case 'f' : chrStructPtr->optionChr = '\f'; break;   /* form-feed */
      case 'n' : chrStructPtr->optionChr = '\n'; break;   /* newline */
      case 'r' : chrStructPtr->optionChr = '\r'; break;   /* carriage return */
      case 't' : chrStructPtr->optionChr = '\t'; break;   /* horizontal tab */
      case 'x' :
      case 'X' : {
        if ( len > ( size_t ) 3 )  {
          chrStructPtr->optionChr = convert2HexChrNibblesToInt( chrString + 2 );
        }
        else if ( len > ( size_t ) 2 )  {
          chrStructPtr->optionChr = convertHexChrNibbleToInt( chrString + 2 );
        }
        break;
      }
      default :  chrStructPtr->optionChr = ','; break;    /* reinstate comma */
    }
    chrStructPtr->optionChr = limitIntegerValueToEqualOrWithinRange( chrStructPtr->optionChr , 0, 127 );
  }
  return( chrStructPtr->active );
}

// Functions for Command Line Options Configuration from JSON Data
void  usage( struct config *  opt, char *  exeName )  {
  printf( "Usage:\n");
  printf( " %s [-A][-C CHR][-c CHR][-D][-d INT][-H][-h][-M][-N][-n][-o TXT][-P][-p][-R][-r][-S][-s INT][-v][-V] [FILE_1 .. [FILE_N]]\n", exeName );
  printf( " %s %s\n", opt->A.optID, opt->A.helpStr ); /* average */
  printf( " %s %s\n", opt->C.optID, opt->C.helpStr ); /* column_separator */
  printf( " %s %s\n", opt->c.optID, opt->c.helpStr ); /* comment_delimiter */
  printf( " %s %s\n", opt->D.optID, opt->D.helpStr ); /* debug */
  printf( " %s %s\n", opt->d.optID, opt->d.helpStr ); /* decimal_places */
  printf( " %s %s\n", opt->H.optID, opt->H.helpStr ); /* header */
  printf( " %s %s\n", opt->h.optID, opt->h.helpStr ); /* help */
  printf( " %s %s\n", opt->M.optID, opt->M.helpStr ); /* median */
  printf( " %s %s\n", opt->N.optID, opt->N.helpStr ); /* negetive */
  printf( " %s %s\n", opt->n.optID, opt->n.helpStr ); /* newline */
  printf( " %s %s\n", opt->o.optID, opt->o.helpStr ); /* output */
  printf( " %s %s\n", opt->P.optID, opt->P.helpStr ); /* positive */
  printf( " %s %s\n", opt->p.optID, opt->p.helpStr ); /* pearson */
  printf( " %s %s\n", opt->R.optID, opt->R.helpStr ); /* range */
  printf( " %s %s\n", opt->r.optID, opt->r.helpStr ); /* row */
  printf( " %s %s\n", opt->S.optID, opt->S.helpStr ); /* stddev */
  printf( " %s %s\n", opt->s.optID, opt->s.helpStr ); /* skip */
  printf( " %s %s\n", opt->v.optID, opt->v.helpStr ); /* verbose */
  printf( " %s %s\n", opt->V.optID, opt->V.helpStr ); /* version */
  printf( " %s %s\n", "[FILE_1 .. [FILE_N]]", "Optional Name(s) of File(s) to process" ); /* posParam1 */
}

void  initConfiguration ( struct config *  opt )  {
// posParam1: positionParam
  opt->posParam1.paramNameStr = "[FILE_1 .. [FILE_N]]";
  opt->posParam1.helpStr = "Optional Name(s) of File(s) to process";
// average: optFlg
  opt->A.active = FALSE;
  opt->A.optID = "-A";
  opt->A.helpStr = "...... disable Arithmetic Mean (i.e. average ) output.";
// column_separator: optChr
  opt->C.active = FALSE;
  opt->C.optID = "-C";
  opt->C.helpStr = "CHR .. use CHR, not comma as the column separator";
  opt->C.optionChr = ',';
// comment_delimiter: optChr
  opt->c.active = FALSE;
  opt->c.optID = "-c";
  opt->c.helpStr = "CHR .. use CHR, not hash as the comment delimiter";
  opt->c.optionChr = '#';
// debug: optFlg
  opt->D.active = FALSE;
  opt->D.optID = "-D";
  opt->D.helpStr = "...... enable debug output mode";
// decimal_places: optInt
  opt->d.active = FALSE;
  opt->d.optID = "-d";
  opt->d.helpStr = "INT .. provide INT decimal places on output stats - where 0 <= INT <= 30";
  opt->d.mostPosLimit = 30;
  opt->d.mostNegLimit = 0;
  opt->d.optionInt = 16;
  opt->d.defaultVal = 16;
// header: optFlg
  opt->H.active = FALSE;
  opt->H.optID = "-H";
  opt->H.helpStr = "...... enable Header output mode";
// help: optFlg
  opt->h.active = FALSE;
  opt->h.optID = "-h";
  opt->h.helpStr = "...... this help / usage information";
// median: optFlg
  opt->M.active = FALSE;
  opt->M.optID = "-M";
  opt->M.helpStr = "...... disable Median value output";
// negetive: optFlg
  opt->N.active = FALSE;
  opt->N.optID = "-N";
  opt->N.helpStr = "...... disable most Negetive value output";
// newline: optFlg
  opt->n.active = FALSE;
  opt->n.optID = "-n";
  opt->n.helpStr = "...... enable extra blank output lines";
// output: optStr
  opt->o.active = FALSE;
  opt->o.optID = "-o";
  opt->o.helpStr = "TXT .. send the output to a file named 'TXT'";
  opt->o.optionStr = "";
// positive: optFlg
  opt->P.active = FALSE;
  opt->P.optID = "-P";
  opt->P.helpStr = "...... disable most Positive value output";
// pearson: optFlg
  opt->p.active = FALSE;
  opt->p.optID = "-p";
  opt->p.helpStr = "...... disable Pearson's skew value output";
// range: optFlg
  opt->R.active = FALSE;
  opt->R.optID = "-R";
  opt->R.helpStr = "...... disable Range (i.e. most Pos - most Neg) value output";
// row: optFlg
  opt->r.active = FALSE;
  opt->r.optID = "-r";
  opt->r.helpStr = "...... enable output in Row format";
// stddev: optFlg
  opt->S.active = FALSE;
  opt->S.optID = "-S";
  opt->S.helpStr = "...... disable Standard Deviation value output";
// skip: optInt
  opt->s.active = FALSE;
  opt->s.optID = "-s";
  opt->s.helpStr = "INT .. skip INT lines at the start of data sources - where 0 <= INT <= 1000";
  opt->s.mostPosLimit = 1000;
  opt->s.mostNegLimit = 0;
  opt->s.optionInt = 0;
  opt->s.defaultVal = 0;
// verbose: optFlg
  opt->v.active = FALSE;
  opt->v.optID = "-v";
  opt->v.helpStr = "...... enable more verbose information output";
// version: optFlg
  opt->V.active = FALSE;
  opt->V.optID = "-V";
  opt->V.helpStr = "...... enable version information output";
}

int  setConfiguration ( int  argc, char *  argv[], struct config *  opt )  {
  int c;

  opterr = 0;
  while ((c = getopt (argc, argv, OPTIONS )) != -1 ) {
    switch ( c ) {
      case 'A': opt->A.active = TRUE; break; /* average */
      case 'C': configureChrOption( &opt->C, optarg ); break; /* column_separator */
      case 'c': configureChrOption( &opt->c, optarg ); break; /* comment_delimiter */
      case 'D': opt->D.active = TRUE; break; /* debug */
      case 'd': configureIntegerOption( &opt->d, optarg ); break; /* decimal_places */
      case 'H': opt->H.active = TRUE; break; /* header */
      case 'h': opt->h.active = TRUE; break; /* help */
      case 'M': opt->M.active = TRUE; break; /* median */
      case 'N': opt->N.active = TRUE; break; /* negetive */
      case 'n': opt->n.active = TRUE; break; /* newline */
      case 'o': opt->o.active = TRUE; opt->o.optionStr = optarg; break; /* output_file */
      case 'P': opt->P.active = TRUE; break; /* positive */
      case 'p': opt->p.active = TRUE; break; /* pearson */
      case 'R': opt->R.active = TRUE; break; /* range */
      case 'r': opt->r.active = TRUE; break; /* row */
      case 'S': opt->S.active = TRUE; break; /* stddev */
      case 's': configureIntegerOption( &opt->s, optarg ); break; /* skip */
      case 'v': opt->v.active = TRUE; break; /* verbose */
      case 'V': opt->V.active = TRUE; break; /* version */
      case '?' : {
        if ( strchr( "Ccdos", optopt ) != NULL ) {
          fprintf (stderr, "Error: Option -%c requires an argument.\n", optopt);
          switch ( optopt ) {
            case 'C': opt->C.active = FALSE; break;
            case 'c': opt->c.active = FALSE; break;
            case 'd': opt->d.active = FALSE; break;
            case 'o': opt->o.active = FALSE; break;
            case 's': opt->s.active = FALSE; break;
          }
        }
        else if (isprint (optopt))
          fprintf (stderr, "Warning: Unknown option \"-%c\" ? - ignoring it!\n", optopt);
        else
          fprintf (stderr, "Warning: Unknown non-printable option character 0x%x ? - ignoring it!\n", optopt);
        break;
      }
    }
  }
  return( optind );
}

void  configuration_status( struct config *  opt )  {
  printf( "Debug: option -A is %sctive (-A %s)\n", (opt->A.active) ? "a" : "ina", opt->A.helpStr); /* average */
  printf( "Debug: option -C is %sctive (-C %s)\n", (opt->C.active) ? "a" : "ina", opt->C.helpStr); /* column_separator */
  printf( "Debug: option -C value is '%c'\n", opt->C.optionChr); /* column_separator */
  printf( "Debug: option -c is %sctive (-c %s)\n", (opt->c.active) ? "a" : "ina", opt->c.helpStr); /* comment_delimiter */
  printf( "Debug: option -c value is '%c'\n", opt->c.optionChr); /* comment_delimiter */
  printf( "Debug: option -D is %sctive (-D %s)\n", (opt->D.active) ? "a" : "ina", opt->D.helpStr); /* debug */
  printf( "Debug: option -d is %sctive (-d %s)\n", (opt->d.active) ? "a" : "ina", opt->d.helpStr); /* decimal_places */
  printf( "Debug: option -d value is %d, limits: %d .. %d\n", opt->d.optionInt, opt->d.mostNegLimit, opt->d.mostPosLimit); /* decimal_places */
  printf( "Debug: option -H is %sctive (-H %s)\n", (opt->H.active) ? "a" : "ina", opt->H.helpStr); /* header */
  printf( "Debug: option -h is %sctive (-h %s)\n", (opt->h.active) ? "a" : "ina", opt->h.helpStr); /* help */
  printf( "Debug: option -M is %sctive (-M %s)\n", (opt->M.active) ? "a" : "ina", opt->M.helpStr); /* median */
  printf( "Debug: option -N is %sctive (-N %s)\n", (opt->N.active) ? "a" : "ina", opt->N.helpStr); /* negetive */
  printf( "Debug: option -n is %sctive (-n %s)\n", (opt->n.active) ? "a" : "ina", opt->n.helpStr); /* newline */
  printf( "Debug: option -o is %sctive (-o %s)\n", (opt->o.active) ? "a" : "ina", opt->o.helpStr); /* output */
  printf( "Debug: option -o value is \"%s\"\n", opt->o.optionStr); /* output */
  printf( "Debug: option -P is %sctive (-P %s)\n", (opt->P.active) ? "a" : "ina", opt->P.helpStr); /* positive */
  printf( "Debug: option -p is %sctive (-p %s)\n", (opt->p.active) ? "a" : "ina", opt->p.helpStr); /* pearson */
  printf( "Debug: option -R is %sctive (-R %s)\n", (opt->R.active) ? "a" : "ina", opt->R.helpStr); /* range */
  printf( "Debug: option -r is %sctive (-r %s)\n", (opt->r.active) ? "a" : "ina", opt->r.helpStr); /* row */
  printf( "Debug: option -S is %sctive (-S %s)\n", (opt->S.active) ? "a" : "ina", opt->S.helpStr); /* stddev */
  printf( "Debug: option -s is %sctive (-s %s)\n", (opt->s.active) ? "a" : "ina", opt->s.helpStr); /* skip */
  printf( "Debug: option -s value is %d, limits: %d .. %d\n", opt->s.optionInt, opt->s.mostNegLimit, opt->s.mostPosLimit); /* skip */
  printf( "Debug: option -v is %sctive (-v %s)\n", (opt->v.active) ? "a" : "ina", opt->v.helpStr); /* verbose */
  printf( "Debug: option -V is %sctive (-V %s)\n", (opt->V.active) ? "a" : "ina", opt->V.helpStr); /* version */
  printf( "Debug: %s (%s)\n", opt->posParam1.paramNameStr, opt->posParam1.helpStr);
}
