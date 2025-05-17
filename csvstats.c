/*
 *
 *  C S V S T A T S . C
 *
 * csvstats.c last edited on Sat May 17 21:03:57 2025
 * 
 * 0v2 Changed to use double precision by default & added more debug code
 * 0v1 Added Std. Dev. of Sample output (i.e. denom. = n)
 * 0v0 descended from statsCSV.c 0v3
 * 
 * Written by O.F.Holland.
 *
 */

#include <stdio.h>		/* fopen(), fclose(), fgets() */
#include <stdlib.h>		/* exit() qsort() */
#include <string.h>		/* strlen() */
#include <unistd.h>		/* getopt() */
#include <math.h>		/* sqrtl(), sqrt(), sqrtf() */

#define  HEADER  "csvstats.c 0v2"
#define  ID_STRING  " csvstats 0v2 "
#ifndef  FALSE
#define  FALSE 0
#endif
#ifndef  TRUE
#define  TRUE (! FALSE)
#endif
#define  MAX_NUM_OF_COLS  100
#define  BFR_SIZE  15*MAX_NUM_OF_COLS
#define  COMP_NUMBER  2520
/* Set up default Floating Point Precision to double */
#ifndef  LDBL_PREC
#ifndef  FLT_PREC
#define  DBL_PREC 1
#endif
#endif

