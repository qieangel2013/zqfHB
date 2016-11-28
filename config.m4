dnl $Id$
dnl config.m4 for extension zqfHB

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

 PHP_ARG_WITH(zqfHB, for zqfHB support,
dnl Make sure that the comment is aligned:
 [  --with-zqfHB             Include zqfHB support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(zqfHB, whether to enable zqfHB support,
dnl Make sure that the comment is aligned:
dnl [  --enable-zqfHB           Enable zqfHB support])

if test "$PHP_ZQFHB" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-zqfHB -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/zqfHB.h"  # you most likely want to change this
  dnl if test -r $PHP_ZQFHB/$SEARCH_FOR; then # path given as parameter
  dnl   ZQFHB_DIR=$PHP_ZQFHB
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for zqfHB files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ZQFHB_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ZQFHB_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the zqfHB distribution])
  dnl fi

  dnl # --with-zqfHB -> add include path
  dnl PHP_ADD_INCLUDE($ZQFHB_DIR/include)

  dnl # --with-zqfHB -> check for lib and symbol presence
  dnl LIBNAME=zqfHB # you may want to change this
  dnl LIBSYMBOL=zqfHB # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ZQFHB_DIR/$PHP_LIBDIR, ZQFHB_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ZQFHBLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong zqfHB lib version or lib not found])
  dnl ],[
  dnl   -L$ZQFHB_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  PHP_SUBST(ZQFHB_SHARED_LIBADD)

  PHP_NEW_EXTENSION(zqfHB, zqfHB.c, $ext_shared)
fi
