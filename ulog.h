/*************************************************************************

	Universita' degli Studi dell'Insubria

    CREATED : 13/03/2007
    AUTHOR  : Moreno Carullo - moreno.carullo@uninsubria.it
    FILE    : Micro-logging C library header
	
	$Id: ulog.h 94 2007-12-21 16:28:27Z moreno $

	(!)  All functions are reported with almost no comment on input params.
	     Please refer to sfosart.h for a complete Doxygen-able documentation.
	     Please see the doxygen directory for an automated Makefile.

 **************************************************************************/

#ifndef _ulog_h_
#define _ulog_h_

typedef enum { ULOG_ERROR, ULOG_WARNING, ULOG_INFO, ULOG_DEBUG, ULOG_ALL } ulog_level_t;

/** Set the loglevel
    \param level desired logging level
*/
void ulog_setlevel(ulog_level_t level);

/** Can i log with this level?
    \param level desired logging level
*/
int ulog_canlog(ulog_level_t level);

/** Log a message
    \param level desired loglevel
    \param msg message to log, plus formatting args
*/
void ulog_log(ulog_level_t level, char *msg, ...);

#endif
