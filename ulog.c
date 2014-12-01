
/*************************************************************************

	Universita' degli Studi dell'Insubria

    CREATED : 13/03/2007
    AUTHOR  : Moreno Carullo - moreno.carullo@uninsubria.it
    FILE    : Micro-logging C library
	
	$Id: ulog.c 94 2007-12-21 16:28:27Z moreno $
	
	(!)  All functions are reported with almost no comment on input params.
	     Please refer to ulog.h for a complete Doxygen-able documentation.
	     Please see the doxygen directory for an automated Makefile.

 **************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "ulog.h"

static ulog_level_t ulog_minlevel = ULOG_ALL;

/** Set the loglevel

  ulog_setlevel -- Author(s): M.Carullo (100%)

*/
void ulog_setlevel(ulog_level_t level)
{
  ulog_minlevel = level;
}

/** Can i log with this level?

  ulog_canlog -- Author(s): M.Carullo (100%)

*/
int ulog_canlog(ulog_level_t level)
{
  return (level <= ulog_minlevel ) ? 1 : 0;
}

/** Log a message

  ulog_log -- Author(s): M.Carullo (100%)

*/
void ulog_log(ulog_level_t level, char *msg, ...)
{
  va_list fmt;
  char buff[1024];

  if( !ulog_canlog(level)) return;

  va_start(fmt,msg);
  vsnprintf(buff, sizeof(buff)-1, msg, fmt);
  va_end(fmt);

  switch(level) {
    case ULOG_ERROR:
      fprintf(stdout, "[error] ");
      break;
    case ULOG_INFO:
      fprintf(stdout, "[info] ");
      break;
    case ULOG_WARNING:
      fprintf(stdout, "[warning] ");
      break;
    case ULOG_DEBUG:
      fprintf(stdout, "[debug] ");
      break;
    default:
      break;
  }

  fprintf(stdout, "%s", buff);
}
