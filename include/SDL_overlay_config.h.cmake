/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _SDL_overlay_config_h
#define _SDL_overlay_config_h

/**
 *  \file SDL_config.h.in
 *
 *  This is a set of defines to configure the SDL features
 */

/* General platform specific identifiers */
#include "SDL_platform.h"

/* C language features */
#cmakedefine const @HAVE_CONST@
#cmakedefine inline @HAVE_INLINE@
#cmakedefine volatile @HAVE_VOLATILE@

/* C datatypes */
/* Define SIZEOF_VOIDP for 64/32 architectures */
#ifdef __LP64__
#define SIZEOF_VOIDP 8
#else
#define SIZEOF_VOIDP 4
#endif

#cmakedefine HAVE_GCC_ATOMICS @HAVE_GCC_ATOMICS@
#cmakedefine HAVE_GCC_SYNC_LOCK_TEST_AND_SET @HAVE_GCC_SYNC_LOCK_TEST_AND_SET@
#cmakedefine HAVE_PTHREAD_SPINLOCK @HAVE_PTHREAD_SPINLOCK@

/* Comment this if you want to build without any C library requirements */
#cmakedefine HAVE_LIBC 1
#if HAVE_LIBC

/* Useful headers */
#cmakedefine HAVE_ALLOCA_H 1
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_STDIO_H 1
#cmakedefine STDC_HEADERS 1
#cmakedefine HAVE_STDLIB_H 1
#cmakedefine HAVE_STDARG_H 1
#cmakedefine HAVE_MALLOC_H 1
#cmakedefine HAVE_MEMORY_H 1
#cmakedefine HAVE_STRING_H 1
#cmakedefine HAVE_STRINGS_H 1
#cmakedefine HAVE_INTTYPES_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_CTYPE_H 1
#cmakedefine HAVE_MATH_H 1
#cmakedefine HAVE_ICONV_H 1
#cmakedefine HAVE_SIGNAL_H 1
#cmakedefine HAVE_ALTIVEC_H 1
#cmakedefine HAVE_PTHREAD_NP_H 1
#cmakedefine HAVE_LIBUDEV_H 1
#cmakedefine HAVE_DBUS_DBUS_H 1

