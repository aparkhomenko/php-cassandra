dnl $Id$
dnl config.m4 for extension cassandra

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(cassandra, for cassandra support,
[  --with-cassandra             Include cassandra support])

PHP_ARG_WITH(cassandra-dir,  for c++ cassandra driver,
[  --with-cassandra-dir[=DIR]   Set the path to libcql install prefix.], yes)

if test "$PHP_CASSANDRA" != "no"; then

	AC_MSG_RESULT($PHP_CASSANDRA)

	SEARCH_FOR="cql/cql.hpp"

	AC_MSG_CHECKING([for cql files in default path])

		for i in $PHP_CASSANDRA_DIR /usr/local /usr ; do
			if test -r $i/include/$SEARCH_FOR;
				then
				CQL_DIR=$i
				AC_MSG_RESULT(found in $i)
				break
			fi
			
			AC_MSG_CHECKING($i)
		done

  dnl Write more examples of tests here...

  dnl # --with-cassandra -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/cassandra.h"  # you most likely want to change this
  dnl if test -r $PHP_CASSANDRA/$SEARCH_FOR; then # path given as parameter
  dnl   CASSANDRA_DIR=$PHP_CASSANDRA
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for cassandra files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CASSANDRA_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  
  if test -z "$CQL_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Please reinstall the cassandra distribution])
  fi

  dnl # --with-cassandra -> add include path
  PHP_ADD_INCLUDE($CQL_DIR/include)

  dnl # --with-cassandra -> check for lib and symbol presence
  LIBNAME=cassandra
  LIBSYMBOL=cassandra

  dnl # PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl # [
  dnl #   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CQL_DIR/lib, CQL_SHARED_LIBADD)
  dnl #   AC_DEFINE(HAVE_CQLLIB,0.7,[ ])
  dnl # ],[
  dnl #   AC_MSG_ERROR([wrong cql lib version or lib not found])
  dnl # ],[
  dnl #   -L$CQL_DIR/lib -lm
  dnl # ])
  dnl

  PHP_REQUIRE_CXX()

  PHP_SUBST(CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(boost_thread, 1, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(boost_system, 1, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(cql, $CQL_DIR, CASSANDRA_SHARED_LIBADD)

  #PHP_ADD_LIBRARY_WITH_PATH(stdc++, /usr/lib/, CPPEXT_SHARED_LIBADD)
  
  CASSANDRA_SOURCES="cassandra.cpp";
  PHP_NEW_EXTENSION(cassandra, $CASSANDRA_SOURCES, $ext_shared)
fi
