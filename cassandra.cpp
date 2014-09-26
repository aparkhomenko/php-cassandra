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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/date/php_date.h"
#include "ext/standard/info.h"
#include "php_cassandra.h"

#define  php_array_init(arg)			_array_init((arg), 0 ZEND_FILE_LINE_CC)
#ifdef   array_init
#undef   array_init
#endif

#ifdef __cplusplus
}
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


/* If you declare any globals in php_cassandra.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cassandra)
*/

/* True global resources - no need for thread safety here */
static int le_cassandra;

zend_class_entry       * php_cql_sc_entry;
zend_class_entry       * php_cql_builder_sc_entry;
zend_class_entry       * php_cql_cluster_sc_entry;
zend_class_entry       * php_cql_error_sc_entry;
zend_class_entry       * php_cql_future_result_sc_entry;
zend_class_entry       * php_cql_query_sc_entry;
zend_class_entry       * php_cql_session_sc_entry;
zend_class_entry       * php_cql_result_sc_entry;

zend_object_handlers     cql_handlers;
zend_object_handlers     cql_builder_handlers;
zend_object_handlers     cql_cluster_handlers;
zend_object_handlers     cql_error_handlers;
zend_object_handlers     cql_future_result_handlers;
zend_object_handlers     cql_query_handlers;
zend_object_handlers     cql_session_handlers;
zend_object_handlers     cql_result_handlers;

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

zend_object_value php_cql_builder_object_new(zend_class_entry *type TSRMLS_DC);
zend_object_value php_cql_cluster_object_new(zend_class_entry *type TSRMLS_DC);
zend_object_value php_cql_error_object_new(zend_class_entry *type TSRMLS_DC);
zend_object_value php_cql_future_result_object_new(zend_class_entry *type TSRMLS_DC);
zend_object_value php_cql_query_object_new(zend_class_entry *type TSRMLS_DC);
zend_object_value php_cql_session_object_new(zend_class_entry *type TSRMLS_DC);
zend_object_value php_cql_result_object_new(zend_class_entry *type TSRMLS_DC);

ZEND_BEGIN_ARG_INFO_EX(cql_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Cql, __construct);

