/*
 *
 *  C S V S T A T S . C
 *
 * csvstats.c last edited on Tue Apr 22 20:41:19 2025
 * 
 * descended from statsCSV.c 0v3
 *
 * Written by O.F.Holland.
 *
 */

#include <stdio.h>		/* fopen(), fclose(), fgets() */
#include <stdlib.h>		/* exit() qsort() */
#include <string.h>		/* strlen() */
#include <unistd.h>		/* getopt() */
#include <math.h>     /* sqrtl() */

#define  HEADER  "csvstats.c 0v0"
#define  ID_STRING  " csvstats 0v0 "
#ifndef  FALSE
#define  FALSE 0
#endif
#ifndef  TRUE
#define  TRUE (! FALSE)
#endif
#define  MAX_NUM_OF_COLS  100
#define  BFR_SIZE  15*MAX_NUM_OF_COLS
#define  COMP_NUMBER  2520

int  debugFlag;
int  verboseFlag;
int  lineModeFlag;	/* Change to each sample row on a new line */
int  headerFlag;	/* Ignore First line in stats file, as it is a header */
int  helpFlag;
int  arithmeticMeanOutputFlag;
int  medianOutputFlag;
int  mostPosOutputFlag;
int  mostNegOutputFlag;
int  rangeOutputFlag;
int  stdDevOutputFlag;
int  pearsonsSkewOutputFlag;
long  columnsSizes[ MAX_NUM_OF_COLS ];
long double  columnsSums[ MAX_NUM_OF_COLS ];
long double  columnsMeans[ MAX_NUM_OF_COLS ];
long double  columnsMedians[ MAX_NUM_OF_COLS ];
long double  columnsMostPos[ MAX_NUM_OF_COLS ];
long double  columnsMostNeg[ MAX_NUM_OF_COLS ];
long double  columnsSumOfSquares[ MAX_NUM_OF_COLS ];
long double  columnsStdDev[ MAX_NUM_OF_COLS ];
long double  columnsPearsonSkew[ MAX_NUM_OF_COLS ];
long double *  storage;   /* global storage for input numbers from all columns */

/*-------------------------------------------------------------*/

void  initializeProgramGlobalData( void )  {
  debugFlag = verboseFlag = FALSE;
  lineModeFlag = FALSE;
  headerFlag = FALSE;
  helpFlag = FALSE;
  arithmeticMeanOutputFlag = TRUE;
  medianOutputFlag = TRUE;
  mostPosOutputFlag = TRUE;
  mostNegOutputFlag = TRUE;
  rangeOutputFlag = TRUE;
  stdDevOutputFlag = TRUE;
  pearsonsSkewOutputFlag = TRUE;
}


/*----------------------------------*/

void usage(void)  {
   printf( "Usage: csvstats [-ADhMNnPRSv] csvFileName\n" );
   printf( " where ;-\n" );
   printf( "  The -A switch turns Off Arithmetic Mean (i.e. average ) output.\n");
   printf( "  The -D switch turns On Debug Information output.\n");
   printf( "  The -H switch turns On Header output mode.\n");
   printf( "  The -h switch turns On this output.\n");
   printf( "  The -M switch turns Off Median value output.\n");
   printf( "  The -N switch turns Off most Negetive value output.\n");
   printf( "  The -n switch turns Off the linefeed terminator on the output.\n");
   printf( "  The -P switch turns Off most Positive value output.\n");
   printf( "  The -p switch turns Off most Pearson's skew value output.\n");
   printf( "  The -R switch turns Off Range (i.e. most Pos - most Neg) value output.\n");
   printf( "  The -S switch turns Off Standard Deviation value output.\n");
   printf( "  The -v switch turns On verbose output (notably a header).\n");
}


/*----------------------------------*/

