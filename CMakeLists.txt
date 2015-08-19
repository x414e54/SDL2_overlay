if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the SDL source code and call cmake from there")
endif()

LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../SDL2)

cmake_minimum_required(VERSION 2.6)
project(SDL2_overlay C)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFiles)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceRuns)
include(CheckCCompilerFlag)
include(CheckTypeSize)
include(CheckStructHasMember)
include(CMakeDependentOption)
find_package(SDL2)
set(CMAKE_MODULE_PATH "${SDL2_overlay_SOURCE_DIR}/cmake")
include(${SDL2_overlay_SOURCE_DIR}/cmake/macros.cmake)
include(${SDL2_overlay_SOURCE_DIR}/cmake/sdlchecks.cmake)

list(APPEND EXTRA_LIBS ${SDL2_LIBRARIES})
    
# General settings
# Edit include/SDL_version.h and change the version, then:
#   SDL_MICRO_VERSION += 1;
#   SDL_INTERFACE_AGE += 1;
#   SDL_BINARY_AGE += 1;
# if any functions have been added, set SDL_INTERFACE_AGE to 0.
# if backwards compatibility has been broken,
# set SDL_BINARY_AGE and SDL_INTERFACE_AGE to 0.
set(SDL_MAJOR_VERSION 0)
set(SDL_MINOR_VERSION 1)
set(SDL_MICRO_VERSION 0)
set(SDL_INTERFACE_AGE 0)
set(SDL_BINARY_AGE 0)
set(SDL_VERSION "${SDL_MAJOR_VERSION}.${SDL_MINOR_VERSION}.${SDL_MICRO_VERSION}")

# Calculate a libtool-like version number
math(EXPR LT_CURRENT "${SDL_MICRO_VERSION} - ${SDL_INTERFACE_AGE}")
math(EXPR LT_AGE "${SDL_BINARY_AGE} - ${SDL_INTERFACE_AGE}")
math(EXPR LT_MAJOR "${LT_CURRENT}- ${LT_AGE}")
set(LT_REVISION "${SDL_INTERFACE_AGE}")
set(LT_RELEASE "${SDL_MAJOR_VERSION}.${SDL_MINOR_VERSION}")
set(LT_VERSION "${LT_MAJOR}.${LT_AGE}.${LT_REVISION}")

message(STATUS "${LT_VERSION} :: ${LT_AGE} :: ${LT_REVISION} :: ${LT_CURRENT} :: ${LT_RELEASE}")

# General settings & flags
set(LIBRARY_OUTPUT_DIRECTORY "build")
# Check for 64 or 32 bit
set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_64 TRUE)
  set(PROCESSOR_ARCH "x64")
else()
  set(ARCH_64 FALSE)
  set(PROCESSOR_ARCH "x86")
endif()
set(LIBNAME SDL2)
if(NOT LIBTYPE)
  set(LIBTYPE SHARED)
endif()

# Get the platform
if(WIN32)
  if(NOT WINDOWS)
    set(WINDOWS TRUE)
  endif()