const zend_function_entry php_cql_class_methods[] = {
		PHP_ME(Cql,  __construct,     cql_construct,       ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
		PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(cql_builder_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_builder_addcontactpoint, 0, 0, 1)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_builder_build, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(CqlBuilder, __construct);
PHP_METHOD(CqlBuilder, addContactPoint);
PHP_METHOD(CqlBuilder, build);

const zend_function_entry php_cql_builder_class_methods[] = {
		PHP_ME(CqlBuilder,  __construct,     cql_builder_construct,       ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
		PHP_ME(CqlBuilder,  addContactPoint, cql_builder_addcontactpoint, ZEND_ACC_PUBLIC)
		PHP_ME(CqlBuilder,  build,           cql_builder_build,           ZEND_ACC_PUBLIC)
		PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(cql_cluster_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, builder)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_cluster_connect, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_cluster_shutdown, 0, 0, 0)
	ZEND_ARG_INFO(0, timeout_ms)
ZEND_END_ARG_INFO()

PHP_METHOD(CqlCluster, __construct);
PHP_METHOD(CqlCluster, connect);
PHP_METHOD(CqlCluster, shutdown);

const zend_function_entry php_cql_cluster_class_methods[] = {
		PHP_ME(CqlCluster,  __construct,     cql_cluster_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
		PHP_ME(CqlCluster,  connect,         cql_cluster_connect, ZEND_ACC_PUBLIC)
		PHP_ME(CqlCluster,  shutdown,        cql_cluster_shutdown, ZEND_ACC_PUBLIC)
		PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(cql_error_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(CqlError, __construct);

const zend_function_entry php_cql_error_class_methods[] = {
		PHP_ME(CqlError,  __construct,     cql_error_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
		PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(cql_future_result_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_future_result_get_error, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_future_result_get_result, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_future_result_wait, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(CqlFutureResult, __construct);
PHP_METHOD(CqlFutureResult, getError);
PHP_METHOD(CqlFutureResult, getResult);
PHP_METHOD(CqlFutureResult, wait);

const zend_function_entry php_cql_future_result_class_methods[] = {
		PHP_ME(CqlFutureResult,  __construct,  cql_future_result_construct,   ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
		PHP_ME(CqlFutureResult,  getResult,    cql_future_result_get_result,  ZEND_ACC_PUBLIC)
		PHP_ME(CqlFutureResult,  getError,     cql_future_result_get_error,   ZEND_ACC_PUBLIC)
		PHP_ME(CqlFutureResult,  wait,         cql_future_result_wait,        ZEND_ACC_PUBLIC)
		PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(cql_query_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, queryString)
	ZEND_ARG_INFO(0, consistency)
	ZEND_ARG_INFO(0, isTraced)
	ZEND_ARG_INFO(0, retryPolicy)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_disable_tracing, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_enable_tracing, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_get_consistency, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_get_query_string, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_get_retry_policy, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_set_consistency, 0, 0, 1)
	ZEND_ARG_INFO(0, consistency)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_set_query_string, 0, 0, 1)
	ZEND_ARG_INFO(0, cql)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_query_set_retry_policy, 0, 0, 1)
	ZEND_ARG_INFO(0, retryPolicy)
ZEND_END_ARG_INFO()

PHP_METHOD(CqlQuery, __construct);
PHP_METHOD(CqlQuery, disableTracing);
PHP_METHOD(CqlQuery, enableTracing);
PHP_METHOD(CqlQuery, getConsistency);
PHP_METHOD(CqlQuery, getQueryString);
PHP_METHOD(CqlQuery, getRetryPolicy);
PHP_METHOD(CqlQuery, setConsistency);
PHP_METHOD(CqlQuery, setQueryString);
PHP_METHOD(CqlQuery, setRetryPolicy);

const zend_function_entry php_cql_query_class_methods[] = {
		PHP_ME(CqlQuery,  __construct,     cql_query_construct,        ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
		PHP_ME(CqlQuery,  disableTracing,  cql_query_disable_tracing,  ZEND_ACC_PUBLIC)
		PHP_ME(CqlQuery,  enableTracing,   cql_query_enable_tracing,   ZEND_ACC_PUBLIC)
		PHP_ME(CqlQuery,  getConsistency,  cql_query_get_consistency,  ZEND_ACC_PUBLIC)
		PHP_ME(CqlQuery,  getQueryString,  cql_query_get_query_string, ZEND_ACC_PUBLIC)
//		PHP_ME(CqlQuery,  getRetryPolicy,  cql_query_get_retry_policy, ZEND_ACC_PUBLIC)
		PHP_ME(CqlQuery,  setConsistency,  cql_query_set_consistency,  ZEND_ACC_PUBLIC)
		PHP_ME(CqlQuery,  setQueryString,  cql_query_set_query_string, ZEND_ACC_PUBLIC)
//		PHP_ME(CqlQuery,  setRetryPolicy,  cql_query_set_retry_policy, ZEND_ACC_PUBLIC)
		PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(cql_session_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, cluster)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_session_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_session_execute, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_session_prepare, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_session_query, 0, 0, 1)
	ZEND_ARG_INFO(0, cqlQuery)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_session_set_keyspace, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(CqlSession, __construct);
PHP_METHOD(CqlSession, close);
PHP_METHOD(CqlSession, execute);
PHP_METHOD(CqlSession, prepare);
PHP_METHOD(CqlSession, query);
PHP_METHOD(CqlSession, setKeyspace);

const zend_function_entry php_cql_session_class_methods[] = {
		PHP_ME(CqlSession,  __construct,     cql_session_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
		PHP_ME(CqlSession,  close,           cql_session_close, ZEND_ACC_PUBLIC)
		PHP_ME(CqlSession,  execute,         cql_session_execute, ZEND_ACC_PUBLIC)
		PHP_ME(CqlSession,  prepare,         cql_session_prepare, ZEND_ACC_PUBLIC)
		PHP_ME(CqlSession,  query,           cql_session_query, ZEND_ACC_PUBLIC)
		PHP_ME(CqlSession,  setKeyspace,     cql_session_set_keyspace, ZEND_ACC_PUBLIC)
		PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(cql_result_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_result_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, column)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_result_get, 0, 0, 1)
	ZEND_ARG_INFO(0, column)
	ZEND_ARG_INFO(0, typeColumn)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_result_get_column_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_result_get_row_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cql_result_next, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(CqlResult, __construct);
PHP_METHOD(CqlResult, exists);
PHP_METHOD(CqlResult, get);
PHP_METHOD(CqlResult, getColumnCount);
PHP_METHOD(CqlResult, getRowCount);
PHP_METHOD(CqlResult, next);

const zend_function_entry php_cql_result_class_methods[] = {
		PHP_ME(CqlResult,  __construct,    cql_result_construct,        ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
		PHP_ME(CqlResult,  exists,         cql_result_exists,           ZEND_ACC_PUBLIC)
		PHP_ME(CqlResult,  get,            cql_result_get,              ZEND_ACC_PUBLIC)
		PHP_ME(CqlResult,  getColumnCount, cql_result_get_column_count, ZEND_ACC_PUBLIC)
		PHP_ME(CqlResult,  getRowCount,    cql_result_get_row_count,    ZEND_ACC_PUBLIC)
		PHP_ME(CqlResult,  next,           cql_result_next,             ZEND_ACC_PUBLIC)
		PHP_FE_END
};

const zend_function_entry cassandra_functions[] = {
	PHP_FE_END
};

/* {{{ Construct object in php space */
void
helper_php_object_construct(zval * object, zend_class_entry * class_entry, zval **params[], zend_uint param_count)
{
	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, class_entry);
	object->refcount__gc = 1;
	object->is_ref__gc   = 1;

	zval *ctor, *dummy = NULL;
	MAKE_STD_ZVAL(ctor);
	ZVAL_STRING(ctor, "__construct", 1);

	if(call_user_function_ex(
			NULL, &object, ctor, &dummy, param_count, params, 0, NULL TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call constructor");
	}

	if (dummy) {
		zval_ptr_dtor(&dummy);
	}

	zval_ptr_dtor(&ctor);
}

zend_object_value
php_cql_builder_object_new(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cql_builder_object *obj = (cql_builder_object *)emalloc(sizeof(cql_builder_object));
    memset(obj, 0, sizeof(cql_builder_object));
    obj->std.ce = type;

    zend_object_std_init(&obj->std, type);
    object_properties_init(&obj->std, type);

    retval.handle   = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &cql_builder_handlers;

    return retval;
}

zend_object_value
php_cql_cluster_object_new(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cql_cluster_object *obj = (cql_cluster_object *) emalloc(sizeof(cql_cluster_object));
    memset(obj, 0, sizeof(cql_cluster_object));
    obj->std.ce = type;

    zend_object_std_init(&obj->std, type);
	object_properties_init(&obj->std, type);

    retval.handle   = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &cql_cluster_handlers;

    return retval;
}

zend_object_value
php_cql_error_object_new(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cql_error_object *obj = (cql_error_object *) emalloc(sizeof(cql_error_object));
    memset(obj, 0, sizeof(cql_error_object));
    obj->std.ce = type;

    zend_object_std_init(&obj->std, type);
	object_properties_init(&obj->std, type);

    retval.handle   = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &cql_error_handlers;

    return retval;
}

zend_object_value
php_cql_future_result_object_new(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cql_future_result_object *obj = (cql_future_result_object *)emalloc(sizeof(cql_future_result_object));
    memset(obj, 0, sizeof(cql_future_result_object));
    obj->std.ce = type;

    zend_object_std_init(&obj->std, type);
    object_properties_init(&obj->std, type);

    retval.handle   = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &cql_future_result_handlers;

    return retval;
}

zend_object_value
php_cql_query_object_new(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cql_query_object *obj = (cql_query_object *)emalloc(sizeof(cql_query_object));
    memset(obj, 0, sizeof(cql_query_object));
    obj->std.ce = type;

    zend_object_std_init(&obj->std, type);
	object_properties_init(&obj->std, type);

    retval.handle   = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &cql_query_handlers;

    return retval;
}

zend_object_value
php_cql_session_object_new(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cql_session_object *obj = (cql_session_object *) emalloc(sizeof(cql_session_object));
    memset(obj, 0, sizeof(cql_session_object));
    obj->std.ce = type;

    zend_object_std_init(&obj->std, type);
	object_properties_init(&obj->std, type);

    retval.handle   = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &cql_session_handlers;

    return retval;
}

zend_object_value
php_cql_result_object_new(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cql_result_object *obj = (cql_result_object *)emalloc(sizeof(cql_result_object));
    memset(obj, 0, sizeof(cql_result_object));
    obj->std.ce = type;

    zend_object_std_init(&obj->std, type);
    object_properties_init(&obj->std, type);

    retval.handle   = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &cql_result_handlers;

    return retval;
}

/* {{{ proto Cql::__construct()
        Constructs a new Cql object
 */
PHP_METHOD(Cql, __construct)
{

}

/* {{{ proto CqlBuilder::__construct()
        Constructs a new CqlBuilder object
 */
PHP_METHOD(CqlBuilder, __construct)
{
	cql_builder_object *obj = (cql_builder_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (NULL == obj) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to obj in CqlBuilder::__construct()");
		return;
	}

	obj->cql_builder = cql::cql_cluster_t::builder();
}
/* }}} */

/* {{{ proto CqlBuilder::addContactPoint(string $host [, int $port ])
 */
PHP_METHOD(CqlBuilder, addContactPoint)
{
	char *host;
	int   host_len;
	long  port      = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s|l", &host, &host_len, &port) == FAILURE) {
		return;
	}

	cql_builder_object *obj = (cql_builder_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (NULL == obj || NULL == obj->cql_builder) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find obj->cql_builder");
		return;
	}

	if (port > 0) {
		obj->cql_builder->add_contact_point(boost::asio::ip::address::from_string(host), port);
	}
	else {
		obj->cql_builder->add_contact_point(boost::asio::ip::address::from_string(host));
	}
}
/* }}} */

/* {{{ proto CqlBuilder::build()
   Returns a new CqlCluster object */
PHP_METHOD(CqlBuilder, build)
{
	cql_builder_object *obj = (cql_builder_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (obj == NULL || obj->cql_builder == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find obj->cql_builder");
		return;
	}

	zval **params[1];
	params[0] = &getThis();

	helper_php_object_construct(return_value, php_cql_cluster_sc_entry, params, 1);
}
/* }}} */

/* {{{ proto CqlCluster::__construct()
        Constructs a new CqlCluster object
 */
PHP_METHOD(CqlCluster, __construct)
{
	zval * cql_builder;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &cql_builder) == FAILURE) {
		return;
	}

	cql_builder_object *obj_builder = (cql_builder_object *) zend_object_store_get_object(cql_builder TSRMLS_CC);

	if (obj_builder == NULL || obj_builder->cql_builder == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find obj_builder->cql_builder");
		return;
	}

	cql_cluster_object *obj = (cql_cluster_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	obj->cql_cluster = boost::shared_ptr<cql::cql_cluster_t>(obj_builder->cql_builder->build());
}
/* }}} */

/* {{{ proto CqlCluster::connect()
 */
PHP_METHOD(CqlCluster, connect)
{
	cql_cluster_object *obj = (cql_cluster_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (obj == NULL || obj->cql_cluster == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find obj->cql_cluster in CqlCluster.connect");
		return;
	}

	zval **params[1];
	params[0] = &getThis();

	helper_php_object_construct(return_value, php_cql_session_sc_entry, params, 1);

}
/* }}} */

/* {{{ proto CqlCluster::shutdown([int $timeout_ms])
 */
PHP_METHOD(CqlCluster, shutdown)
{
	long  timeout_ms = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"|l", &timeout_ms) == FAILURE) {
		return;
	}

	cql_cluster_object *obj = (cql_cluster_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (obj == NULL || obj->cql_cluster == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find obj->cql_cluster in CqlCluster.connect");
		return;
	}

	obj->cql_cluster->shutdown(timeout_ms);
}

PHP_METHOD(CqlError, __construct)
{

}

/* {{{ proto CqlFutureResult::__construct()
        Constructs a new CqlFutureResult object
 */
PHP_METHOD(CqlFutureResult, __construct)
{

}

PHP_METHOD(CqlFutureResult, wait)
{
	cql_future_result_object *obj = (cql_future_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	obj->cql_future_result.wait();
}

PHP_METHOD(CqlFutureResult, getError)
{
	cql_future_result_object *obj = (cql_future_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!obj->cql_future_result.get().error.is_err()) {
		RETURN_NULL();
		return;
	}

	zval **params[0];
	helper_php_object_construct(return_value, php_cql_error_sc_entry, params, 0);

	cql_error_object * obj_error = (cql_error_object *) zend_object_store_get_object(return_value TSRMLS_CC);

	zend_update_property_bool(php_cql_error_sc_entry, return_value,   "cassandra", sizeof("cassandra")-1, obj->cql_future_result.get().error.cassandra TSRMLS_CC);
	zend_update_property_long(php_cql_error_sc_entry, return_value,   "code", sizeof("code")-1, obj->cql_future_result.get().error.code TSRMLS_CC);
	zend_update_property_bool(php_cql_error_sc_entry, return_value,   "hasError", sizeof("hasError")-1, obj->cql_future_result.get().error.is_err() TSRMLS_CC);
	zend_update_property_bool(php_cql_error_sc_entry,   return_value, "library", sizeof("library")-1, obj->cql_future_result.get().error.library TSRMLS_CC);
	zend_update_property_string(php_cql_error_sc_entry, return_value, "message", sizeof("message")-1, obj->cql_future_result.get().error.message.c_str() TSRMLS_CC);
	zend_update_property_bool(php_cql_error_sc_entry,   return_value, "transport", sizeof("transport")-1, obj->cql_future_result.get().error.transport TSRMLS_CC);
}

PHP_METHOD(CqlFutureResult, getResult)
{
	cql_future_result_object *obj = (cql_future_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!obj->cql_future_result.is_ready()) {
		RETURN_NULL();
		return;
	}

	if (obj->cql_future_result.get().error.is_err()) {
		RETURN_NULL();
		return;
	}

	Z_TYPE_P(return_value) = IS_OBJECT;
	object_init_ex(return_value, php_cql_result_sc_entry);
	return_value->refcount__gc = 1;
	return_value->is_ref__gc   = 1;

	cql_result_object * obj_result = (cql_result_object *) zend_object_store_get_object(return_value TSRMLS_CC);
	obj_result->cql_result         = obj->cql_future_result.get().result;

}

void helper_cql_query_set_consistency(boost::shared_ptr<cql::cql_query_t> cql_query, long consistency)
{
	switch (consistency) {

		case cql::CQL_CONSISTENCY_ALL:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_ALL);
			break;

		case cql::CQL_CONSISTENCY_ANY:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_ANY);
			break;

		case cql::CQL_CONSISTENCY_EACH_QUORUM:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_EACH_QUORUM);
			break;

		case cql::CQL_CONSISTENCY_LOCAL_QUORUM:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_LOCAL_QUORUM);
			break;

		case cql::CQL_CONSISTENCY_ONE:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_ONE);
			break;

		case cql::CQL_CONSISTENCY_QUORUM:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_QUORUM);
			break;

		case cql::CQL_CONSISTENCY_THREE:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_THREE);
			break;

		case cql::CQL_CONSISTENCY_TWO:
			cql_query->set_consistency(cql::CQL_CONSISTENCY_TWO);
			break;
	}
}

/* {{{ proto CqlQuery::__construct()
        Constructs a new CqlQuery object
 */
PHP_METHOD(CqlQuery, __construct)
{
	char * query_string;
	int query_string_len;
	long consistency     = cql::CQL_CONSISTENCY_DEFAULT;
	bool is_traced       = false;
	zval * retry_policy   = NULL;
	zval * t = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s|lb!o",
			&query_string,
			&query_string_len,
			&consistency,
			&is_traced,
			&retry_policy) == FAILURE) {
		return;
	}

	cql_query_object *obj = (cql_query_object *) zend_object_store_get_object(t TSRMLS_CC);

	if (obj == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find cql_query_object in CqlQuery.__construct");
		return;
	}

	obj->cql_query = boost::shared_ptr<cql::cql_query_t> (new cql::cql_query_t(query_string, cql::CQL_CONSISTENCY_DEFAULT, is_traced, false));
	helper_cql_query_set_consistency(obj->cql_query, consistency);

	zend_update_property_string(php_cql_query_sc_entry, t, "queryString", sizeof("queryString")-1, query_string TSRMLS_CC);
	zend_update_property_long(php_cql_query_sc_entry, t, "consistency", sizeof("consistency")-1, consistency TSRMLS_CC);
	zend_update_property_bool(php_cql_query_sc_entry, t, "isTraced", sizeof("isTraced")-1, is_traced TSRMLS_CC);

	if (NULL != retry_policy) {
		zend_update_property(php_cql_query_sc_entry, t, "retryPolicy", sizeof("retryPolicy")-1, retry_policy TSRMLS_CC);
	}
}

PHP_METHOD(CqlQuery, disableTracing)
{
	zval * t = getThis();

	cql_query_object *obj = (cql_query_object *) zend_object_store_get_object(t TSRMLS_CC);

	if (obj == NULL || obj->cql_query == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find obj->cql_query in CqlQuery.disableTracing");
		return;
	}

	obj->cql_query->disable_tracing();
	zend_update_property_bool(php_cql_query_sc_entry, t, "isTraced", sizeof("isTraced")-1, false TSRMLS_CC);
}

PHP_METHOD(CqlQuery, enableTracing)
{
	zval * t = getThis();

	cql_query_object *obj = (cql_query_object *) zend_object_store_get_object(t TSRMLS_CC);

	if (obj == NULL || obj->cql_query == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find obj->cql_query in CqlQuery.disableTracing");
		return;
	}

	obj->cql_query->enable_tracing();
	zend_update_property_bool(php_cql_query_sc_entry, t, "isTraced", sizeof("isTraced")-1, true TSRMLS_CC);
}

PHP_METHOD(CqlQuery, getConsistency)
{
	zval * value;

	value = zend_read_property(php_cql_query_sc_entry, getThis(), "consistency", sizeof("consistency")-1, 1 TSRMLS_CC);
	RETURN_LONG(Z_LVAL_P(value));
}

PHP_METHOD(CqlQuery, getQueryString)
{
	zval * value;

	value = zend_read_property(php_cql_query_sc_entry, getThis(), "queryString", sizeof("queryString")-1, 0 TSRMLS_CC);
	RETURN_STRING(Z_STRVAL_P(value), 0);
}

PHP_METHOD(CqlQuery, getRetryPolicy)
{

}

PHP_METHOD(CqlQuery, setConsistency)
{
	long  consistency      = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"l", &consistency) == FAILURE) {
		return;
	}

	cql_query_object *obj = (cql_query_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (NULL == obj || NULL == obj->cql_query) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find obj->cql_query");
		return;
	}

	if (consistency > 0) {
		helper_cql_query_set_consistency(obj->cql_query, consistency);
		zend_update_property_long(php_cql_query_sc_entry, getThis(), "consistency", sizeof("consistency")-1, consistency TSRMLS_CC);
	}

}

