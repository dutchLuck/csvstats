/*
 *  S T A T S F U N . H
 *
 * last modified on Thu May 30 13:04:33 2024, by O.F.H.
 *
 * written by O.Holland
 *
 */


#ifndef STATSFUN_H
#define STATSFUN_H

#include <stdio.h>
#include "config.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!(FALSE))
#endif

int  processInToOut( struct config *  cfg, FILE *  fp, FILE *  ofp );

#endif
