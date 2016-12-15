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
   SEARCH_PATH="/usr/local /usr"     # you might want to change this
   SEARCH_FOR="/include/hiredis/hiredis.h"  # you most likely want to change this
   if test -r $PHP_ZQFHB/$SEARCH_FOR; then # path given as parameter
     ZQFHB_DIR=$PHP_ZQFHB
   else # search default path list
    AC_MSG_CHECKING([for hiredis files in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         ZQFHB_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
   fi
  dnl
   if test -z "$ZQFHB_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Please reinstall the hiredis distribution wget https://github.com/redis/hiredis/archive/v0.13.3.tar.gz])
   fi

  dnl # --with-zqfHB -> add include path
   PHP_ADD_INCLUDE($ZQFHB_DIR/include/hiredis)

   LIBNAME=hiredis
   LIBSYMBOL=redisConnect  

   PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
   [
     PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ZQFHB_DIR/$PHP_LIBDIR, ZQFHB_SHARED_LIBADD)
     AC_DEFINE(HAVE_ZQFHBLIB,1,[ ])
   ],[
     AC_MSG_ERROR([wrong hiredis lib version or lib not found])
   ],[
     -L$ZQFHB_DIR/$PHP_LIBDIR -lm
   ])
  
  PHP_SUBST(ZQFHB_SHARED_LIBADD)

  PHP_NEW_EXTENSION(zqfHB, zqfHB.c, $ext_shared)
fi
