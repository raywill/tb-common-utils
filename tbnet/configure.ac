#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT(src/transport.h)
AM_INIT_AUTOMAKE(tbnet, 0.1)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_LANG_CPLUSPLUS 
CXXFLAGS="-O3 -Wall -fPIC" 

if test -n "${TBLIB_ROOT}"; then
    ac_default_prefix=${TBLIB_ROOT}
    includedir=${ac_default_prefix}/include/tbnet 
fi
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h string.h strings.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 gethostbyname gettimeofday memmove memset socket strcasecmp strdup])
AC_PREFIX_DEFAULT(/usr/local/tbnet)
AC_CONFIG_FILES([Makefile
                 src/Makefile
		 src/test/Makefile
                 test/Makefile])
AC_OUTPUT
