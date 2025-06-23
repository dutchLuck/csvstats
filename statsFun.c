/*
 *  S T A T S F U N . C
 *
 * last modified on Mon Jun 23 11:04:19 2025, by O.F.H.
 *
 * written by O.Holland
 *
 * Added ability to use alternate column separator to comma
 * Fixed compile error when -DDEBUG is used
 * 
 */

#include <stdio.h>
#include <stdlib.h>		/* exit() */
#include <string.h>		/* strlen() */
#include <math.h>		/* sqrtl(), sqrt(), sqrtf() */
#include "config.h"		/* struct config *  cfg */
#include "statsFun.h"	/*  */

#define  MAX_NUM_OF_COLS  100
#define  BFR_SIZE  15*MAX_NUM_OF_COLS
#define  COMP_NUMBER  2520

/* Set up default Floating Point Precision to double */
#ifndef  LDBL_PREC
#ifndef  FLT_PREC
#define  DBL_PREC 1
#endif
#endif

/* Handle different precisions for floating point numbers */
/* - This is clunky old school but it works - rewrite in C++ ? */
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

/*----------------------------------*/

int  processFirstLineOfNumbers( char *  linePtr, int  delimiterChr )  {
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
	  if( result == 1 )  columnsSizes[ cnt++ ] = 1;  /* Only increment if successful */
	  *storagePtr++ = *sumsPtr;     /* put new value in storage */
	  *mostPosPtr++  = *sumsPtr;
	  *mostNegPtr++  = *sumsPtr++;
	  if ( delimiterChr == '\0' )  {	/* Can't handle delimiter being '\0' as it also means end-of-line */
		linePtr += strcspn( linePtr, ",;\t" );		/* Try shifting pionter to the most likely separators if NULL is specified */
		if ( *linePtr == '\0' )  linePtr = NULL;	/* if nothing was found except end-of-line then set to NULL */
	  }
	  else  {
		linePtr = strchr( linePtr, delimiterChr );
	  }
	  if( linePtr != NULL )  linePtr += 1;	/* step over delimiter (i.e. comma) */
	} while(( linePtr != NULL ) && ( cnt < MAX_NUM_OF_COLS )); 
	return( cnt );
}
  
  
/*----------------------------------*/
  
int  processLine( char *  linePtr, int  delimiterChr, int  numberOfColumns )  {
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
	  if ( delimiterChr == '\0' )  {	/* Can't handle NULL as it also means end-of-line */
		linePtr += strcspn( linePtr, ",;\t" );	/* Try the most likely separators if NULL is specified */
		if ( *linePtr == '\0' )  linePtr = NULL;	/* if nothing was found except end-of-line then set to NULL */
	  }
	  else  {
		linePtr = strchr( linePtr, delimiterChr );
	  }
	  if( linePtr != NULL )  linePtr += 1;	/* step over delimiter (i.e. comma)  */
	} while(( linePtr != NULL ) && ( cnt < numberOfColumns )); 
	return( cnt );
}


/*----------------------------------*/

void  printLongAndSeparator( FILE *  ofp, long  number, char *  separator )  {
	fprintf( ofp, "%ld", number );
	if (( separator != NULL ) || ( *separator != '\0' ))  fprintf( ofp, "%s", separator );
}


/*----------------------------------*/

void  printULongAndSeparator( FILE *  ofp, unsigned long  number, char *  separator )  {
	fprintf( ofp, "%lu", number );
	if (( separator != NULL ) || ( *separator != '\0' ))  fprintf( ofp, "%s", separator );
}


/*----------------------------------*/
  
void  printNumberAndSeparator( FILE *  ofp, REAL_NUM_PREC  number, char *  separator )  {
	fprintf( ofp, WRT_OUT_FMT, WRT_OUT_DIG, number );   /* use precision specified by macro in float.h */
	if (( separator != NULL ) || ( *separator != '\0' ))  fprintf( ofp, "%s", separator );
}


/*----------------------------------*/

