m4_define([MDNS_MAJOR], [0])
m4_define([MDNS_MINOR], [0])
m4_define([MDNS_MICRO], [9])

AC_COPYRIGHT([Copyright 2014-2018 VideoLabs])

AC_INIT([microDNS], [MDNS_MAJOR.MDNS_MINOR.MDNS_MICRO])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [
  AM_SILENT_RULES([yes])
])
AC_CANONICAL_HOST

MICRODNS_MAJOR=MDNS_MAJOR
MICRODNS_MINOR=MDNS_MINOR
MICRODNS_MICRO=MDNS_MICRO
AC_SUBST(MICRODNS_MAJOR)
AC_SUBST(MICRODNS_MINOR)
AC_SUBST(MICRODNS_MICRO)

LT_INIT([win32-dll])
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_C_INLINE

AC_CONFIG_MACRO_DIR([m4])

AC_HEADER_ASSERT

dnl check for -Wall -Wextra support
CC_CHECK_CFLAG_APPEND([-Wall -Wsign-compare -Wextra -Wstrict-aliasing -Wstrict-overflow -Wformat=2 -Wno-unused-parameter -pipe -Wcast-align -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -Wlogical-op])

dnl
dnl  Check the operating system
dnl
HAVE_WIN64="0"
HAVE_IOS="0"
HAVE_OSX="0"
HAVE_TVOS="0"

LIBSOCKET=

case "${host_os}" in
  "")
    SYS=unknown
    ;;
  linux*)
    SYS=linux
    ;;
  bsdi*)
    SYS=bsdi
    ;;
  freebsd*)
    SYS=freebsd
    CPPFLAGS="${CPPFLAGS} -I/usr/local/include"
    LDFLAGS="${LDFLAGS} -L/usr/local/lib"
    ;;
  openbsd*)
    SYS=openbsd
    CPPFLAGS="${CPPFLAGS} -I/usr/local/include -I/usr/X11R6/include"
    LDFLAGS="${LDFLAGS} -L/usr/local/lib -L/usr/X11R6/lib/"
    ;;
  netbsd*)
    SYS=netbsd
    ;;
  dragonfly*)
    SYS=dragonfly
    ;;
  darwin*)

    dnl Force gcc "-arch" flag
    ARCH_flag=""
    case "${host}" in
      i?86*)
        ARCH_flag="-arch i386"
      ;;
      ppc64*)
        ARCH_flag="-arch ppc64"
      ;;
      ppc*)
        ARCH_flag="-arch ppc"
      ;;
      x86_64*)
        ARCH_flag="-arch x86_64"
      ;;
      arm*)
        ac_cv_c_bigendian="no"
        ac_cv_c_attribute_packed="no"
      ;;
    esac

    SYS=darwin
    CFLAGS="${CFLAGS} -D_INTL_REDIRECT_MACROS -std=gnu11 ${ARCH_flag}"
    CXX="${CXX} -stdlib=libc++ -std=c++11"
    CXXFLAGS="${CXXFLAGS} -D_INTL_REDIRECT_MACROS ${ARCH_flag}"
    CPPFLAGS="${CPPFLAGS} ${ARCH_flag}"
    OBJCFLAGS="${OBJCFLAGS} -D_INTL_REDIRECT_MACROS -std=gnu11 ${ARCH_flag}"
    LDFLAGS="${LDFLAGS} -Wl,-headerpad_max_install_names ${ARCH_flag}"

    AC_EGREP_CPP(yes,
            [#import <TargetConditionals.h>
             #if TARGET_OS_IPHONE
             yes
             #endif],
             [HAVE_IOS="1"],
             [HAVE_OSX="1"])

    AC_EGREP_CPP(yes,
            [#import <TargetConditionals.h>
             #if TARGET_OS_TV
             yes
             #endif],
             [HAVE_TVOS="1"
             HAVE_IOS="0"
             HAVE_OSX="0"
             ],)

    dnl Allow binaries created on Lion to run on earlier releases
    AC_EGREP_CPP(yes,
           [#import <Cocoa/Cocoa.h>
            #ifdef MAC_OS_X_VERSION_10_7
            yes
            #endif],
            [AC_MSG_RESULT([yes])
            AC_LIBOBJ([getdelim])
            AC_LIBOBJ([strndup])
            AC_LIBOBJ([strnlen])],)

    dnl
    dnl  Handle Mac OS X SDK flags
    dnl
    AC_ARG_WITH(macosx-sdk,
      [AS_HELP_STRING([--with-macosx-sdk=DIR],
        [compile using the SDK in DIR])])
    if test "${with_macosx_sdk}" != "" ; then
        test ! -d "${with_macosx_sdk}" && AC_MSG_ERROR([SDK "${with_macosx_sdk}" not found])
        CPP="${CPP} -isysroot ${with_macosx_sdk}"
        CC="${CC} -isysroot ${with_macosx_sdk}"
        CXX="${CXX} -isysroot ${with_macosx_sdk}"
        OBJC="${OBJC} -isysroot ${with_macosx_sdk}"
        LD="${LD} -syslibroot ${with_macosx_sdk}"
    fi
    AC_ARG_WITH(macosx-version-min,
      [AS_HELP_STRING([--with-macosx-version-min=VERSION],
        [compile for Mac OS X VERSION and above])])
    if test "${with_macosx_version_min}" != "" ; then
        CPP="${CPP} -mmacosx-version-min=${with_macosx_version_min}"
        CC="${CC} -mmacosx-version-min=${with_macosx_version_min}"
        CXX="${CXX} -mmacosx-version-min=${with_macosx_version_min}"
        OBJC="${OBJC} -mmacosx-version-min=${with_macosx_version_min}"
        LD="${LD} -mmacosx_version_min=${with_macosx_version_min}"
    fi
    ;;
  *mingw32* | *cygwin* | *wince* | *mingwce*)
    AC_DEFINE([_UNICODE], [1], [Define to 1 for Unicode (Wide Chars) APIs.])
    AC_DEFINE([UNICODE], [1], [Define to 1 for Unicode (Wide Chars) APIs.])
    AC_DEFINE([_POSIX_C_SOURCE], [200809L], [IEEE Std 1003.1.])
    AC_DEFINE([_BSD_SOURCE], [1], [ISO C, POSIX, and 4.3BSD things.])

    case "${host_os}" in
      *mingw32*)
        SYS=mingw32
        ;;
      *cygwin*)
        SYS=cygwin
        ;;
    esac

    if test "${SYS}" = "mingw32"; then
        # DEP, ASLR, NO SEH
        LDFLAGS="${LDFLAGS} -Wl,--nxcompat -Wl,--no-seh -Wl,--dynamicbase"

        AC_CHECK_PROGS(U2D, [unix2dos todos], unix2dos)
        ac_default_prefix="`pwd`/_win32"
        DESTDIR="`pwd`/_win32/"

        dnl
        dnl NSIS/MSI Installer prefix and WIN64
        dnl
        case "${host}" in
            amd64*|x86_64*)
                HAVE_WIN64="1"
                WINDOWS_ARCH="x64"
                PROGRAMFILES="PROGRAMFILES64"
            ;;
            *)
                WINDOWS_ARCH="x86"
                PROGRAMFILES="PROGRAMFILES"
            ;;
        esac
        LIBSOCKET='-lws2_32 -liphlpapi'
        AC_SUBST(WINDOWS_ARCH)
        AC_SUBST(PROGRAMFILES)

    fi
    ;;
  *nto*)
    SYS=nto
    ;;
  solaris*)
    SYS=solaris
    ;;
  hpux*)
    SYS=hpux
    ;;
  *os2*)
    SYS=os2
    LDFLAGS="${LDFLAGS} -Zomf -Zbin-files -Zargs-wild -Zhigh-mem"
    AC_LIBOBJ([freeaddrinfo])
    ;;
  *)
    SYS="${host_os}"
    ;;
