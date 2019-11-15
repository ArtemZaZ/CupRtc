#include "os_support.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifndef OVERRIDE_SPEEX_PUTC
static inline void _speex_putc(int ch, void *file)
{
   FILE *f = (FILE *)file;
   fprintf(f, "%c", ch);
}
#endif


#ifndef OVERRIDE_SPEEX_FATAL
static inline void _speex_fatal(const char *str, const char *file, int line)
{
   fprintf (stderr, "Fatal (internal) error in %s, line %d: %s\n", file, line, str);
   exit(1);
}
#endif