PHP_METHOD(CqlQuery, setQueryString)
{
	char *  query_string;
	int     query_string_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s", &query_string, &query_string_len) == FAILURE) {
		return;
	}

	cql_query_object *obj = (cql_query_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (NULL == obj || NULL == obj->cql_query) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find obj->cql_query");
		return;
	}

	obj->cql_query->set_query(query_string);
	zend_update_property_string(php_cql_query_sc_entry, getThis(), "queryString", sizeof("queryString")-1, query_string TSRMLS_CC);
}

PHP_METHOD(CqlQuery, setRetryPolicy)
{

}

/* CqlSession */

PHP_METHOD(CqlSession, __construct)
{
	zval * cql_cluster;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &cql_cluster) == FAILURE) {
		return;
	}

	cql_cluster_object * obj_cql_cluster = (cql_cluster_object *) zend_object_store_get_object(cql_cluster TSRMLS_CC);

	if (obj_cql_cluster == NULL || obj_cql_cluster->cql_cluster == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find obj_cql_cluster->cql_cluster");
		return;
	}

	try {

		cql_session_object *obj = (cql_session_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
		obj->cql_session        = obj_cql_cluster->cql_cluster->connect();

	}
	catch (std::exception & e) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, e.what());
	}
}

PHP_METHOD(CqlSession, close)
{
	cql_session_object *obj = (cql_session_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	if (NULL == obj || NULL == obj->cql_session) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find obj->cql_session in CqlSession.close");
		return;
	}

	try {
		obj->cql_session->close();
	}
	catch (std::exception & e) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what());
	}
}