elseif(UNIX AND NOT APPLE)
  if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
    set(LINUX TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kFreeBSD.*")
    set(FREEBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kNetBSD.*|NetBSD.*")
    set(NETBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
    set(OPENBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*GNU.*")
    set(GNU TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*BSDI.*")
    set(BSDI TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly.*|FreeBSD")
    set(FREEBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "SYSV5.*")
    set(SYSV5 TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Solaris.*")
    set(SOLARIS TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "HP-UX.*")
    set(HPUX TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "AIX.*")
    set(AIX TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Minix.*")
    set(MINIX TRUE)
  endif()
elseif(APPLE)
  if(CMAKE_SYSTEM_NAME MATCHES ".*Darwin.*")
    set(DARWIN TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
    set(MACOSX TRUE)
  endif()
  # TODO: iOS?
elseif(CMAKE_SYSTEM_NAME MATCHES "BeOS.*")
  message_error("BeOS support has been removed as of SDL 2.0.2.")
elseif(CMAKE_SYSTEM_NAME MATCHES "Haiku.*")
  set(HAIKU TRUE)
endif()

# Don't mistake osx for unix
if(UNIX AND NOT APPLE)
  set(UNIX_SYS ON)
else()
  set(UNIX_SYS OFF)
endif()

if(UNIX OR APPLE)
  set(UNIX_OR_MAC_SYS ON)
else()
  set(UNIX_OR_MAC_SYS OFF)
endif()

# Default option knobs
if(APPLE OR ARCH_64)
  set(OPT_DEF_SSEMATH ON)
endif()
if(UNIX OR MINGW OR MSYS)
  set(OPT_DEF_LIBC ON)
endif()

# Compiler info
if(CMAKE_COMPILER_IS_GNUCC)
  set(USE_GCC TRUE)
  set(OPT_DEF_ASM TRUE)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(USE_CLANG TRUE)
  set(OPT_DEF_ASM TRUE)
elseif(MSVC_VERSION GREATER 1400) # VisualStudio 8.0+
  set(OPT_DEF_ASM TRUE)
  #set(CMAKE_C_FLAGS "/ZI /WX- /
else()
  set(OPT_DEF_ASM FALSE)
endif()

# Default flags, if not set otherwise
if("$ENV{CFLAGS}" STREQUAL "")
  if(USE_GCC OR USE_CLANG)
    set(CMAKE_C_FLAGS "-g -O3")
  endif()
else("$ENV{CFLAGS}" STREQUAL "")
  set(CMAKE_C_FLAGS "$ENV{CFLAGS}")
  list(APPEND EXTRA_CFLAGS "$ENV{CFLAGS}")
endif()
if(NOT ("$ENV{CFLAGS}" STREQUAL "")) # Hackish, but does the trick on Win32
  list(APPEND EXTRA_LDFLAGS "$ENV{LDFLAGS}")
endif()

if(MSVC)
  option(FORCE_STATIC_VCRT "Force /MT for static VC runtimes" OFF)
  if(FORCE_STATIC_VCRT)
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif()
    endforeach(flag_var)
  endif()
endif()

# General includes
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_overlay_BINARY_DIR}/include ${SDL2_overlay_SOURCE_DIR}/include)

set(LIBC ON)
set(SDL_SHARED ON CACHE BOOL "Build a shared version of the library")
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library")

# General source files
file(GLOB SOURCE_FILES
  ${SDL2_overlay_SOURCE_DIR}/src/*.c)

# TODO: Can't deactivate on FreeBSD? w/o LIBC, SDL_stdinc.h can't define
# anything.
if(LIBC)
  if(WINDOWS AND NOT MINGW)
    set(HAVE_LIBC TRUE)
    foreach(_HEADER stdio.h string.h ctype.h math.h)
      string(TOUPPER "HAVE_${_HEADER}" _UPPER)
      string(REPLACE "." "_" _HAVE_H ${_UPPER})
      set(${_HAVE_H} 1)
    endforeach()
    set(HAVE_SIGNAL_H 1)
    foreach(_FN
            malloc calloc realloc free qsort abs memset memcpy memmove memcmp
            strlen _strrev _strupr _strlwr strchr strrchr strstr itoa _ltoa
            _ultoa strtol strtoul strtoll strtod atoi atof strcmp strncmp
            _stricmp _strnicmp sscanf atan atan2 acos asin ceil copysign cos
            cosf fabs floor log pow scalbn sin sinf sqrt)
      string(TOUPPER ${_FN} _UPPER)
      set(HAVE_${_UPPER} 1)
    endforeach()
    if(NOT CYGWIN AND NOT MINGW)
      set(HAVE_ALLOCA 1)
    endif()
    set(HAVE_M_PI 1)
    add_definitions(-D_USE_MATH_DEFINES) # needed for M_PI
    set(STDC_HEADERS 1)
  else(WINDOWS AND NOT MINGW)
    set(HAVE_LIBC TRUE)
    check_include_file(sys/types.h HAVE_SYS_TYPES_H)
    foreach(_HEADER
            stdio.h stdlib.h stddef.h stdarg.h malloc.h memory.h string.h
            strings.h inttypes.h stdint.h ctype.h math.h iconv.h signal.h)
      string(TOUPPER "HAVE_${_HEADER}" _UPPER)
      string(REPLACE "." "_" _HAVE_H ${_UPPER})
      check_include_file("${_HEADER}" ${_HAVE_H})
    endforeach()

    check_include_files("dlfcn.h;stdint.h;stddef.h;inttypes.h;stdlib.h;strings.h;string.h;float.h" STDC_HEADERS)
    check_type_size("size_t" SIZEOF_SIZE_T)
    check_symbol_exists(M_PI math.h HAVE_M_PI)
    # TODO: refine the mprotect check
    check_c_source_compiles("#include <sys/types.h>
                             #include <sys/mman.h>
                             int main() { }" HAVE_MPROTECT)
    foreach(_FN
            strtod malloc calloc realloc free getenv setenv putenv unsetenv
            qsort abs bcopy memset memcpy memmove memcmp strlen strlcpy strlcat
            strdup _strrev _strupr _strlwr strchr strrchr strstr itoa _ltoa
            _uitoa _ultoa strtol strtoul _i64toa _ui64toa strtoll strtoull
            atoi atof strcmp strncmp _stricmp strcasecmp _strnicmp strncasecmp
            vsscanf vsnprintf fseeko fseeko64 sigaction setjmp
            nanosleep sysconf sysctlbyname
            )
      string(TOUPPER ${_FN} _UPPER)
      set(_HAVEVAR "HAVE_${_UPPER}")
      check_function_exists("${_FN}" ${_HAVEVAR})
    endforeach()

    check_library_exists(m pow "" HAVE_LIBM)
    if(HAVE_LIBM)
      set(CMAKE_REQUIRED_LIBRARIES m)
      foreach(_FN
              atan atan2 ceil copysign cos cosf fabs floor log pow scalbn sin
              sinf sqrt)
        string(TOUPPER ${_FN} _UPPER)
        set(_HAVEVAR "HAVE_${_UPPER}")
        check_function_exists("${_FN}" ${_HAVEVAR})
      endforeach()
      set(CMAKE_REQUIRED_LIBRARIES)
      list(APPEND EXTRA_LIBS m)
    endif()

    check_library_exists(iconv iconv_open "" HAVE_LIBICONV)
    if(HAVE_LIBICONV)
      list(APPEND EXTRA_LIBS iconv)
    endif()

    check_struct_has_member("struct sigaction" "sa_sigaction" "signal.h" HAVE_SA_SIGACTION)
  endif()
else(LIBC)
  if(WINDOWS)
    set(HAVE_STDARG_H 1)
    set(HAVE_STDDEF_H 1)
  endif()
endif()

set(SDL_OVERLAY 1)
file(GLOB OVERLAY_SOURCES ${SDL2_overlay_SOURCE_DIR}/src/overlay/*.c)
set(SOURCE_FILES ${SOURCE_FILES} ${OVERLAY_SOURCES})
set(HAVE_SDL_OVERLAY TRUE)

# Platform-specific options and settings
if(UNIX AND NOT APPLE)

  if(RPATH)
    set(SDL_RLD_FLAGS "")
    if(BSDI OR FREEBSD OR LINUX OR NETBSD)
      set(SDL_RLD_FLAGS "-Wl,-rpath,\${libdir}")
    elseif(SOLARIS)
      set(SDL_RLD_FLAGS "-R\${libdir}")
    endif()
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(HAVE_RPATH TRUE)
  endif()

elseif(APPLE)

  # Actually load the frameworks at the end so we don't duplicate include.
  if(SDL_FRAMEWORK_COCOA)
    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND EXTRA_LIBS ${COCOA_LIBRARY})
  endif()
endif()

configure_file("${SDL2_overlay_SOURCE_DIR}/include/SDL_overlay_config.h.cmake"
  "${SDL2_overlay_BINARY_DIR}/include/SDL_overlay_config.h")

# Prepare the flags and remove duplicates
if(EXTRA_LDFLAGS)
  list(REMOVE_DUPLICATES EXTRA_LDFLAGS)
endif()
if(EXTRA_LIBS)
  list(REMOVE_DUPLICATES EXTRA_LIBS)
endif()
if(EXTRA_CFLAGS)
  list(REMOVE_DUPLICATES EXTRA_CFLAGS)
endif()
listtostr(EXTRA_CFLAGS _EXTRA_CFLAGS)
set(EXTRA_CFLAGS ${_EXTRA_CFLAGS})

# Compat helpers for the configuration files
if(NOT WINDOWS OR CYGWIN)

  set(prefix ${CMAKE_INSTALL_PREFIX})
  set(exec_prefix "\${prefix}")
  set(libdir "\${exec_prefix}/lib${LIB_SUFFIX}")
  set(bindir "\${exec_prefix}/bin")
  set(includedir "\${prefix}/include")
  if(SDL_STATIC)
    set(ENABLE_STATIC_TRUE "")
    set(ENABLE_STATIC_FALSE "#")
  else(SDL_STATIC)
    set(ENABLE_STATIC_TRUE "#")
    set(ENABLE_STATIC_FALSE "")
  endif()
  if(SDL_SHARED)
    set(ENABLE_SHARED_TRUE "")
    set(ENABLE_SHARED_FALSE "#")
  else(SDL_SHARED)
    set(ENABLE_SHARED_TRUE "#")
    set(ENABLE_SHARED_FALSE "")
  endif()
endif()

##### Info output #####
message(STATUS "")
message(STATUS "SDL2_overlay was configured with the following options:")
message(STATUS "")
message(STATUS "Platform: ${CMAKE_SYSTEM}")
message(STATUS "64-bit:   ${ARCH_64}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "")
message(STATUS "")
message(STATUS " CFLAGS:        ${CMAKE_C_FLAGS}")
message(STATUS " EXTRA_CFLAGS:  ${EXTRA_CFLAGS}")
message(STATUS " EXTRA_LDFLAGS: ${EXTRA_LDFLAGS}")
message(STATUS " EXTRA_LIBS:    ${EXTRA_LIBS}")
message(STATUS "")
message(STATUS " Build Shared Library: ${SDL_SHARED}")
message(STATUS " Build Static Library: ${SDL_STATIC}")
message(STATUS "")
message(STATUS " Source Files: ${SOURCE_FILES}")
message(STATUS "")
if(UNIX)
  message(STATUS "If something was not detected, although the libraries")
  message(STATUS "were installed, then make sure you have set the")
  message(STATUS "CFLAGS and LDFLAGS environment variables correctly.")
  message(STATUS "")
endif()

# Ensure that the extra cflags are used at compile time
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

if(SDL_SHARED)
  add_library(SDL2_overlay SHARED ${SOURCE_FILES})
  if(UNIX)
    set_target_properties(SDL2_overlay PROPERTIES
      VERSION ${LT_VERSION}
      SOVERSION ${LT_REVISION}
      OUTPUT_NAME "SDL2_overlay")
  else(UNIX)
    set_target_properties(SDL2_overlay PROPERTIES
      VERSION ${SDL_VERSION}
      SOVERSION ${LT_REVISION}
      OUTPUT_NAME "SDL2_overlay")
  endif()
 set(_INSTALL_LIBS "SDL2_overlay" ${_INSTALL_LIBS})
 target_link_libraries(SDL2_overlay ${EXTRA_LIBS} ${EXTRA_LDFLAGS})
 set_target_properties(SDL2_overlay PROPERTIES MACOSX_RPATH "@rpath/libSDL2_overlay.dylib")
endif()

if(SDL_STATIC)
  set (BUILD_SHARED_LIBS FALSE)
  add_library(SDL2_overlay-static STATIC ${SOURCE_FILES})
  set_target_properties(SDL2_overlay-static PROPERTIES OUTPUT_NAME "SDL2_overlay")
  if(MSVC)
    set_target_properties(SDL2_overlay-static PROPERTIES LINK_FLAGS_RELEASE "/NODEFAULTLIB")
    set_target_properties(SDL2_overlay-static PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB")
    set_target_properties(SDL2_overlay-static PROPERTIES STATIC_LIBRARY_FLAGS "/NODEFAULTLIB")
  endif()
  # TODO: Win32 platforms keep the same suffix .lib for import and static
  # libraries - do we need to consider this?
  set(_INSTALL_LIBS "SDL2_overlay-static" ${_INSTALL_LIBS})
  target_link_libraries(SDL2_overlay-static ${EXTRA_LIBS} ${EXTRA_LDFLAGS})
endif()