/* C library functions */
#cmakedefine HAVE_MALLOC 1
#cmakedefine HAVE_CALLOC 1
#cmakedefine HAVE_REALLOC 1
#cmakedefine HAVE_FREE 1
#cmakedefine HAVE_ALLOCA 1
#ifndef __WIN32__ /* Don't use C runtime versions of these on Windows */
#cmakedefine HAVE_GETENV 1
#cmakedefine HAVE_SETENV 1
#cmakedefine HAVE_PUTENV 1
#cmakedefine HAVE_UNSETENV 1
#endif
#cmakedefine HAVE_QSORT 1
#cmakedefine HAVE_ABS 1
#cmakedefine HAVE_BCOPY 1
#cmakedefine HAVE_MEMSET 1
#cmakedefine HAVE_MEMCPY 1
#cmakedefine HAVE_MEMMOVE 1
#cmakedefine HAVE_MEMCMP 1
#cmakedefine HAVE_STRLEN 1
#cmakedefine HAVE_STRLCPY 1
#cmakedefine HAVE_STRLCAT 1
#cmakedefine HAVE_STRDUP 1
#cmakedefine HAVE__STRREV 1
#cmakedefine HAVE__STRUPR 1
#cmakedefine HAVE__STRLWR 1
#cmakedefine HAVE_INDEX 1
#cmakedefine HAVE_RINDEX 1
#cmakedefine HAVE_STRCHR 1
#cmakedefine HAVE_STRRCHR 1
#cmakedefine HAVE_STRSTR 1
#cmakedefine HAVE_ITOA 1
#cmakedefine HAVE__LTOA 1
#cmakedefine HAVE__UITOA 1
#cmakedefine HAVE__ULTOA 1
#cmakedefine HAVE_STRTOL 1
#cmakedefine HAVE_STRTOUL 1
#cmakedefine HAVE__I64TOA 1
#cmakedefine HAVE__UI64TOA 1
#cmakedefine HAVE_STRTOLL 1
#cmakedefine HAVE_STRTOULL 1
#cmakedefine HAVE_STRTOD 1
#cmakedefine HAVE_ATOI 1
#cmakedefine HAVE_ATOF 1
#cmakedefine HAVE_STRCMP 1
#cmakedefine HAVE_STRNCMP 1
#cmakedefine HAVE__STRICMP 1
#cmakedefine HAVE_STRCASECMP 1
#cmakedefine HAVE__STRNICMP 1
#cmakedefine HAVE_STRNCASECMP 1
#cmakedefine HAVE_VSSCANF 1
#cmakedefine HAVE_VSNPRINTF 1
#cmakedefine HAVE_M_PI 1
#cmakedefine HAVE_ATAN 1
#cmakedefine HAVE_ATAN2 1
#cmakedefine HAVE_ACOS 1
#cmakedefine HAVE_ASIN 1
#cmakedefine HAVE_CEIL 1
#cmakedefine HAVE_COPYSIGN 1
#cmakedefine HAVE_COS 1
#cmakedefine HAVE_COSF 1
#cmakedefine HAVE_FABS 1
#cmakedefine HAVE_FLOOR 1
#cmakedefine HAVE_LOG 1
#cmakedefine HAVE_POW 1
#cmakedefine HAVE_SCALBN 1
#cmakedefine HAVE_SIN 1
#cmakedefine HAVE_SINF 1
#cmakedefine HAVE_SQRT 1
#cmakedefine HAVE_FSEEKO 1
#cmakedefine HAVE_FSEEKO64 1
#cmakedefine HAVE_SIGACTION 1
#cmakedefine HAVE_SA_SIGACTION 1
#cmakedefine HAVE_SETJMP 1
#cmakedefine HAVE_NANOSLEEP 1
#cmakedefine HAVE_SYSCONF 1
#cmakedefine HAVE_SYSCTLBYNAME 1
#cmakedefine HAVE_CLOCK_GETTIME 1
#cmakedefine HAVE_GETPAGESIZE 1
#cmakedefine HAVE_MPROTECT 1
#cmakedefine HAVE_ICONV 1
#cmakedefine HAVE_PTHREAD_SETNAME_NP 1
#cmakedefine HAVE_PTHREAD_SET_NAME_NP 1
#cmakedefine HAVE_SEM_TIMEDWAIT 1
#elif __WIN32__
#cmakedefine HAVE_STDARG_H 1
#cmakedefine HAVE_STDDEF_H 1
#else
/* We may need some replacement for stdarg.h here */
#include <stdarg.h>
#endif /* HAVE_LIBC */

/* Enable overlay support */
#cmakedefine SDL_OVERLAY @SDL_OVERLAY@

/* Platform specific definitions */
#if !defined(__WIN32__)
#  if !defined(_STDINT_H_) && !defined(_STDINT_H) && !defined(HAVE_STDINT_H) && !defined(_HAVE_STDINT_H)
typedef unsigned int size_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t;
#  endif /* if (stdint.h isn't available) */
#else /* __WIN32__ */
#  if !defined(_STDINT_H_) && !defined(HAVE_STDINT_H) && !defined(_HAVE_STDINT_H)
#    if defined(__GNUC__) || defined(__DMC__) || defined(__WATCOMC__)
#define HAVE_STDINT_H	1
#    elif defined(_MSC_VER)
typedef signed __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef signed __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef signed __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
#      ifndef _UINTPTR_T_DEFINED
#        ifdef  _WIN64
typedef unsigned __int64 uintptr_t;
#          else
typedef unsigned int uintptr_t;
#        endif
#define _UINTPTR_T_DEFINED
#      endif
/* Older Visual C++ headers don't have the Win64-compatible typedefs... */
#      if ((_MSC_VER <= 1200) && (!defined(DWORD_PTR)))
#define DWORD_PTR DWORD
#      endif
#      if ((_MSC_VER <= 1200) && (!defined(LONG_PTR)))
#define LONG_PTR LONG
#      endif
#    else /* !__GNUC__ && !_MSC_VER */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
#      ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef unsigned int size_t;
#      endif
typedef unsigned int uintptr_t;
#    endif /* __GNUC__ || _MSC_VER */
#  endif /* !_STDINT_H_ && !HAVE_STDINT_H */
#endif /* __WIN32__ */

#endif /* _SDL_config_h */
