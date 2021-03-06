dnl $Id$
dnl config.m4 for extension entity

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(entity, for entity support,
dnl Make sure that the comment is aligned:
dnl [  --with-entity             Include entity support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(entity, whether to enable entity support,
dnl Make sure that the comment is aligned:
[  --enable-entity           Enable entity support])

if test "$PHP_ENTITY" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=`$PKG_CONFIG foo --cflags`
  dnl     LIBFOO_LIBDIR=`$PKG_CONFIG foo --libs`
  dnl     LIBFOO_VERSON=`$PKG_CONFIG foo --modversion`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, ENTITY_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-entity -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/entity.h"  # you most likely want to change this
  dnl if test -r $PHP_ENTITY/$SEARCH_FOR; then # path given as parameter
  dnl   ENTITY_DIR=$PHP_ENTITY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for entity files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ENTITY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ENTITY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the entity distribution])
  dnl fi

  dnl # --with-entity -> add include path
  dnl PHP_ADD_INCLUDE($ENTITY_DIR/include)

  dnl # --with-entity -> check for lib and symbol presence
  dnl LIBNAME=entity # you may want to change this
  dnl LIBSYMBOL=entity # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ENTITY_DIR/$PHP_LIBDIR, ENTITY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ENTITYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong entity lib version or lib not found])
  dnl ],[
  dnl   -L$ENTITY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ENTITY_SHARED_LIBADD)

  PHP_NEW_EXTENSION(entity, entity.c src/utils.c src/class_column.c src/class_column_save.c src/class_column_set.c \
   src/class_entity.c src/class_entity_column_set.c src/class_entity_set.c src/class_exception.c src/class_filter.c \
   src/class_filter_rule.c src/class_table.c src/class_validation.c src/class_validation_rule.c \
   src/class_db.c src/class_db_sqlbuilder.c src/class_db_sqlruner.c src/class_db_result.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