int  modifyProgramGlobalDataAccordingToCommandLineSwitches( int  argc, char *  argv[] )  {
  int  ch;

  while(( ch = getopt( argc, argv, "ADHhMNnPpRSv" )) != -1 )  {
    /* Process any switch parameters at the end of the command line */
    switch( ch )  {
      case 'A' : arithmeticMeanOutputFlag = FALSE; break;
      case 'D' : debugFlag = TRUE; break;
      case 'H' : headerFlag = TRUE; break;
      case 'h' : helpFlag = TRUE; break;
      case 'M' : medianOutputFlag = FALSE; break;
      case 'N' : mostNegOutputFlag = FALSE; break;
      case 'n' : lineModeFlag = TRUE; break;
      case 'P' : mostPosOutputFlag = FALSE; break;
      case 'p' : pearsonsSkewOutputFlag = FALSE; break;
      case 'R' : rangeOutputFlag = FALSE; break;
      case 'S' : stdDevOutputFlag = FALSE; break;
      case 'v' : verboseFlag = TRUE; break;
      case '?' : 
      default  : 
        fprintf( stderr, "?? command line switch \"%c\" not understood\n",
          optopt );
        usage(); break;
    } /* end switch */
  } /* end while */
  if( debugFlag )  printf( "Command Line parameters looked at = %d\n", optind );
  return( optind );
}


/*----------------------------------*/

int  processFirstLineOfNumbers( char *  linePtr )  {
  int  cnt, result;
  long double *  sumsPtr;
  long double *  mostPosPtr;
  long double *  mostNegPtr;
  long double *  sumsOfSquaresPtr;
  long double *  storagePtr;

  sumsPtr = columnsSums;
  mostPosPtr = columnsMostPos;
  mostNegPtr = columnsMostNeg;
  sumsOfSquaresPtr = columnsSumOfSquares;
  storagePtr = storage;
  cnt = 0;
  do  {
    result = sscanf( linePtr, "%Lg", sumsPtr );   /* read numbers as long double (i.e. high precision floating point )*/
    if( result == 1 )
      columnsSizes[ cnt++ ] = 1;  /* Only increment if successful */
    *storagePtr++ = *sumsPtr;       /* put new value in storage */
    *sumsOfSquaresPtr++ = *sumsPtr * *sumsPtr;
    *mostPosPtr++  = *sumsPtr;
    *mostNegPtr++  = *sumsPtr++;
    linePtr = strchr( linePtr, ',' );
    if( linePtr != NULL )  linePtr += 1;	/* step over comma */
  } while(( linePtr != NULL ) && ( cnt < MAX_NUM_OF_COLS )); 
  return( cnt );
}


/*----------------------------------*/

int  processLine( char *  linePtr, int  numberOfColumns )  {
  int  cnt;
  long double  dTmp;
  long double *  sumsPtr;
  long double *  mostPosPtr;
  long double *  mostNegPtr;
  long double *  sumsOfSquaresPtr;
  long double *  storagePtr;

  sumsPtr = columnsSums;
  mostPosPtr = columnsMostPos;
  mostNegPtr = columnsMostNeg;
  sumsOfSquaresPtr = columnsSumOfSquares;
  storagePtr = storage + numberOfColumns;     /* start after the first line */
  cnt = 0;
  do  {
    if( sscanf( linePtr, "%Lg", &dTmp ) == 1 )  {   /* read numbers as long double (i.e. high precision floating point )*/
      columnsSizes[ cnt ] += 1;
      *storagePtr++ = dTmp;   /* save number to storage */
      *sumsPtr++ += dTmp;
      *sumsOfSquaresPtr++ += dTmp * dTmp;
      if( dTmp > *mostPosPtr )  *mostPosPtr++ = dTmp;
      else  mostPosPtr++;
      if( dTmp < *mostNegPtr )  *mostNegPtr++ = dTmp;
      else  mostNegPtr++;
      cnt += 1;   /* set up to do next column */
    }
    linePtr = strchr( linePtr, ',' );
    if( linePtr != NULL )  linePtr += 1;	/* step over comma */
  } while(( linePtr != NULL ) && ( cnt < numberOfColumns )); 
  return( cnt );
}


/*----------------------------------*/

int  compareDoubles( const void *  ptr1, const void *  ptr2 )  {
  long double *  dp1;
  long double *  dp2;

  dp1 = ( long double * ) ptr1;
  dp2 = ( long double * ) ptr2;
  if ( *dp1 > *dp2 )  return( 1 );
  else if ( *dp2 > *dp1 )  return( -1 );
  return 0;
}


/*----------------------------------*/