PHP_METHOD(CqlSession, execute)
{

}

PHP_METHOD(CqlSession, prepare)
{

}

PHP_METHOD(CqlSession, query)
{
	zval * cql_query;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"O", &cql_query, php_cql_query_sc_entry) == FAILURE) {
		return;
	}

	cql_query_object * obj_cql_query = (cql_query_object *)   zend_object_store_get_object(cql_query TSRMLS_CC);
	cql_session_object *obj          = (cql_session_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	try {

		boost::shared_future<cql::cql_future_result_t> future = obj->cql_session->query(obj_cql_query->cql_query);

		Z_TYPE_P(return_value)     = IS_OBJECT;
		object_init_ex(return_value, php_cql_future_result_sc_entry);
		return_value->refcount__gc = 1;
		return_value->is_ref__gc   = 1;

		cql_future_result_object * obj_future_result = (cql_future_result_object *) zend_object_store_get_object(return_value TSRMLS_CC);
		obj_future_result->cql_future_result         = future;

	}
	catch (std::exception & e) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what());
	}
}

PHP_METHOD(CqlSession, setKeyspace)
{
	char * keyspace;
	int keyspace_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s", &keyspace, &keyspace_len) == FAILURE) {
		return;
	}

	cql_session_object *obj          = (cql_session_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	obj->cql_session->set_keyspace(keyspace);
}