esac
AM_CONDITIONAL(HAVE_WIN32,   test "${SYS}" = "mingw32")

LIBS_SAVE=${LIBS}
LIBS="${LIBS} ${LIBSOCKET}"
AC_LINK_IFELSE([
    AC_LANG_PROGRAM([#ifdef _WIN32
        #include <ws2tcpip.h>
        #include <windows.h>
        # if _WIN32_WINNT < 0x600
        #  error Needs vista+
        # endif
        #else
        #include <sys/socket.h>
        #include <arpa/inet.h>
        #endif], [
        inet_ntop(AF_INET, NULL, NULL, 0);
    ])],[AC_DEFINE([HAVE_INET_NTOP],[1],[Define to 1 if you have inet_ntop function])],[AC_LIBOBJ([inet_ntop])])
AC_LINK_IFELSE([
    AC_LANG_PROGRAM([#include <stddef.h>
        #ifdef _WIN32
        #include <winsock2.h>
        #include <windows.h>
        # if _WIN32_WINNT < 0x600
        #  error Needs vista+
        # endif
        #else
        #include <poll.h>
        #endif], [
        poll(NULL, 0, 0);
    ])],[AC_DEFINE([HAVE_POLL],[1],[Define to 1 if you have poll function])])

LIBS=${LIBS_SAVE}

dnl Check for struct pollfd
AC_CHECK_TYPES([struct pollfd],,,
[#include <sys/types.h>
#if HAVE_POLL
# include <poll.h>
#elif defined (_WIN32)
# include <winsock2.h>
#endif
])


AC_CHECK_MEMBER([struct ifaddrs.ifa_addr],[
    AC_DEFINE([HAVE_IFADDRS],[1],[Define to 1 if you have ifaddrs.ifa_addr function])],
    [],[[#include <ifaddrs.h>]])


AC_SUBST([LIBSOCKET])

PKG_INSTALLDIR

AC_CONFIG_FILES([
Makefile
src/microdns.pc
src/version.h
doc/Doxyfile
])

AC_OUTPUT
