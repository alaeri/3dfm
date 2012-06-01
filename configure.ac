# Process this file with autoconf to produce a configure script.
AC_INIT(3dfm, 1.0, praxis@well.com)
AM_INIT_AUTOMAKE(3dfm, 1.0)
AC_CONFIG_SRCDIR([src/PrefsLFtab.h])
AM_CONFIG_HEADER([config.h])


# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([GL], [glVertex3f])
AC_CHECK_LIB([GLU], [gluOrtho2D])
AC_CHECK_LIB([glut], [glutBitmapCharacter])
AC_CHECK_LIB([m], [cos])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([atexit floor getcwd memset sqrt strstr])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