/* {{{ proto CqlResult::__construct()
        Constructs a new CqlResult object
 */
PHP_METHOD(CqlResult, __construct)
{

}

PHP_METHOD(CqlResult, exists)
{
	char * column;
	int column_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s", &column, &column_len) == FAILURE) {
		return;
	}

	cql_result_object *obj = (cql_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_BOOL(obj->cql_result->exists(column));
}

PHP_METHOD(CqlResult, get)
{
	char * column;
	int column_len;
	long   column_type = cql::CQL_COLUMN_TYPE_UNKNOWN;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s|l", &column, &column_len, &column_type) == FAILURE) {
		return;
	}

	cql_result_object *obj = (cql_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	cql::cql_bigint_t  tmp_value_big_int  = 0;
	bool               tmp_value_bool     = false;
	double             tmp_value_double   = 0;
	float              tmp_value_float    = 0;
	cql::cql_int_t     tmp_value_int      = 0;
	std::string        tmp_value_string;
	zval *             tmp_zval;
	char *             tmp_key;
	char *             tmp_value_char;

	time_t ts = 0;
	long orig_column_type = 0;

	boost::shared_ptr< cql::cql_set_t >  tmp_collection_set;
	boost::shared_ptr< cql::cql_list_t > tmp_collection_list;
	boost::shared_ptr< cql::cql_map_t >  tmp_collection_map;

	std::vector< cql::cql_byte_t > data;
	std::stringstream ss;

	cql::cql_column_type_enum type;
	obj->cql_result->column_type(column, type);
	orig_column_type = static_cast<long>(type);

	if (column_type == cql::CQL_COLUMN_TYPE_UNKNOWN) {
		column_type = orig_column_type;
	}

	switch (column_type) {

		case cql::CQL_COLUMN_TYPE_BIGINT:

			obj->cql_result->get_bigint(column, tmp_value_big_int);

			if (column_type != orig_column_type && tmp_value_big_int == 0) {
				RETURN_NULL();
				return;
			}

			RETURN_LONG(tmp_value_big_int);

		break;

		case cql::CQL_COLUMN_TYPE_BOOLEAN:

			obj->cql_result->get_bool(column, tmp_value_bool);

			if (column_type != orig_column_type && !tmp_value_bool) {
				RETURN_NULL();
				return;
			}

			RETURN_BOOL(tmp_value_bool);

		break;

		case cql::CQL_COLUMN_TYPE_DOUBLE:

			obj->cql_result->get_double(column, tmp_value_double);

			if (column_type != orig_column_type && tmp_value_double == 0) {
				RETURN_NULL();
				return;
			}

			RETURN_DOUBLE(tmp_value_double);

		break;

		case cql::CQL_COLUMN_TYPE_FLOAT:

			obj->cql_result->get_float(column, tmp_value_float);

			if (column_type != orig_column_type && tmp_value_float == 0) {
				RETURN_NULL();
				return;
			}

			RETURN_DOUBLE(tmp_value_float);

		break;

		case cql::CQL_COLUMN_TYPE_INT:

			obj->cql_result->get_int(column, tmp_value_int);

			if (column_type != orig_column_type && tmp_value_int == 0) {
				RETURN_NULL();
				return;
			}

			RETURN_LONG(tmp_value_int);

		break;

		case cql::CQL_COLUMN_TYPE_MAP:

			php_array_init(return_value);
			obj->cql_result->get_map(column, tmp_collection_map);

			ALLOC_INIT_ZVAL(tmp_zval);

			for (size_t i = 0; i < tmp_collection_map->size(); ++i) {

				// detect key type
				switch (tmp_collection_map->key_type()) {

					case cql::CQL_COLUMN_TYPE_BIGINT:
						tmp_collection_map->get_key_bigint(i, tmp_value_big_int);
						ZVAL_LONG(tmp_zval, tmp_value_big_int);
					break;

					case cql::CQL_COLUMN_TYPE_BOOLEAN:
						tmp_collection_map->get_key_bool(i, tmp_value_bool);
						ZVAL_BOOL(tmp_zval, tmp_value_bool);
					break;

					case cql::CQL_COLUMN_TYPE_DOUBLE:
						tmp_collection_map->get_key_double(i, tmp_value_double);
						ZVAL_DOUBLE(tmp_zval, tmp_value_double);
					break;

					case cql::CQL_COLUMN_TYPE_FLOAT:
						tmp_collection_map->get_key_float(i, tmp_value_float);
						ZVAL_DOUBLE(tmp_zval, tmp_value_float);
					break;

					case cql::CQL_COLUMN_TYPE_INT:
						tmp_collection_map->get_key_int(i, tmp_value_int);
						ZVAL_LONG(tmp_zval, tmp_value_int);
					break;

					case cql::CQL_COLUMN_TYPE_TIMESTAMP:
					case cql::CQL_COLUMN_TYPE_TIMEUUID:

						// Driver doesn't have method for getting timestamp [workaround]
						tmp_collection_map->get_key_bigint(i, tmp_value_big_int);

						if (tmp_value_big_int > 1000) {
							ts  = (time_t) (tmp_value_big_int / 1000);
						}
						else {
							ts = (time_t) 0;
						}

						tmp_key = php_format_date("Y-m-d H:i:s", 11, ts, 1 TSRMLS_CC);
						ZVAL_STRING(tmp_zval, tmp_key, 1);

					break;

					case cql::CQL_COLUMN_TYPE_TEXT:
					case cql::CQL_COLUMN_TYPE_VARCHAR:
						tmp_collection_map->get_key_string(i, tmp_value_string);
						ZVAL_STRING(tmp_zval, tmp_value_string.c_str(), 1);
					break;

				}

				if (tmp_zval->type != IS_STRING) {
					convert_to_string_ex(&tmp_zval);
				}

				tmp_key = Z_STRVAL_P(tmp_zval);

				// detect value type and add new element to array
				switch (tmp_collection_map->value_type()) {

					case cql::CQL_COLUMN_TYPE_BIGINT:
						tmp_collection_map->get_value_bigint(i, tmp_value_big_int);
						add_assoc_long(return_value, tmp_key, tmp_value_big_int);
					break;

					case cql::CQL_COLUMN_TYPE_BOOLEAN:
						tmp_collection_map->get_value_bool(i, tmp_value_bool);
						add_assoc_bool(return_value, tmp_key, tmp_value_bool);
					break;

					case cql::CQL_COLUMN_TYPE_DOUBLE:
						tmp_collection_map->get_value_double(i, tmp_value_double);
						add_assoc_double(return_value, tmp_key, tmp_value_double);
					break;

					case cql::CQL_COLUMN_TYPE_FLOAT:
						tmp_collection_map->get_value_float(i, tmp_value_float);
						add_assoc_double(return_value, tmp_key, tmp_value_float);
					break;

					case cql::CQL_COLUMN_TYPE_INT:
						tmp_collection_map->get_value_int(i, tmp_value_int);
						add_assoc_long(return_value, tmp_key, tmp_value_int);
					break;

					case cql::CQL_COLUMN_TYPE_TIMESTAMP:
					case cql::CQL_COLUMN_TYPE_TIMEUUID:

						// Driver doesn't have method for getting timestamp [workaround]
						tmp_collection_map->get_value_bigint(i, tmp_value_big_int);

						if (tmp_value_big_int > 1000) {
							ts  = (time_t) (tmp_value_big_int / 1000);
						}
						else {
							ts = (time_t) 0;
						}

						tmp_value_char = php_format_date("Y-m-d H:i:s", 11, ts, 1 TSRMLS_CC);
						add_assoc_string(return_value, tmp_key, tmp_value_char, 1);

					break;

					case cql::CQL_COLUMN_TYPE_TEXT:
					case cql::CQL_COLUMN_TYPE_VARCHAR:
						tmp_collection_map->get_value_string(i, tmp_value_string);
						add_assoc_string(return_value, tmp_key, (char *) tmp_value_string.c_str(), 1);
					break;

				}

			}

			return;

		break;

		case cql::CQL_COLUMN_TYPE_LIST:

			php_array_init(return_value);

			obj->cql_result->get_list(column, tmp_collection_list);

			for (size_t i = 0; i < tmp_collection_list->size(); ++i) {

				switch (tmp_collection_list->element_type()) {

					case cql::CQL_COLUMN_TYPE_BIGINT:
						tmp_collection_list->get_bigint(i, tmp_value_big_int);
						add_next_index_long(return_value, tmp_value_big_int);
					break;

					case cql::CQL_COLUMN_TYPE_BOOLEAN:
						tmp_collection_list->get_bool(i, tmp_value_bool);
						add_next_index_bool(return_value, tmp_value_bool);
					break;

					case cql::CQL_COLUMN_TYPE_DOUBLE:
						tmp_collection_list->get_double(i, tmp_value_double);
						add_next_index_double(return_value, tmp_value_double);
					break;

					case cql::CQL_COLUMN_TYPE_FLOAT:
						tmp_collection_list->get_float(i, tmp_value_float);
						add_next_index_double(return_value, tmp_value_float);
					break;

					case cql::CQL_COLUMN_TYPE_INT:
						tmp_collection_list->get_int(i, tmp_value_int);
						add_next_index_long(return_value, tmp_value_int);
					break;

					case cql::CQL_COLUMN_TYPE_VARCHAR:
					case cql::CQL_COLUMN_TYPE_TEXT:
						tmp_collection_list->get_string(i, tmp_value_string);
						add_next_index_string(return_value, tmp_value_string.c_str(), 1);
					break;

				}

			}

			return;

		break;

		case cql::CQL_COLUMN_TYPE_SET:

			php_array_init(return_value);

			obj->cql_result->get_set(column, tmp_collection_set);

			for (size_t i = 0; i < tmp_collection_set->size(); ++i) {

				switch (tmp_collection_set->element_type()) {

					case cql::CQL_COLUMN_TYPE_BIGINT:
						tmp_collection_set->get_bigint(i, tmp_value_big_int);
						add_next_index_long(return_value, tmp_value_big_int);
					break;

					case cql::CQL_COLUMN_TYPE_BOOLEAN:
						tmp_collection_set->get_bool(i, tmp_value_bool);
						add_next_index_bool(return_value, tmp_value_bool);
					break;

					case cql::CQL_COLUMN_TYPE_DOUBLE:
						tmp_collection_set->get_double(i, tmp_value_double);
						add_next_index_double(return_value, tmp_value_double);
					break;

					case cql::CQL_COLUMN_TYPE_FLOAT:
						tmp_collection_set->get_float(i, tmp_value_float);
						add_next_index_double(return_value, tmp_value_float);
					break;

					case cql::CQL_COLUMN_TYPE_INT:
						tmp_collection_set->get_int(i, tmp_value_int);
						add_next_index_long(return_value, tmp_value_int);
					break;

					case cql::CQL_COLUMN_TYPE_VARCHAR:
					case cql::CQL_COLUMN_TYPE_TEXT:
						tmp_collection_set->get_string(i, tmp_value_string);
						add_next_index_string(return_value, tmp_value_string.c_str(), 1);
					break;

				}

			}

			return;

		break;

		case cql::CQL_COLUMN_TYPE_TIMESTAMP:
		case cql::CQL_COLUMN_TYPE_TIMEUUID:

			// Driver doesn't have method for getting timestamp [workaround]
			obj->cql_result->get_bigint(column, tmp_value_big_int);

			if (column_type != orig_column_type && tmp_value_big_int == 0) {
				RETURN_NULL();
				return;
			}

			if (tmp_value_big_int > 1000) {
				ts  = (time_t) (tmp_value_big_int / 1000);
			}
			else {
				ts = (time_t) 0;
			}

			tmp_value_char = php_format_date("Y-m-d H:i:s", 11, ts, 1 TSRMLS_CC);
			RETURN_STRING(tmp_value_char, 1);

		break;

		case cql::CQL_COLUMN_TYPE_TEXT:
		case cql::CQL_COLUMN_TYPE_VARCHAR:

			obj->cql_result->get_string(column, tmp_value_string);

			if (tmp_value_string.length() < 1) {
				RETURN_NULL();
				return;
			}

			RETURN_STRING(tmp_value_string.c_str(), 1);

		break;

		case cql::CQL_COLUMN_TYPE_UUID:

			obj->cql_result->get_data(column, data);

			for (size_t i = 0; i < data.size(); ++i) {

				unsigned char * ch = reinterpret_cast<unsigned char *>(&data[i]);

				int c = (int) *ch;
				ss << std::setfill('0') << std::setw(2) << std::hex << c;

				if (i == 3 || i == 5 || i == 7 || i == 9) {
					ss << "-";
				}

			}

			tmp_value_string = ss.str();

			if (tmp_value_string.length() < 1) {
				RETURN_NULL();
				return;
			}

			RETURN_STRING(tmp_value_string.c_str(), 1);

		break;

	}

	RETURN_NULL();
}

