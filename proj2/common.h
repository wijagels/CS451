/* This file defines some helper macros that may be helpful. */

#ifndef _COMMON_H
#define _COMMON_H

/* This snippet enables the use of __func__ instead of the less-portable
 * __FUNCTION__. This allows me to turn on -pedantic-errors.
 * https://gcc.gnu.org/onlinedocs/gcc-4.5.3/gcc/Function-Names.html */
#if __STDC_VERSION__ < 199901L
#if __GNUC__ >= 2
#define __func__ __FUNCTION__
#else
#define __func__ "<unknown>"
#endif
#endif

#define SUCCESS 0
#define ERROR -1

#define US_PER_SEC 1000000

#define DEBUG
#ifdef DEBUG
#define DEBUG_MSG(fmt, args...) \
  fprintf(stderr, "%s(): \t" fmt, __func__, ##args)
#else
#define DEBUG_MSG(fmt, args...)
#endif

#define ERR_MSG(fmt, args...) \
  fprintf(stderr, "ERROR in %d:%s(): " fmt, __LINE__, __func__, ##args)

#define EXIT_ERROR(v, fmt, args...) \
  do {                              \
    ERR_MSG(fmt, ##args);           \
    exit(v);                        \
  } while (0)

#define _EXIT_ERROR(v, fmt, args...) \
  do {                               \
    ERR_MSG(fmt, ##args);            \
    _exit(v);                        \
  } while (0)

#endif