void  printDebugStats( FILE *  ofp, int colnum, REAL_NUM_PREC  ave, REAL_NUM_PREC  adev, REAL_NUM_PREC  sdev, REAL_NUM_PREC  var,
	REAL_NUM_PREC  skew, REAL_NUM_PREC  curt )  {
  
	fprintf( ofp, "Debug: Col %d, ", colnum );
	fprintf( ofp, "Av. " );
	printNumberAndSeparator( ofp, ave, ", " );
	fprintf( ofp, "ADev. " );
	printNumberAndSeparator( ofp, adev, ", " );
	fprintf( ofp, "SDev. " );
	printNumberAndSeparator( ofp, sdev, ", " );
	fprintf( ofp, "Var. " );
	printNumberAndSeparator( ofp, var, ", " );
	fprintf( ofp, "Skew " );
	printNumberAndSeparator( ofp, skew, ", " );
	fprintf( ofp, "Kurt. " );
	printNumberAndSeparator( ofp, curt, ", " );
	fprintf( ofp, "\n" );
}


/*----------------------------------*/

int  calcStdDev( struct config *  cfg, FILE *  ofp, int  numOfCols, size_t  numOfColValues[], REAL_NUM_PREC  store[],
	REAL_NUM_PREC *  colEstStdDev, REAL_NUM_PREC *  colSmplStdDev, REAL_NUM_PREC *  colMeans )  {
	int  i;
	size_t j;
	REAL_NUM_PREC *  colStorage;
	REAL_NUM_PREC *  storagePtr;
	REAL_NUM_PREC *  ptr;
#ifdef DEBUG
	REAL_NUM_PREC  ave, adev, sdev, var, skew, curt;
#endif
  
	if ( cfg->D.active )
		fprintf( ofp, "Debug: Calculating Std. Dev. with %s precision (%d byte numbers)\n", REAL_NUM_TYPE_STR, SIZEOF_REAL_NUM );
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
			if ( cfg->D.active )  {
				ave = adev = sdev = var = skew = curt = ( REAL_NUM_PREC ) 0.0;
				printDebugStats( ofp, i, ave, adev, sdev, var, skew, curt );
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
			if ( cfg->D.active )  {
				ave = adev = sdev = var = skew = curt = ( REAL_NUM_PREC ) 0.0;
				printDebugStats( ofp, i, ave, adev, sdev, var, skew, curt );
			}
			if ( cfg->D.active )  {
				printf( "\nDebug: column %d centred values\n", i );
				for( j = 0; j < numOfColValues[ i ]; j++ )  {
					printf( "%ld: ", j );
					printNumberAndSeparator( ofp, colStorage[ j ], "\n");
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
  
int  calcMedians( struct config *  cfg, FILE *  ofp, int  numOfCols, size_t  numOfColValues[], REAL_NUM_PREC  store[], REAL_NUM_PREC *  colMedians )  {
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
			if ( cfg->D.active )  {
				printf( "\nUnsorted column %d ( %lu values)\n", i, numOfColValues[ i ]);
				for( j = 0; j < numOfColValues[ i ]; j++ )  {
					printf( "%lu: ", j );
					printNumberAndSeparator( ofp, colStorage[ j ], "\n");
				}
			}
#endif
			/* sort the column */
			qsort( colStorage, numOfColValues[ i ], SIZEOF_REAL_NUM, compareRealNumbers );
			if ( cfg->D.active )  {
				printf( "Debug: calc medians: Sorted column %d ( %lu values)\n", i, numOfColValues[ i ]);
				for( j = 0; j < numOfColValues[ i ]; j++ )  {
					printf( "Debug: %lu: ", j );
					printNumberAndSeparator( ofp, colStorage[ j ], "\n");
				}
			}
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

void  printColumnsStatsInRows( struct config *  cfg, FILE *  ofp, int  numOfColumns, size_t  numOfValues[], char *  separatorStr, int  oneRowFlag )  {
	int  i;
  
	/* all columns Stats except the most righthand one need the separator */
	for( i = 0; i < numOfColumns; i++ )  {
	  printf( "%d, %lu, ", i + 1, numOfValues[ i ] );  /* column number, count, assume at least one column stat to print */
	  printNumberAndSeparator( ofp, columnsSums[ i ], ( !cfg->A.active || !cfg->M.active || !cfg->P.active ||
		!cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	  if( !cfg->M.active )
		printNumberAndSeparator( ofp, columnsMedians[ i ], ( !cfg->A.active || !cfg->P.active ||
			!cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	  if( !cfg->A.active )
		printNumberAndSeparator( ofp, columnsMeans[ i ], ( !cfg->P.active ||
			!cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	  if( !cfg->P.active )
		printNumberAndSeparator( ofp, columnsMostPos[ i ], ( !cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	  if( !cfg->N.active )
		printNumberAndSeparator( ofp, columnsMostNeg[ i ], ( !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	  if( !cfg->R.active )
		printNumberAndSeparator( ofp, columnsMostPos[ i ] - columnsMostNeg[i], ( !cfg->S.active ) ? separatorStr : "" );
	  if( !cfg->S.active )  {
		printNumberAndSeparator( ofp, columnsEstPopStdDev[ i ], "" );
	  }
	  /* if oneRowFlag is active then continue using separator until the last column stats are printed */
	  if ( oneRowFlag && (( i + 1 ) < numOfColumns ))  printf( "%s", separatorStr );
	  else  printf( "\n" );
	}
}


/*----------------------------------*/

void  printColumnsStatsLabelsInRows( struct config *  cfg, char *  separatorStr )  {
	/* all columns Stats except the most righthand one need the separator */
	printf( "\"Column Number\", \"Count\", " );  /* column number, count, assume at least one column stat to print */
	printf( "\"Sum\"%s", ( !cfg->M.active || !cfg->A.active || !cfg->P.active ||
		!cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	if( !cfg->M.active )
	  printf( "\"Median\"%s", ( !cfg->A.active || !cfg->P.active ||
		!cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	if( !cfg->A.active )
	  printf( "\"Mean\"%s", ( !cfg->P.active || !cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	if( !cfg->P.active )
	  printf( "\"Most +ve\"%s", ( !cfg->N.active || !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	if( !cfg->N.active )
	  printf( "\"Most -ve\"%s", ( !cfg->R.active || !cfg->S.active ) ? separatorStr : "" );
	if( !cfg->R.active )
	  printf( "\"Range\"%s", ( !cfg->S.active ) ? separatorStr : "" );
	if( !cfg->S.active )  {
	  printf( "\"Est. Pop. Std. Dev.\"" );
	}
}


/*----------------------------------*/

void  printColumnsStatsInColumns( struct config *  cfg, FILE *  ofp, int  numOfColumns, size_t  numOfValues[], char *  separatorStr )  {
	int  i;
  
	/* all columns Stats except the most righthand one need the separator */
	if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Count\", " );
	for( i = 0; i < numOfColumns; i++ )
	  printULongAndSeparator( ofp, numOfValues[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	if( !cfg->N.active )  {
	  if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Most -ve\", " );
	  for( i = 0; i < numOfColumns; i++ )
		printNumberAndSeparator( ofp, columnsMostNeg[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	}
	if( !cfg->M.active )  {
	  if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Median\", " );
	  for( i = 0; i < numOfColumns; i++ )
		printNumberAndSeparator( ofp, columnsMedians[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	}
	if( !cfg->P.active )  {
	  if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Most +ve\", " );
	  for( i = 0; i < numOfColumns; i++ )
		printNumberAndSeparator( ofp, columnsMostPos[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	}
	if( !cfg->R.active )  {
	  if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Range\", " );
	  for( i = 0; i < numOfColumns; i++ )
		printNumberAndSeparator( ofp, columnsMostPos[i] - columnsMostNeg[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	}
	if( !cfg->A.active )  {
	  if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Mean\", " );
	  for( i = 0; i < numOfColumns; i++ )
		printNumberAndSeparator( ofp, columnsMeans[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	}
	if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Sum\", " );
	for( i = 0; i < numOfColumns; i++ )
	  printNumberAndSeparator( ofp, columnsSums[i], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	if( !cfg->S.active )  {
		if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Sample Std. Dev.\", " );
		for( i = 0; i < numOfColumns; i++ )  {
			printNumberAndSeparator( ofp, columnsSampleStdDev[ i ], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
		}
		if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Est. Pop. Std. Dev.\", " );
		for( i = 0; i < numOfColumns; i++ )  {
			printNumberAndSeparator( ofp, columnsEstPopStdDev[ i ], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
		}
	}
	if( !cfg->p.active )  {
	  if ( cfg->v.active || cfg->H.active )  fprintf( ofp, "\"Pearson's Skew\", " );
	  for( i = 0; i < numOfColumns; i++ )  {
		printNumberAndSeparator( ofp, columnsPearsonSkew[ i ], ( i < ( numOfColumns - 1 )) ? separatorStr : "\n" );
	  }
	}
}


/*----------------------------------*/

void  outputStatsInRowForm( struct config *  cfg, FILE *  ofp, int  numOfColumns )  {
	int  i;

	/* print all column stats in just one line (i.e. just one row ) */
	if ( numOfColumns > 1 )  {
		if ( cfg->v.active || cfg->H.active ) {
			for( i = 0; i < numOfColumns; i++ )  {
				printColumnsStatsLabelsInRows( cfg, ", " );
				if (( i + 1 ) < numOfColumns )  printf( ", " );   /* add separator up to the penultimate column */
			}
			printf( "\n" );
		}
		printColumnsStatsInRows( cfg, ofp, numOfColumns, columnsSizes, ", ", TRUE );
		if( cfg->n.active )  printf( "\n" );   /* output a blank line if in line mode */
	}
	/* print column stats in row form (i.e. output one line or row per column ) */
	if ( cfg->v.active || cfg->H.active )  {
		printColumnsStatsLabelsInRows( cfg, ", " );
		printf( "\n" );
	}
	printColumnsStatsInRows( cfg, ofp, numOfColumns, columnsSizes, ", ", FALSE );
}


/*----------------------------------*/

void  outputStatsInColumnForm( struct config *  cfg, FILE *  ofp, int  numOfColumns )  {
	int  i;
  
	/* print column stats in column form */
	if ( cfg->v.active || cfg->H.active )  {
		fprintf( ofp, "\"Stat. Type\", " );
		for( i = 1; i < numOfColumns; i++ )  fprintf( ofp, "\"Column %d\", ", i );
		fprintf( ofp, "\"Column %d\"\n", numOfColumns );
	}
	printColumnsStatsInColumns( cfg, ofp, numOfColumns, columnsSizes, ", " );
}


size_t  readInput( struct config *  cfg, FILE *  fp, int *  numOfColumns )  {
	int  i;
	size_t  lineNumber, totalNumbers, storageSize;
	char  bfr[ BFR_SIZE ];

	lineNumber = totalNumbers = (size_t) 0;
	storageSize = (size_t) COMP_NUMBER;
	if (( storage = malloc( storageSize * SIZEOF_REAL_NUM )) == NULL )  {
        fprintf( stderr, "Error: unable to allocate memory to store %ld csv numbers: Aborting\n", storageSize );
    }
    else  {
		while( fgets( bfr, BFR_SIZE - 1, fp ) != NULL )  {
			if (( *bfr == '\0' ) || ( *bfr == cfg->c.optionChr ) || ( *bfr == '\r' ) || ( *bfr == '\n' ))  {   /* skip comments or blank lines */
				if ( cfg->D.active )  printf( "Debug: Warning: skipping a line (blank or comment) - %s", ( *bfr == '#' ) ? bfr : "\n" );
			}
			else  {
				lineNumber += 1;
				if( lineNumber == (size_t) 1 )  {
					if ( cfg->D.active )  printf( "Debug: Line Number: %lu, Line String: \"%s\"", lineNumber, bfr );
					*numOfColumns = processFirstLineOfNumbers( bfr, cfg->d.optionChr );
					totalNumbers = (size_t) *numOfColumns;
					if( cfg->D.active )
						printf( "Debug: %d Column(s) found in the first line of numbers.\n", *numOfColumns );
				}
				else  {
					if(( i = processLine( bfr, cfg->d.optionChr, totalNumbers )) != *numOfColumns )  {
						if ( cfg->v.active )
							printf( "Warning: %d values in line %ld doesn't match the expected %d values found in the first line\n",
								i, lineNumber, *numOfColumns );
					}
					totalNumbers += (size_t) i;  /* keep track of how many numbers in total are in the storage */
					if ( cfg->D.active )  printf( "Debug: %lu total numbers so far - \"%s\"", totalNumbers, bfr );
					if ( totalNumbers >= ( storageSize - *numOfColumns ))  {
						if ( cfg->D.active )
							printf( "Warning: Storage for csv numbers is nearly full (%lu): allocating more storage\n", totalNumbers );
						storageSize += COMP_NUMBER;    /* increase storage size to avoid overflow */
						if (( storage = realloc( storage, storageSize * SIZEOF_REAL_NUM )) == NULL ) {
							fprintf( stderr, "Error: unable to get more memory to store more csv numbers at input line %ld: Aborting\n", lineNumber );
							return( 0 );
						}
					}
				}
			}
		}
	}
	return( totalNumbers );
}


void  writeOutput( struct config *  cfg, FILE *  ofp, int  numOfColumns )  {
	int  i;

	if ( calcMedians( cfg, ofp, numOfColumns, columnsSizes, storage, columnsMedians ) != numOfColumns )
		fprintf( ofp, "Warning: unable to calulate median values\n" );
	/* Calculate means for use in the Std Dev calcs */
	for( i = 0; i < numOfColumns; i++ )
		columnsMeans[ i ] = columnsSums[ i ] / ( REAL_NUM_PREC ) columnsSizes[ i ];
	if ( calcStdDev( cfg, ofp, numOfColumns, columnsSizes, storage, columnsEstPopStdDev, columnsSampleStdDev, columnsMeans ) != numOfColumns )
		fprintf( ofp, "Warning: unable to calulate Std. Dev. values\n" );
	/* Re-calculate means after use in the Std. Dev. calcs & then calc Pearson's measure of skewness */
	for( i = 0; i < numOfColumns; i++ )  {
		columnsMeans[ i ] = columnsSums[ i ] / ( REAL_NUM_PREC ) columnsSizes[ i ];
		if (( columnsEstPopStdDev[ i ] == ( REAL_NUM_PREC ) +0.0 ) || ( columnsEstPopStdDev[ i ] == ( REAL_NUM_PREC ) -0.0 ))  {
			fprintf( ofp, "Warning: Pearson's Skewness cannot be calculated when Std. Dev. is zero\n" );
			columnsPearsonSkew[ i ] =  ( REAL_NUM_PREC ) 0.0;
		}
		else
			columnsPearsonSkew[ i ] = ( REAL_NUM_PREC ) 3 * ( columnsMeans[ i ] - columnsMedians[ i ]) / columnsEstPopStdDev[ i ];
	}
	/* Print out the stats in various forms of csv format */
	if ( cfg->r.active )  outputStatsInRowForm( cfg, ofp, numOfColumns );   /* only output stats in row form in row mode */
	else  outputStatsInColumnForm( cfg, ofp, numOfColumns );	/* output stats in column mode, unless in row mode */
	if ( storage != NULL )  free( storage );
}


/* processInToOut - returns a negetive number if an error occurs */
/* otherwise it returns the number of characters processed */

int  processInToOut( struct config *  cfg, FILE *  fp, FILE *  ofp )  {
	int  numberOfColumns = 0;
	size_t  numbersProcessed = 0;
	int  errorIndicator = 1;			/* Assume no error at the start */

	if (( numbersProcessed = readInput( cfg, fp, &numberOfColumns )) > 0 )  {
		writeOutput( cfg, ofp, numberOfColumns );
		if( cfg->n.active )  printf( "\n" );   /* output a blank line if new line mode is active */
	}
	if ( ! feof( fp ))  perror( "Error when reading input file" );
	fflush( ofp );
	fflush( stderr );
	return((int)( errorIndicator * numbersProcessed ));
}