PHP_METHOD(CqlResult, getColumnCount)
{
	cql_result_object *obj = (cql_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_LONG(obj->cql_result->column_count());
}

PHP_METHOD(CqlResult, getRowCount)
{
	cql_result_object *obj = (cql_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_LONG(obj->cql_result->row_count());
}

PHP_METHOD(CqlResult, next)
{
	cql_result_object *obj = (cql_result_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_BOOL(obj->cql_result->next());
}

/* {{{ cassandra_module_entry
 */
zend_module_entry cassandra_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"cassandra",
	cassandra_functions,
	PHP_MINIT(cassandra),
	PHP_MSHUTDOWN(cassandra),
	PHP_RINIT(cassandra),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(cassandra),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(cassandra),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CASSANDRA
BEGIN_EXTERN_C()
ZEND_GET_MODULE(cassandra)
END_EXTERN_C()
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("cassandra.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_cassandra_globals, cassandra_globals)
    STD_PHP_INI_ENTRY("cassandra.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_cassandra_globals, cassandra_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_cassandra_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_cassandra_init_globals(zend_cassandra_globals *cassandra_globals)
{
	cassandra_globals->global_value = 0;
	cassandra_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cassandra)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

	zend_class_entry ce;

	memcpy(&cql_handlers,               zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&cql_builder_handlers,       zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&cql_cluster_handlers,       zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&cql_error_handlers,         zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&cql_future_result_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&cql_query_handlers,         zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&cql_session_handlers,       zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&cql_result_handlers,        zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	// INIT Cql
	INIT_CLASS_ENTRY(ce, "Cql", php_cql_class_methods);
	php_cql_sc_entry = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_ANY",          sizeof("CQL_CONSISTENCY_ANY")-1,          cql::CQL_CONSISTENCY_ANY TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_ONE",          sizeof("CQL_CONSISTENCY_ONE")-1,          cql::CQL_CONSISTENCY_ONE TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_TWO",          sizeof("CQL_CONSISTENCY_TWO")-1,          cql::CQL_CONSISTENCY_TWO TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_THREE",        sizeof("CQL_CONSISTENCY_THREE")-1,        cql::CQL_CONSISTENCY_THREE TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_QUORUM",       sizeof("CQL_CONSISTENCY_QUORUM")-1,       cql::CQL_CONSISTENCY_QUORUM TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_ALL",          sizeof("CQL_CONSISTENCY_ALL")-1,          cql::CQL_CONSISTENCY_ALL TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_LOCAL_QUORUM", sizeof("CQL_CONSISTENCY_LOCAL_QUORUM")-1, cql::CQL_CONSISTENCY_LOCAL_QUORUM TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_EACH_QUORUM",  sizeof("CQL_CONSISTENCY_EACH_QUORUM")-1,  cql::CQL_CONSISTENCY_EACH_QUORUM TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_CONSISTENCY_DEFAULT",      sizeof("CQL_CONSISTENCY_DEFAULT")-1,      cql::CQL_CONSISTENCY_DEFAULT TSRMLS_CC);

	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_UNKNOWN",      sizeof("CQL_COLUMN_TYPE_UNKNOWN")-1,      cql::CQL_COLUMN_TYPE_UNKNOWN TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_CUSTOM",       sizeof("CQL_COLUMN_TYPE_CUSTOM")-1,       cql::CQL_COLUMN_TYPE_CUSTOM TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_ASCII",        sizeof("CQL_COLUMN_TYPE_ASCII")-1,        cql::CQL_COLUMN_TYPE_ASCII TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_BIGINT",       sizeof("CQL_COLUMN_TYPE_BIGINT")-1,       cql::CQL_COLUMN_TYPE_BIGINT TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_BLOB",         sizeof("CQL_COLUMN_TYPE_BLOB")-1,         cql::CQL_COLUMN_TYPE_BLOB TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_BOOLEAN",      sizeof("CQL_COLUMN_TYPE_BOOLEAN")-1,      cql::CQL_COLUMN_TYPE_BOOLEAN TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_COUNTER",      sizeof("CQL_COLUMN_TYPE_COUNTER")-1,      cql::CQL_COLUMN_TYPE_COUNTER TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_DECIMAL",      sizeof("CQL_COLUMN_TYPE_DECIMAL")-1,      cql::CQL_COLUMN_TYPE_DECIMAL TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_DOUBLE",       sizeof("CQL_COLUMN_TYPE_DOUBLE")-1,       cql::CQL_COLUMN_TYPE_DOUBLE TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_FLOAT",        sizeof("CQL_COLUMN_TYPE_FLOAT"),          cql::CQL_COLUMN_TYPE_FLOAT TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_INT",          sizeof("CQL_COLUMN_TYPE_INT")-1,          cql::CQL_COLUMN_TYPE_INT TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_TEXT",         sizeof("CQL_COLUMN_TYPE_TEXT")-1,         cql::CQL_COLUMN_TYPE_TEXT TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_TIMESTAMP",    sizeof("CQL_COLUMN_TYPE_TIMESTAMP")-1,    cql::CQL_COLUMN_TYPE_TIMESTAMP TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_UUID",         sizeof("CQL_COLUMN_TYPE_UUID")-1,         cql::CQL_COLUMN_TYPE_UUID TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_VARCHAR",      sizeof("CQL_COLUMN_TYPE_VARCHAR")-1,      cql::CQL_COLUMN_TYPE_VARCHAR TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_VARINT",       sizeof("CQL_COLUMN_TYPE_VARINT")-1,       cql::CQL_COLUMN_TYPE_VARINT TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_TIMEUUID",     sizeof("CQL_COLUMN_TYPE_TIMEUUID")-1,     cql::CQL_COLUMN_TYPE_TIMEUUID TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_INET",         sizeof("CQL_COLUMN_TYPE_INET")-1,         cql::CQL_COLUMN_TYPE_INET TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_LIST",         sizeof("CQL_COLUMN_TYPE_LIST")-1,         cql::CQL_COLUMN_TYPE_LIST TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_MAP",          sizeof("CQL_COLUMN_TYPE_MAP")-1,          cql::CQL_COLUMN_TYPE_MAP TSRMLS_CC);
	zend_declare_class_constant_long(php_cql_sc_entry, "CQL_COLUMN_TYPE_SET",          sizeof("CQL_COLUMN_TYPE_SET")-1,          cql::CQL_COLUMN_TYPE_SET TSRMLS_CC);

	// INIT CqlBuilder
	INIT_CLASS_ENTRY(ce, "CqlBuilder", php_cql_builder_class_methods);
	php_cql_builder_sc_entry                 = zend_register_internal_class(&ce TSRMLS_CC);
	php_cql_builder_sc_entry->create_object  = php_cql_builder_object_new;
	cql_builder_handlers.clone_obj           = NULL;

	zend_declare_class_constant_long(php_cql_builder_sc_entry, "DEFAULT_PORT", sizeof("DEFAULT_PORT")-1, cql::cql_builder_t::DEFAULT_PORT TSRMLS_CC);

	// INIT CqlCluster
	INIT_CLASS_ENTRY(ce, "CqlCluster", php_cql_cluster_class_methods);
	php_cql_cluster_sc_entry                 = zend_register_internal_class(&ce TSRMLS_CC);
	php_cql_cluster_sc_entry->create_object  = php_cql_cluster_object_new;
	cql_cluster_handlers.clone_obj           = NULL;

	// INIT CqlError
	INIT_CLASS_ENTRY(ce, "CqlError", php_cql_error_class_methods);
	php_cql_error_sc_entry                   = zend_register_internal_class(&ce TSRMLS_CC);
	php_cql_error_sc_entry->create_object    = php_cql_error_object_new;
	cql_cluster_handlers.clone_obj           = NULL;

	zend_declare_property_null(php_cql_error_sc_entry, "cassandra", sizeof("cassandra")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(php_cql_error_sc_entry, "code",      sizeof("code")-1,      ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(php_cql_error_sc_entry, "hasError",  sizeof("hasError")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(php_cql_error_sc_entry, "library",   sizeof("library")-1,   ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(php_cql_error_sc_entry, "message",   sizeof("message")-1,   ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(php_cql_error_sc_entry, "transport", sizeof("transport")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	// INIT CqlFutureResult
	INIT_CLASS_ENTRY(ce, "CqlFutureResult", php_cql_future_result_class_methods);
	php_cql_future_result_sc_entry                 = zend_register_internal_class(&ce TSRMLS_CC);
	php_cql_future_result_sc_entry->create_object  = php_cql_future_result_object_new;
	cql_future_result_handlers.clone_obj           = NULL;

	// INIT CqlQuery
	INIT_CLASS_ENTRY(ce, "CqlQuery", php_cql_query_class_methods);
	php_cql_query_sc_entry                   = zend_register_internal_class(&ce TSRMLS_CC);
	php_cql_query_sc_entry->create_object    = php_cql_query_object_new;
	cql_query_handlers.clone_obj             = NULL;

	zend_declare_property_long(php_cql_query_sc_entry, "consistency", sizeof("consistency")-1, cql::CQL_CONSISTENCY_DEFAULT, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(php_cql_query_sc_entry, "isTraced", sizeof("isTraced")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(php_cql_query_sc_entry, "queryString", sizeof("queryString")-1, "", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(php_cql_query_sc_entry, "retryPolicy", sizeof("retryPolicy")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

	// INIT CqlSession
	INIT_CLASS_ENTRY(ce, "CqlSession", php_cql_session_class_methods);
	php_cql_session_sc_entry                 = zend_register_internal_class(&ce TSRMLS_CC);
	php_cql_session_sc_entry->create_object  = php_cql_session_object_new;
	cql_session_handlers.clone_obj           = NULL;

	// INIT CqlResult
	INIT_CLASS_ENTRY(ce, "CqlResult", php_cql_result_class_methods);
	php_cql_result_sc_entry                  = zend_register_internal_class(&ce TSRMLS_CC);
	php_cql_result_sc_entry->create_object   = php_cql_result_object_new;
	cql_result_handlers.clone_obj            = NULL;

	cql::cql_initialize();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cassandra)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/

	cql::cql_terminate();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(cassandra)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(cassandra)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cassandra)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "cassandra support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
