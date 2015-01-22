/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_CASSANDRA_H
#define PHP_CASSANDRA_H

extern zend_module_entry cassandra_module_entry;
#define phpext_cassandra_ptr &cassandra_module_entry

#ifdef PHP_WIN32
#	define PHP_CASSANDRA_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_CASSANDRA_API __attribute__ ((visibility("default")))
#else
#	define PHP_CASSANDRA_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include <boost/asio.hpp>
#include <cql/cql.hpp>
#include <cql/cql_connection.hpp>
#include <cql/cql_session.hpp>
#include <cql/cql_cluster.hpp>
#include <cql/cql_builder.hpp>
#include <cql/cql_result.hpp>

#include <cql/cql_list.hpp>
#include <cql/cql_set.hpp>
#include <cql/cql_map.hpp>
#include <cql/cql_decimal.hpp>
#include <cql/cql_varint.hpp>

#include <cql/exceptions/cql_no_host_available_exception.hpp>

PHP_MINIT_FUNCTION(cassandra);
PHP_MSHUTDOWN_FUNCTION(cassandra);
PHP_RINIT_FUNCTION(cassandra);
PHP_RSHUTDOWN_FUNCTION(cassandra);
PHP_MINFO_FUNCTION(cassandra);

PHP_FUNCTION(confirm_cassandra_compiled);	/* For testing, remove later. */

struct cql_builder_object {
    zend_object                            std;
    boost::shared_ptr<cql::cql_builder_t>  cql_builder;
};

struct cql_cluster_object {
    zend_object                            std;
    boost::shared_ptr<cql::cql_cluster_t>  cql_cluster;
};

struct cql_error_object {
    zend_object                            std;
    cql::cql_error_t                       cql_error;
};

struct cql_future_result_object {
    zend_object                            std;
    boost::shared_future<cql::cql_future_result_t> cql_future_result;
};

struct cql_query_object {
    zend_object                            std;
    boost::shared_ptr<cql::cql_query_t>    cql_query;
};

struct cql_session_object {
    zend_object                            std;
    boost::shared_ptr<cql::cql_session_t>  cql_session;
};

struct cql_result_object {
    zend_object                            std;
    boost::shared_ptr<cql::cql_result_t>   cql_result;
};

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

 */
ZEND_BEGIN_MODULE_GLOBALS(cassandra)
	cql_cluster_object *g_cluster_object;
	cql_session_object *g_session_object;
ZEND_END_MODULE_GLOBALS(cassandra)

/* In every utility function you add that needs to use variables 
   in php_cassandra_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as CASSANDRA_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define CASSANDRA_G(v) TSRMG(cassandra_globals_id, zend_cassandra_globals *, v)
#else
#define CASSANDRA_G(v) (cassandra_globals.v)
#endif

#endif	/* PHP_CASSANDRA_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
