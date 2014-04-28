dnl $Id$
dnl config.m4 for extension cassandra

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(cassandra, for cassandra support,
[  --with-cassandra[=DIR]             Include Cassandra support. DIR is Cassandra base directory.])

PHP_ARG_WITH(cql-lib, for specified location of CQL library,
[  --with-cql-lib[=DIR]             Cassandra: Set the path to libcql install prefix.])

if test "$PHP_CASSANDRA" != "no"; then

  SEARCH_FOR="cql/cql.hpp"

  if test -r $PHP_CASSANDRA/include/$SEARCH_FOR; then
    CQL_DIR=$PHP_CASSANDRA
  else
    AC_MSG_CHECKING(for CQL in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/$SEARCH_FOR; then
        CQL_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$CQL_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the cassandra distribution])
  fi

  dnl # --with-cassandra -> add include path
  PHP_ADD_INCLUDE($CQL_DIR/include)

  dnl # --with-cassandra -> check for lib and symbol presence
  LIBNAME=cql
  LIBSYMBOL=cql

  PHP_REQUIRE_CXX()

  PHP_SUBST(CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(boost_thread, 1, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(boost_system, 1, CASSANDRA_SHARED_LIBADD)

  if test -z "$PHP_CQL_LIB"; then
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PHP_CQL_LIB, CASSANDRA_SHARED_LIBADD)
  else

    SEARCH_LIB="lib$LIBNAME.so"
    
    AC_MSG_CHECKING(for lib CQL in default path)
    for i in /usr/local /usr; do
      if test -r $i/lib/$SEARCH_LIB; then
        CQL_LIB_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done

    if test -z "$CQL_LIB_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please reinstall the cassandra cql library distribution])
    fi

    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CQL_LIB_DIR, CASSANDRA_SHARED_LIBADD)

  fi

  CASSANDRA_SOURCES="cassandra.cpp";
  PHP_NEW_EXTENSION(cassandra, $CASSANDRA_SOURCES, $ext_shared)
fi
