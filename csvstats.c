/*
 * C S V S T A T S . C
 *
 * Last Modified on Sat Jun 28 22:18:41 2025
 *
 * Read comma separated values from stdin or files
 * and calculate basic statistics like arithmetic
 * mean, median and standard deviation on the
 * values.
 *
 *
 * 0v7 Added decimal place control and broke -d previous use, now -C
 * 0v6 Added ability to skip lines at the start of data streams
 * 0v5 Added ability to use alternate column separator to comma
 *
 */


#include <stdio.h>		/* printf() */
#include <stdlib.h>		/* free() */
#include <string.h>		/* strdup() */
#include <libgen.h>		/* basename() */
#include "config.h"		/* struct config */
#include "statsFun.h" /* processInToOut() */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!(FALSE))
#endif

#define  NAME_VERSION  "csvstats 0v7 (2025-06-28)"

char *  exePath = NULL;
char *  exeName = NULL;

int  processNonConfigCommandLineParameters( struct config *  cfg, int  frstIndx, int  lstIndx, char *  cmdLnStrngs[] );
void  setExecutableName( char *  argv[] );


void  cleanupStorage( void )  {
	if( exePath != NULL )  free(( void *) exePath );
}


int  main (int argc, char *  argv[])  {
  struct config  config;
  int indexToFirstNonConfig;
  int index;

  /* Ensure any allocated memory is free'd */
	atexit( cleanupStorage );
  /* setup the name of this program */
  setExecutableName( argv );
  /* set defaults for all configuration options */
  initConfiguration( &config );
  /* set any configuration options that have been specified in the command line */
  indexToFirstNonConfig = setConfiguration( argc, argv, &config );

  /* if -V specified then show version information */
  if ( config.V.active || config.D.active )  printf( "%s\n", NAME_VERSION );

  /* if -h specified then show the help/usage information and finish */
  if ( config.D.active ) {
    configuration_status( &config );  /* show status of each configuration option */
    for ( index = indexToFirstNonConfig; index < argc; index++)
      printf ("Debug: Non-option argument ( %d ): \"%s\"\n", index, argv[index]);
    fflush( stdout );
  }
  /* Print the help and usage information */
  if ( config.h.active || config.V.active )  {
    if ( config.h.active )  usage( &config, exeName );
  }
	else  {
  	/* Attempt to dump Hex and Ascii and return 0 if successful */
    return( processNonConfigCommandLineParameters( &config, indexToFirstNonConfig, argc - 1, argv ));
  }
  return 0;
}


void  setExecutableName( char *  argv[] ) {
/* Isolate the name of the executable */
  if(( exePath = strdup( argv[0] )) == NULL )
    perror( "Warning: Unable to create duplicate of path to this executable" );
  else if(( exeName = basename( exePath )) == NULL )  {
    perror( "Warning: Unable to obtain the name of this executable" );
  }
  if ( exeName == NULL )  exeName = argv[ 0 ];
}


int  processA_SingleCommandLineParameter( struct config *  cfg, FILE *  ofp, char *  nameStrng ) {
	int  returnVal;
	long  processCnt;
	FILE *  fp;

/* Open the file for reading */
	returnVal = (( fp = fopen( nameStrng, "r" )) == NULL );
	if( returnVal )  {
		fprintf( ofp, "Warning: Unable to open a file named \"%s\" for reading\n", nameStrng );
		perror( nameStrng );
	}
	else  {	/* Begin file opened ok block */
	  if( cfg->D.active || cfg->v.active )
    	fprintf( ofp, "%sInfo: Start of processing file \"%s\"\n", ( cfg->n.active ) ? "\n" : "", nameStrng );
		processCnt = processInToOut( cfg, fp, ofp );   /* if process count returned is negetive then an error occurred */
	  /* Close the file just processed */
		returnVal = ( fclose( fp ) != 0 );
	  if( returnVal )  {
  		fprintf( ofp, "Warning: Unable to close the file named \"%s\"\n", nameStrng );
			perror( nameStrng );
		}
    if( processCnt < 0 ) {
      processCnt *= -1;    /* convert error indication to a positive count */
      returnVal = TRUE;
    } 
	  if( cfg->D.active || cfg->v.active )
    	fprintf( ofp, "%sInfo: End of processing file \"%s\" (%ld data items)\n", ( cfg->n.active ) ? "\n" : "", nameStrng, processCnt );
	}	/* End of file opened ok block */
	return( returnVal );
}


int  processNonConfigCommandLineParameters( struct config *  cfg, int  frstIndx, int  lstIndx, char *  cmdLnStrngs[] )  {
  int  returnVal;	/* returnVal is incremented for each error */
  int  result, indx;
  int  chrCnt;
  FILE *  ofp = stdout;

#ifdef DEBUG
  if( cfg->D.active )  {
    printf( "Debug: Executing: processNonSwitchCommandLineParameters()\n" );
    printf( "Debug: first index is %d and last index is %d\n", frstIndx, lstIndx );
    for( indx = frstIndx; indx <= lstIndx; indx++ )
      printf( "Debug: cmdLnStrngs[ %d ] string is \"%s\"\n", indx, cmdLnStrngs[ indx ] );
  }
#endif
  returnVal = ( cfg->o.active && (( cfg->o.optionStr == ( char * ) NULL ) || ( *cfg->o.optionStr == '\0' )));
  if( returnVal )
    fprintf( stderr, "Warning: -o specified, but no output file name specified - aborting\n" );
  else  {
    /* Attempt to open the output file if required */
    returnVal = ( cfg->o.active ) ? (( ofp = fopen( cfg->o.optionStr, "w" )) == NULL ) : 0;
    if( returnVal )  {
      printf( "Warning: Unable to open a file named \"%s\" for writing output- aborting\n", cfg->o.optionStr );
      if( cfg->D.active )  perror( cfg->o.optionStr );
    }
    else  {
      if((( lstIndx + 1 ) == frstIndx ) ||
        (( lstIndx  == frstIndx ) && ( strncmp( cmdLnStrngs[ frstIndx ], "-", 1 ) == 0 )))  {
        /* There are no files specified in the command line or the filename is "-" so process stdin */
        if( cfg->D.active )
          printf( "Debug: Reading data from stdin - mark end of any manually typed input with EOT (i.e. ^D)\n" );
        chrCnt = processInToOut( cfg, stdin, ofp );
        if( cfg->D.active || cfg->v.active )
        printf( "%sInfo: Processed %d data items from stdin\n", ( cfg->n.active ) ? "\n" : "", chrCnt );
      }
      else  {
        /* Process each file specified in the command line */
        for( indx = frstIndx; indx <= lstIndx; indx++ )  {
    			result = processA_SingleCommandLineParameter( cfg, ofp, cmdLnStrngs[ indx ] );
		    	returnVal += result;	/* Count the unsuccessful results */
    			if( cfg->D.active )
            printf( "Debug: Processing \"%s\" returned result %d, returnVal %d\n", cmdLnStrngs[ indx ], result, returnVal );
        }
      }
      /* Close output file specified in the command line, if there was one */
      result = ( cfg->o.active ) ? (fclose( ofp ) != 0) : 0;
      if( result )  {
        fprintf( stderr, "Warning: Attempt to close the output file named \"%s\" failed\n", cfg->o.optionStr );
        if( cfg->D.active )  perror( cfg->o.optionStr );
      }
      returnVal += result;	/* Count any unsuccessful fclose() on output file */
    }
  }
  return( returnVal );
}