int  calcStdDev( int  numOfCols, long  numOfColValues[], long double  store[], long double *  colStdDev, long double *  colMeans )  {
  int  i;
  long j;
  long double *  colStorage;
  long double *  storagePtr;
  long double *  ptr;

  /* find longest column, just in case they are not all the same lengths */
  for( i = 0, j = 0; i < numOfCols; i++ )  if ( numOfColValues[ i ] > j )  j = numOfColValues[ i ];   /* find biggest */
  /* ensure we allocate enough temporary storage for the longest column */
  if (( colStorage = calloc( j, sizeof( long double ))) == NULL )
    return( 0 );  /* No medians calculated */
  else  {
    for( i = 0; i < numOfCols; i++ )  {   /* step through each columns data */
      ptr = colStorage;         /* point at start of temp column storage */
      storagePtr = store + i;   /* point at first value for the column in the external store */
      /* demultiplex stored value into the column, but also subtract the mean from each reading */
      /* this doesn't change the Std. Dev. but does reduce the risk of numerical inaccuracies */
      for( j = 0; j < numOfColValues[ i ]; j++ )  {
        *ptr++ = *storagePtr - colMeans[ i ];   /* partially normalize numbers */
        storagePtr += numOfCols;    /* step to next number for the current column */
      }
#ifdef  DEBUG
      if ( debugFlag )  {
        printf( "\ncolumn %d centred values\n", i );
        for( j = 0; j < numOfColValues[ i ]; j++ )  printf( "%ld: %Lg\n", j, colStorage[ j ]);
      }
#endif
      /* find the mean value of the column of partially normalized numbers */
      colMeans[ i ] = ( long double ) 0.0;
      for( j = 0, ptr = colStorage; j < numOfColValues[ i ]; j++ )  colMeans[ i ] += *ptr++; /* Sum the column */
      colMeans[ i ] = colMeans[ i ] / ( long double ) numOfColValues[ i ];   /* recalc mean */
      /* find the Std. Dev. of partially normalized numbers */
      colStdDev[ i ] = ( long double ) 0.0;
      for( j = 0, ptr = colStorage; j < numOfColValues[ i ]; j++, ptr++ )
        colStdDev[ i ] += ( colMeans[ i ] - *ptr ) * ( colMeans[ i ] - *ptr ); /* Sum squares the column */
      if( colStdDev[ i ] > (long double ) 0.0 )
        colStdDev[ i ] = sqrtl( colStdDev[ i ] / ( long double ) ( numOfColValues[ i ] - 1 ));
      else  colStdDev[ i ] = ( long double ) 0.0;
    }
    free( colStorage );
  }
  return( numOfCols );
}


/*----------------------------------*/

int  calcMedians( int  numOfCols, long  numOfColValues[], long double  store[], long double *  colMedians )  {
  int  i;
  long  j;
  long double *  colStorage;
  long double *  storagePtr;
  long double *  ptr;

  /* find longest column, just in case they are not all the same lengths */
  for( i = 0, j = 0; i < numOfCols; i++ )  if ( numOfColValues[ i ] > j )  j = numOfColValues[ i ];   /* find biggest */
  /* ensure we allocate enough temporary storage for the longest column */
  if (( colStorage = calloc( j, sizeof( long double ))) == NULL )
    return( 0 );  /* No medians calculated */
  else  {
    for( i = 0; i < numOfCols; i++ )  {   /* step through each columns data */
      ptr = colStorage;         /* point at start of temp column storage */
      storagePtr = store + i;   /* point at first value for the column in the external store */
      /* demultiplex stored value into the column */
      for( j = 0; j < numOfColValues[ i ]; j++ )  {
        *ptr++ = *storagePtr;
        storagePtr += numOfCols;
      }
#ifdef  DEBUG
      if ( debugFlag )  {
        printf( "\nUnsorted column %d ( %ld values)\n", i, numOfColValues[ i ] );
        for( j = 0; j < numOfColValues[ i ]; j++ )  printf( "%ld: %Lg\n", j, colStorage[ j ]);
      }
#endif
      /* sort the column */
      qsort( colStorage, numOfColValues[ i ], sizeof( long double ), compareDoubles );
#ifdef  DEBUG
      if ( debugFlag )  {
        printf( "Sorted column %d\n", i );
        for( j = 0; j < numOfColValues[ i ]; j++ )  printf( "%ld: %Lg\n", j, colStorage[ j ]);
      }
#endif
      /* find the median value */
     j = numOfColValues[ i ] / 2;
      if (( numOfColValues[ i ] % 2 ) == 1 )   /* odd number ? */
        colMedians[ i ] =  colStorage[ j ];
      else  {
        colMedians[ i ] = 0.5 * ( colStorage[ j - 1 ] + colStorage[ j ]);
      }
    }
    free( colStorage );
  }
  return( numOfCols );
}