/* Handle different precision of floating point numbers - This is clunky probably needs rewriting in C++ */
#ifdef  LDBL_PREC
#define  REAL_NUM_PREC  long double
#define  REAL_NUM_TYPE_STR  "long double"
#define  SIZEOF_REAL_NUM  __SIZEOF_LONG_DOUBLE__
#define  RD_IN_FMT  "%Lg"
#define  WRT_OUT_FMT  "%+.*Lg"
#define  WRT_OUT_DIG  __LDBL_DECIMAL_DIG__
#define  FABS  fabsl
#define  SQRT  sqrtl
#endif
#ifdef  DBL_PREC
#define  REAL_NUM_PREC  double
#define  REAL_NUM_TYPE_STR  "double"
#define  SIZEOF_REAL_NUM  __SIZEOF_DOUBLE__
#define  RD_IN_FMT  "%lg"
#define  WRT_OUT_FMT  "%+.*lg"
#define  WRT_OUT_DIG  __DBL_DECIMAL_DIG__
#define  FABS  fabs
#define  SQRT  sqrt
#endif
#ifdef  FLT_PREC
#define  REAL_NUM_PREC  float
#define  REAL_NUM_TYPE_STR  "float"
#define  SIZEOF_REAL_NUM  __SIZEOF_FLOAT__
#define  RD_IN_FMT  "%g"
#define  WRT_OUT_FMT  "%+.*e"
#define  WRT_OUT_DIG  __FLT_DECIMAL_DIG__
#define  FABS  fabsf
#define  SQRT  sqrtf
#endif

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
size_t  columnsSizes[ MAX_NUM_OF_COLS ];
REAL_NUM_PREC  columnsSums[ MAX_NUM_OF_COLS ];
REAL_NUM_PREC  columnsMeans[ MAX_NUM_OF_COLS ];
REAL_NUM_PREC  columnsMedians[ MAX_NUM_OF_COLS ];
REAL_NUM_PREC  columnsMostPos[ MAX_NUM_OF_COLS ];
REAL_NUM_PREC  columnsMostNeg[ MAX_NUM_OF_COLS ];
REAL_NUM_PREC  columnsSampleStdDev[ MAX_NUM_OF_COLS ];  /* Sample Std. Dev. (denom. = n) */
REAL_NUM_PREC  columnsEstPopStdDev[ MAX_NUM_OF_COLS ];  /* Population Std. Dev. estimate (denom. = n-1) */
REAL_NUM_PREC  columnsPearsonSkew[ MAX_NUM_OF_COLS ];
REAL_NUM_PREC *  storage;   /* global storage for input numbers from all columns */

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
  REAL_NUM_PREC *  sumsPtr;
  REAL_NUM_PREC *  mostPosPtr;
  REAL_NUM_PREC *  mostNegPtr;
  REAL_NUM_PREC *  storagePtr;

  sumsPtr = columnsSums;
  mostPosPtr = columnsMostPos;
  mostNegPtr = columnsMostNeg;
  storagePtr = storage;
  cnt = 0;
  do  {
    result = sscanf( linePtr, RD_IN_FMT, sumsPtr );   /* read numbers in desired precision floating point */
    if( result == 1 )
      columnsSizes[ cnt++ ] = 1;  /* Only increment if successful */
    *storagePtr++ = *sumsPtr;     /* put new value in storage */
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
  REAL_NUM_PREC  dTmp;
  REAL_NUM_PREC *  sumsPtr;
  REAL_NUM_PREC *  mostPosPtr;
  REAL_NUM_PREC *  mostNegPtr;
  REAL_NUM_PREC *  storagePtr;

  sumsPtr = columnsSums;
  mostPosPtr = columnsMostPos;
  mostNegPtr = columnsMostNeg;
  storagePtr = storage + numberOfColumns;     /* start after the first line */
  cnt = 0;
  do  {
    if( sscanf( linePtr, RD_IN_FMT, &dTmp ) == 1 )  {   /* read numbers in desired precision floating point */
      columnsSizes[ cnt ] += 1;
      *storagePtr++ = dTmp;   /* save number to storage */
      *sumsPtr++ += dTmp;
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

void  printNumberAndSeparator( REAL_NUM_PREC  number, char *  separator )  {
  printf( WRT_OUT_FMT, WRT_OUT_DIG, number );   /* use precision specified by macro in float.h */
  if (( separator != NULL ) || ( *separator != '\0' ))  printf( "%s", separator );
}


/*----------------------------------*/

void  printDebugStats( int colnum, REAL_NUM_PREC  ave, REAL_NUM_PREC  adev, REAL_NUM_PREC  sdev, REAL_NUM_PREC  var,
  REAL_NUM_PREC  skew, REAL_NUM_PREC  curt )  {

  printf( "Debug: Col %d, ", colnum );
  printf( "Av. " );
  printNumberAndSeparator( ave, ", " );
  printf( "ADev. " );
  printNumberAndSeparator( adev, ", " );
  printf( "SDev. " );
  printNumberAndSeparator( sdev, ", " );
  printf( "Var. " );
  printNumberAndSeparator( var, ", " );
  printf( "Skew " );
  printNumberAndSeparator( skew, ", " );
  printf( "Kurt. " );
  printNumberAndSeparator( curt, ", " );
  printf( "\n" );
}

#ifdef  DEBUG
/*----------------------------------*/

void  doStats( REAL_NUM_PREC  data[], size_t  dataPoints, REAL_NUM_PREC *  average, REAL_NUM_PREC *  adev,
  REAL_NUM_PREC *  sdev, REAL_NUM_PREC *  var, REAL_NUM_PREC *  skew, REAL_NUM_PREC *  kurt )  {
  size_t  index;
  REAL_NUM_PREC  sum, dist, d2sum, d3sum, d4sum, a_mean, ld_n;

  *adev = *sdev = *var = *skew = *kurt = d2sum = d3sum = d4sum = sum = ( REAL_NUM_PREC) 0.0;
  if ( dataPoints == ( size_t ) 0 )  { *average = ( REAL_NUM_PREC) 0.0; }
  else if ( dataPoints == ( size_t ) 1 )  { *average = data[0]; }
  else  {
    ld_n = ( REAL_NUM_PREC ) dataPoints;
    for ( index = 0; index < dataPoints; index++ )  sum += data[ index ];    /* total up the data */
    *average = a_mean = sum / ld_n;    /* calculate the arithmetic mean as the average */
    for ( index = 0; index < dataPoints; index++ )  {
      dist = data[ index ] - a_mean;
      *adev += FABS( dist );
      d2sum += ( dist * dist );   /* 2nd power of distance from average */
      d3sum += ( dist * dist * dist );  /* 3rd power of distance from average */
      d4sum += ( dist * dist * dist * dist );   /* 4th power of distance from average */
    }
    *adev /= ld_n;
    *var = d2sum / ( REAL_NUM_PREC )( dataPoints - 1 );
    if (( *var == ( REAL_NUM_PREC ) +0.0 ) || ( *var == ( REAL_NUM_PREC ) -0.0 ))  {
      printf( "Warning: Variance is zero: unable to calculate Std. Dev.\n");
    }
    else  {
      *sdev = SQRT( *var );
      *skew = d3sum / ( ld_n * ( *var ) * ( *sdev ));
      *kurt = ( d4sum / ( ld_n * ( *var ) * ( *var ))) - ( REAL_NUM_PREC ) 3.0;
    }
  }
}
#endif


/*----------------------------------*/

int  calcStdDev( int  numOfCols, size_t  numOfColValues[], REAL_NUM_PREC  store[], REAL_NUM_PREC *  colEstStdDev,
  REAL_NUM_PREC *  colSmplStdDev, REAL_NUM_PREC *  colMeans )  {
  int  i;
  size_t j;
  REAL_NUM_PREC *  colStorage;
  REAL_NUM_PREC *  storagePtr;
  REAL_NUM_PREC *  ptr;
#ifdef DEBUG
  REAL_NUM_PREC  ave, adev, sdev, var, skew, curt;
#endif

  printf( "Std. Dev. with %s precision (%d byte numbers)\n", REAL_NUM_TYPE_STR, SIZEOF_REAL_NUM );
  /* find longest column, just in case they are not all the same lengths */
  for( i = 0, j = 0; i < numOfCols; i++ )  if ( numOfColValues[ i ] > j )  j = numOfColValues[ i ];   /* find biggest */
  /* ensure we allocate enough temporary storage for the longest column */
  if (( colStorage = calloc( j, SIZEOF_REAL_NUM )) == NULL )
    return( 0 );  /* Std. Dev. not calculated if no memory is available */
  else  {
    for( i = 0; i < numOfCols; i++ )  {   /* step through each columns data */
#ifdef  DEBUG
      ptr = colStorage;         /* point at start of temp column storage */
      storagePtr = store + i;   /* point at first value for the column in the external store */
      /* demultiplex stored value into the column, but don't subtract the mean from each reading */
      /* this is straight forward, but comes at a risk of possible numerical inaccuracies */
      for( j = 0; j < numOfColValues[ i ]; j++ )  {
        *ptr++ = *storagePtr;   /* just copy, don't partially normalize numbers */
        storagePtr += numOfCols;  /* step to next number for the current column */
      }
      if ( debugFlag )  {
        ave = adev = sdev = var = skew = curt = ( REAL_NUM_PREC ) 0.0;
        doStats( colStorage, numOfColValues[ i ], &ave, &adev, &sdev, &var, &skew, &curt );
        printDebugStats( i, ave, adev, sdev, var, skew, curt );
      }
#endif
      ptr = colStorage;         /* point at start of temp column storage */
      storagePtr = store + i;   /* point at first value for the column in the external store */
      /* demultiplex stored value into the column, but also subtract the mean from each reading */
      /* this doesn't change the Std. Dev. but does reduce the risk of numerical inaccuracies */
      for( j = 0; j < numOfColValues[ i ]; j++ )  {
        *ptr++ = *storagePtr - colMeans[ i ];   /* partially normalize numbers */
        storagePtr += numOfCols;  /* step to next number for the current column */
      }
#ifdef  DEBUG
      if ( debugFlag )  {
        ave = adev = sdev = var = skew = curt = ( REAL_NUM_PREC ) 0.0;
        doStats( colStorage, numOfColValues[ i ], &ave, &adev, &sdev, &var, &skew, &curt );
        printDebugStats( i, ave, adev, sdev, var, skew, curt );
      }
      if ( debugFlag )  {
        printf( "\nDebug: column %d centred values\n", i );
        for( j = 0; j < numOfColValues[ i ]; j++ )  {
          printf( "%ld: ", j );
          printNumberAndSeparator( colStorage[ j ], "\n");
        }
      }
#endif
      /* find the mean value of the column of partially normalized numbers */
      colMeans[ i ] = ( REAL_NUM_PREC ) 0.0;
      for( j = 0, ptr = colStorage; j < numOfColValues[ i ]; j++ )  colMeans[ i ] += *ptr++; /* Sum the column */
      colMeans[ i ] /= ( REAL_NUM_PREC ) numOfColValues[ i ];   /* recalc mean */
      /* find the Std. Dev. of partially normalized numbers */
      colEstStdDev[ i ] = ( REAL_NUM_PREC ) 0.0;
      for( j = 0, ptr = colStorage; j < numOfColValues[ i ]; j++, ptr++ )
        colEstStdDev[ i ] += ( colMeans[ i ] - *ptr ) * ( colMeans[ i ] - *ptr ); /* Sum squares the column */
      colSmplStdDev[ i ] = colEstStdDev[ i ];   /* copy sum of squares value */
      if( colEstStdDev[ i ] > (REAL_NUM_PREC ) 0.0 )  {
        colEstStdDev[ i ] = SQRT( colEstStdDev[ i ] / ( REAL_NUM_PREC ) ( numOfColValues[ i ] - 1 ));  /* Estimated Std. Dev. of Population */
        colSmplStdDev[ i ] = SQRT( colSmplStdDev[ i ] / ( REAL_NUM_PREC ) numOfColValues[ i ]);    /* Std. Dev. of read in samples */
      }
      else  colSmplStdDev[ i ] = colEstStdDev[ i ] = ( REAL_NUM_PREC ) 0.0;   /* zero if we can't do square root */
    }
    free( colStorage );
  }
  return( numOfCols );
}


/*----------------------------------*/

int  compareRealNumbers( const void *  ptr1, const void *  ptr2 )  {
  REAL_NUM_PREC *  dp1;
  REAL_NUM_PREC *  dp2;

  dp1 = ( REAL_NUM_PREC * ) ptr1;
  dp2 = ( REAL_NUM_PREC * ) ptr2;
  if ( *dp1 > *dp2 )  return( 1 );
  else if ( *dp2 > *dp1 )  return( -1 );
  return 0;
}


/*----------------------------------*/

int  calcMedians( int  numOfCols, size_t  numOfColValues[], REAL_NUM_PREC  store[], REAL_NUM_PREC *  colMedians )  {
  int  i;
  size_t  j;
  REAL_NUM_PREC *  colStorage;
  REAL_NUM_PREC *  storagePtr;
  REAL_NUM_PREC *  ptr;

  /* find longest column, just in case they are not all the same lengths */
  for( i = 0, j = 0; i < numOfCols; i++ )  if ( numOfColValues[ i ] > j )  j = numOfColValues[ i ];   /* find biggest */
  /* ensure we allocate enough temporary storage for the longest column */
  if (( colStorage = calloc( j, SIZEOF_REAL_NUM )) == NULL )
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
        printf( "\nUnsorted column %d ( %lu values)\n", i, numOfColValues[ i ]);
        for( j = 0; j < numOfColValues[ i ]; j++ )  {
          printf( "%lu: ", j );
          printNumberAndSeparator( colStorage[ j ], "\n");
        }
      }
#endif
      /* sort the column */
      qsort( colStorage, numOfColValues[ i ], SIZEOF_REAL_NUM, compareRealNumbers );
#ifdef  DEBUG
      if ( debugFlag )  {
        printf( "Sorted column %d ( %lu values)\n", i, numOfColValues[ i ]);
        for( j = 0; j < numOfColValues[ i ]; j++ )  {
          printf( "%lu: ", j );
          printNumberAndSeparator( colStorage[ j ], "\n");
        }
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

void  printULongAndSeparator( unsigned long  number, char *  separator )  {
  printf( "%lu", number );
  if (( separator != NULL ) || ( *separator != '\0' ))  printf( "%s", separator );
}


/*----------------------------------*/

void  printColumnsStatsInRows( int  numOfColumns, size_t  numOfValues[], char *  separatorStr, int  oneRowFlag )  {
  int  i;

  /* all columns Stats except the most righthand one need the separator */
  for( i = 0; i < numOfColumns; i++ )  {
    printf( "%d, %lu, ", i + 1, numOfValues[ i ] );  /* column number, count, assume at least one column stat to print */
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
      printNumberAndSeparator( columnsEstPopStdDev[ i ], "" );
    }
    /* if oneRowFlag is active then continue using separator until the last column stats are printed */
    if ( oneRowFlag && (( i + 1 ) < numOfColumns ))  printf( "%s", separatorStr );
    else  printf( "\n" );
  }
}


/*----------------------------------*/

void  printColumnsStatsLabelsInRows( char *  separatorStr )  {
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
    printf( "\"Est. Pop. Std. Dev.\"" );
  }
}


/*----------------------------------*/

void  printColumnsStatsInColumns( int  numOfColumns, size_t  numOfValues[], char *  separatorStr, int verbose )  {
  int  i;

  /* all columns Stats except the most righthand one need the separator */
  if ( verbose || headerFlag )  printf( "\"Count\", " );
  for( i = 0; i < numOfColumns; i++ )
    printULongAndSeparator( numOfValues[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
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
    if ( verbose || headerFlag )  printf( "\"Sample Std. Dev.\", " );
    for( i = 0; i < numOfColumns; i++ )  {
      printNumberAndSeparator( columnsSampleStdDev[ i ], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
    }
    if ( verbose || headerFlag )  printf( "\"Est. Pop. Std. Dev.\", " );
    for( i = 0; i < numOfColumns; i++ )  {
      printNumberAndSeparator( columnsEstPopStdDev[ i ], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
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

void  outputStatsInRowForm( int  numOfColumns )  {
  int  i;

  /* print all column stats in just one line (i.e. just one row ) */
  if ( verboseFlag || headerFlag )  {
    for( i = 0; i < numOfColumns; i++ )  {
      printColumnsStatsLabelsInRows( ", " );
      if (( i + 1 ) < numOfColumns )  printf( ", " );   /* add separator up to the penultimate column */
    }
    printf( "\n" );
  }
  printColumnsStatsInRows( numOfColumns, columnsSizes, ", ", TRUE );
  if( lineModeFlag )  printf( "\n" );   /* output a blank line if in line mode */
  /* print column stats in row form (i.e. one line or row per column ) */
  if ( verboseFlag || headerFlag )  {
    printColumnsStatsLabelsInRows( ", " );
    printf( "\n" );
  }
  printColumnsStatsInRows( numOfColumns, columnsSizes, ", ", FALSE );
  if( lineModeFlag )  printf( "\n" );   /* output a blank line if in line mode */
}


/*----------------------------------*/

void  outputStatsInColumnForm( int  numOfColumns )  {
  int  i;

  /* print column stats in column form */
  if ( verboseFlag || headerFlag )  {
    printf( "\"Stat. Type\", " );
    for( i = 1; i < numOfColumns; i++ )  printf( "\"Column %d\", ", i );
    printf( "\"Column %d\"\n", numOfColumns );
  }
  printColumnsStatsInColumns( numOfColumns, columnsSizes, ", ", verboseFlag );
}


/*----------------------------------*/

int  main( int argc, char * argv[])  {
  int  i, numOfColumns, farg;
  int  exitFlag;
  long  lineNumber;
  size_t  totalNumbers, storageSize;
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
      if (( storage = malloc( storageSize * SIZEOF_REAL_NUM )) == NULL )  {
        fprintf( stderr, "Error: unable to allocate memory to store %ld csv numbers: Aborting\n", storageSize );
      }
      else  {
        while( fgets( bfr, BFR_SIZE - 1, fp ) != NULL )  {
          if (( *bfr == '\0' ) || ( *bfr == '#' ) || ( *bfr == '\r' ) || ( *bfr == '\n' ))  {   /* skip comments or blank lines */
            if ( debugFlag )  printf( "Debug: Warning: skipping a line (blank or comment) - %s", ( *bfr == '#' ) ? bfr : "\n" );
          }
          else  {
            lineNumber += 1;
            if( lineNumber == (long) 1 )  {
              if ( debugFlag )  printf( "Debug: %ld: %s", lineNumber, bfr );
	            totalNumbers = numOfColumns = processFirstLineOfNumbers( bfr );
              if( debugFlag || verboseFlag )
                printf( "Debug: %d Column(s) found in the first line of numbers.\n", numOfColumns );
            }
    	      else  {
              if(( i = processLine( bfr, totalNumbers )) != numOfColumns )  {
                if ( verboseFlag )
                  printf( "Warning: %d values in line %ld doesn't match the expected %d values found in the first line\n",
                    i, lineNumber, numOfColumns );
              }
              totalNumbers += i;  /* keep track of how many numbers in total are in the storage */
              if ( debugFlag )  printf( "%lu total numbers so far - %s", totalNumbers, bfr );
              if ( totalNumbers >= ( storageSize - numOfColumns ))  {
                if ( debugFlag )
                  printf( "Warning: Storage for csv numbers is nearly full (%lu): allocating more storage\n", totalNumbers );
                storageSize += COMP_NUMBER;    /* increase storage size to avoid overflow */
                if (( storage = realloc( storage, storageSize * SIZEOF_REAL_NUM )) == NULL ) {
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
        columnsMeans[ i ] = columnsSums[ i ] / ( REAL_NUM_PREC ) columnsSizes[ i ];
      if ( calcStdDev( numOfColumns, columnsSizes, storage, columnsEstPopStdDev, columnsSampleStdDev, columnsMeans ) != numOfColumns )
        printf( "Warning: unable to calulate Std. Dev. values\n" );
      /* Re-calculate means after use in the Std. Dev. calcs & then calc Pearson's measure of skewness */
      for( i = 0; i < numOfColumns; i++ )  {
        columnsMeans[ i ] = columnsSums[ i ] / ( REAL_NUM_PREC ) columnsSizes[ i ];
        if (( columnsEstPopStdDev[ i ] == ( REAL_NUM_PREC ) +0.0 ) || ( columnsEstPopStdDev[ i ] == ( REAL_NUM_PREC ) -0.0 ))  {
          printf( "Warning: Pearson's Skewness cannot be calculated when Std. Dev. is zero\n" );
          columnsPearsonSkew[ i ] =  ( REAL_NUM_PREC ) 0.0;
        }
        else
          columnsPearsonSkew[ i ] = ( REAL_NUM_PREC ) 3 * ( columnsMeans[ i ] - columnsMedians[ i ]) / columnsEstPopStdDev[ i ];
      }
      /* Print out the stats in various forms of csv format */
      if( lineModeFlag )  printf( "\n" );   /* output a blank line if in line mode */
      if ( verboseFlag )  outputStatsInRowForm( numOfColumns );   /* only output stats in row form in vebose mode */
      outputStatsInColumnForm( numOfColumns );
#ifdef  DEBUG
      if ( debugFlag )  {
        printf( "\nDebug: csv storage listing\n" );
        for( i = 0; ( size_t ) i < totalNumbers; i++ )  {
          printf( "%d : ", i );
          printNumberAndSeparator( storage[ i ], "\n" );
        }
      }
#endif
      if ( storage != NULL )  free( storage );
    }
  }
  if ( lineModeFlag )  printf( "\n" );   /* output an extra line feed */
  if ( helpFlag )  usage();
  return( exitFlag );
}