/*----------------------------------*/

void  printLongAndSeparator( long  number, char *  separator )  {
  printf( "%ld", number );
  if (( separator != NULL ) || ( *separator != '\0' ))  printf( "%s", separator );
}


/*----------------------------------*/

void  printNumberAndSeparator( long double  number, char *  separator )  {
  printf( "%+.14Lg", number );
  if (( separator != NULL ) || ( *separator != '\0' ))  printf( "%s", separator );
}


/*----------------------------------*/

void  printColumnsStatsInRows( int  numOfColumns, long  numOfValues[], char *  separatorStr, int  oneRowFlag )  {
  int  i;

  /* all columns Stats except the most righthand one need the separator */
  for( i = 0; i < numOfColumns; i++ )  {
    printf( "%d, %ld, ", i + 1, numOfValues[ i ] );  /* column number, count, assume at least one column stat to print */
    printNumberAndSeparator( columnsSums[ i ], ( medianOutputFlag || arithmeticMeanOutputFlag || mostPosOutputFlag ||
      mostNegOutputFlag || rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
    if( medianOutputFlag )
      printNumberAndSeparator( columnsMedians[ i ], ( arithmeticMeanOutputFlag || mostPosOutputFlag || mostNegOutputFlag ||
        rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
    if( arithmeticMeanOutputFlag )
      printNumberAndSeparator( columnsMeans[ i ], ( mostPosOutputFlag || mostNegOutputFlag ||
        rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
    if( mostPosOutputFlag )
      printNumberAndSeparator( columnsMostPos[ i ], ( mostNegOutputFlag ||
        rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
    if( mostNegOutputFlag )
      printNumberAndSeparator( columnsMostNeg[ i ], (
        rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
    if( rangeOutputFlag )
      printNumberAndSeparator( columnsMostPos[ i ] - columnsMostNeg[i], (
        stdDevOutputFlag ) ? separatorStr : "" );
    if( stdDevOutputFlag )  {
      printNumberAndSeparator( columnsStdDev[ i ], "" );
    }
    /* if oneRowFlag is active then continue using separator until the last column stats are printed */
    if ( oneRowFlag && (( i + 1 ) < numOfColumns ))  printf( "%s", separatorStr );
    else  printf( "\n" );
  }
}


/*----------------------------------*/

void  printColumnsStatsLabelsInRows( int  numOfColumns, char *  separatorStr )  {
  /* all columns Stats except the most righthand one need the separator */
  printf( "\"Column Number\", \"Count\", " );  /* column number, count, assume at least one column stat to print */
  printf( "\"Sum\"%s", ( medianOutputFlag || arithmeticMeanOutputFlag || mostPosOutputFlag ||
    mostNegOutputFlag || rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
  if( medianOutputFlag )
    printf( "\"Median\"%s", ( arithmeticMeanOutputFlag || mostPosOutputFlag || mostNegOutputFlag ||
      rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
  if( arithmeticMeanOutputFlag )
    printf( "\"Mean\"%s", ( mostPosOutputFlag || mostNegOutputFlag ||
      rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
  if( mostPosOutputFlag )
    printf( "\"Most +ve\"%s", ( mostNegOutputFlag ||
      rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
  if( mostNegOutputFlag )
    printf( "\"Most -ve\"%s", ( rangeOutputFlag || stdDevOutputFlag ) ? separatorStr : "" );
  if( rangeOutputFlag )
    printf( "\"Range\"%s", ( stdDevOutputFlag ) ? separatorStr : "" );
  if( stdDevOutputFlag )  {
    printf( "\"Std. Dev.\"" );
  }
}


/*----------------------------------*/

void  printColumnsStatsInColumns( int  numOfColumns, long  numOfValues[], char *  separatorStr, int verbose )  {
  int  i;

  /* all columns Stats except the most righthand one need the separator */
  if ( verbose || headerFlag )  printf( "\"Count\", " );
  for( i = 0; i < numOfColumns; i++ )
    printLongAndSeparator( numOfValues[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
  if ( verbose || headerFlag )  printf( "\"Sum\", " );
  for( i = 0; i < numOfColumns; i++ )
    printNumberAndSeparator( columnsSums[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
  if( medianOutputFlag )  {
    if ( verbose || headerFlag )  printf( "\"Median\", " );
    for( i = 0; i < numOfColumns; i++ )
      printNumberAndSeparator( columnsMedians[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
  }
  if( arithmeticMeanOutputFlag )  {
    if ( verbose || headerFlag )  printf( "\"Mean\", " );
    for( i = 0; i < numOfColumns; i++ )
      printNumberAndSeparator( columnsMeans[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
  }
  if( mostPosOutputFlag )  {
    if ( verbose || headerFlag )  printf( "\"Most +ve\", " );
    for( i = 0; i < numOfColumns; i++ )
      printNumberAndSeparator( columnsMostPos[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
  }
  if( mostNegOutputFlag )  {
    if ( verbose || headerFlag )  printf( "\"Most -ve\", " );
    for( i = 0; i < numOfColumns; i++ )
      printNumberAndSeparator( columnsMostNeg[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
  }
  if( rangeOutputFlag )  {
    if ( verbose || headerFlag )  printf( "\"Range\", " );
    for( i = 0; i < numOfColumns; i++ )
      printNumberAndSeparator( columnsMostPos[i] - columnsMostNeg[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
  }
  if( stdDevOutputFlag )  {
    if ( verbose || headerFlag )  printf( "\"Std. Dev.\", " );
    for( i = 0; i < numOfColumns; i++ )  {
      printNumberAndSeparator( columnsStdDev[ i ], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
    }
  }
  if( pearsonsSkewOutputFlag )  {
    if ( verbose || headerFlag )  printf( "\"Pearson's Skew\", " );
    for( i = 0; i < numOfColumns; i++ )  {
      printNumberAndSeparator( columnsPearsonSkew[ i ], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
    }
  }
}


/*----------------------------------*/

int  main( int argc, char * argv[])  {
  int  i, numOfColumns, farg;
  int  exitFlag;
  long  totalNumbers, lineNumber;
  size_t  storageSize;
  FILE *  fp;
  char  bfr[ BFR_SIZE ];

  numOfColumns = 0;
  exitFlag = EXIT_FAILURE;
  lineNumber = (long) 0;
  storageSize = COMP_NUMBER;
  initializeProgramGlobalData();
  farg = modifyProgramGlobalDataAccordingToCommandLineSwitches( argc, argv );
  if( debugFlag || verboseFlag )  {
    printf( "%s - %s\n", argv[ 0 ], HEADER );
    if( debugFlag )
      printf( "Source file %s, compiled on %s at %s\n", __FILE__, __DATE__, __TIME__ );
  }
  if( argc <= farg )  {
    fprintf( stderr, "Error: Please specify a filename to identify the csv file to read\n" );
    if ( ! helpFlag )  usage();   /* if -h was specified then no need to print usage here, it will be printed later */
  }
  else  {
    fp = fopen( argv[ farg ], "r" );
    if( fp == NULL )
      fprintf( stderr, "Error: Unable to open file named \"%s\".\n", argv[ farg ] );
    else  {
      if (( storage = malloc( storageSize * sizeof( long double ))) == NULL )  {
        fprintf( stderr, "Error: unable to allocate memory to store %ld csv numbers: Aborting\n", storageSize );
      }
      else  {
        while( fgets( bfr, BFR_SIZE - 1, fp ) != NULL )  {
          if (( *bfr == '\0' ) || ( *bfr == '#' ) || ( *bfr == '\r' ) || ( *bfr == '\n' ))  {
            if ( debugFlag )  printf( "Debug: Warning: skipping a line (blank or comment)\n" );
          }
          else  {
            lineNumber += 1;
            if( lineNumber == 1 )  {
              if ( debugFlag )  printf( "Debug: %ld: '%s'\n", lineNumber, bfr );
	            totalNumbers = numOfColumns = processFirstLineOfNumbers( bfr );
              if( debugFlag )
                printf( "Debug: %d Column(s) found in the first line of numbers.\n", numOfColumns );
            }
    	      else  {
              if(( i = processLine( bfr, totalNumbers )) != numOfColumns )  {
                if ( verboseFlag )
                  printf( "Warning: %d values in line %ld doesn't match the expected %d values found in the first line\n",
                    i, lineNumber, numOfColumns );
              }
              totalNumbers += i;  /* keep track of how many numbers in total are in the storage */
              if ( debugFlag )  printf( "%ld total numbers so far\n", totalNumbers );
              if ( totalNumbers >= ( storageSize - numOfColumns ))  {
                if ( debugFlag )
                  printf( "Warning: Storage for csv numbers is nearly full (%ld): allocating more storage\n", totalNumbers );
                storageSize += COMP_NUMBER;    /* increase storage size to avoid overflow */
                if (( storage = realloc( storage, storageSize * sizeof( long double ))) == NULL ) {
                  fprintf( stderr, "Error: unable to get more memory to store more csv numbers at input line %ld: Aborting\n", lineNumber );
                  return( -2 );
                }
              }
            }
          }
        }
        exitFlag = EXIT_SUCCESS;
      }
      fclose( fp );
      if( debugFlag || verboseFlag )
        printf( "Statistics based on %ld lines(s) from \"%s\".\n", lineNumber, argv[ farg ] );
      if ( calcMedians( numOfColumns, columnsSizes, storage, columnsMedians ) != numOfColumns )
        printf( "Warning: unable to calulate median values\n" );
      /* Calculate means for use in the Std Dev calcs */
      for( i = 0; i < numOfColumns; i++ )
        columnsMeans[ i ] = columnsSums[ i ] / ( long double ) columnsSizes[ i ];
      if ( calcStdDev( numOfColumns, columnsSizes, storage, columnsStdDev, columnsMeans ) != numOfColumns )
        printf( "Warning: unable to calulate Std. Dev. values\n" );
      /* Re-calculate means after use in the Std Dev calcs & then calc Pearson's measure of skewness */
      for( i = 0; i < numOfColumns; i++ )  {
        columnsMeans[ i ] = columnsSums[ i ] / ( long double ) columnsSizes[ i ];
        columnsPearsonSkew[ i ] = ( long double ) 3 * ( columnsMeans[ i ] - columnsMedians[ i ]) / columnsStdDev[ i ];
      }
      /* Print out the stats in various forms of csv format */
      if( lineModeFlag )  printf( "\n" );   /* output a blank line if in line mode */
      /* print all column stats in just one line (i.e. just one row ) */
      if ( verboseFlag || headerFlag )  {
        for( i = 0; i < numOfColumns; i++ )  {
          printColumnsStatsLabelsInRows( numOfColumns, ", " );
          if (( i + 1 ) < numOfColumns )  printf( ", " );   /* add separator up to the penultimate column */
        }
        printf( "\n" );
      }
      printColumnsStatsInRows( numOfColumns, columnsSizes, ", ", TRUE );
      if( lineModeFlag )  printf( "\n" );   /* output a blank line if in line mode */
      /* print column stats in row form (i.e. one line or row per column ) */
      if ( verboseFlag || headerFlag )  {
        printColumnsStatsLabelsInRows( numOfColumns, ", " );
        printf( "\n" );
      }
      printColumnsStatsInRows( numOfColumns, columnsSizes, ", ", FALSE );
      if( lineModeFlag )  printf( "\n" );   /* output a blank line if in line mode */
      /* print column stats in column form */
      if ( verboseFlag || headerFlag )  {
        printf( "\"Stat. Type\", " );
        for( i = 1; i < numOfColumns; i++ )  printf( "\"Column %d\", ", i );
        printf( "\"Column %d\"\n", numOfColumns );
      }
      printColumnsStatsInColumns( numOfColumns, columnsSizes, ", ", verboseFlag );
#ifdef  DEBUG
      if ( debugFlag )  {
        printf( "\ncsv storage listing\n" );
        for( i = 0; i < totalNumbers; i++ )  printf( "%d : %Lg\n", i, storage[ i ]);
      }
#endif
      if ( storage != NULL )  free( storage );
    }
  }
  if( lineModeFlag )  printf( "\n" );   /* output an extra line feed */
  if( helpFlag )  usage();
  return( exitFlag );
}
