/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:   zqf  904208360@qq.com                                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_zqfHB.h"

/* If you declare any globals in php_zqfHB.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(zqfHB)
*/

/* True global resources - no need for thread safety here */
static int le_zqfHB;
ZEND_BEGIN_ARG_INFO_EX(zqfHB_hongbao_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0,zqfmoney)
    ZEND_ARG_INFO(0,zqfcount)
    ZEND_ARG_INFO(0,zqftype)
ZEND_END_ARG_INFO()
/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("zqfHB.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_zqfHB_globals, zqfHB_globals)
    STD_PHP_INI_ENTRY("zqfHB.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_zqfHB_globals, zqfHB_globals)
PHP_INI_END()
*/
/* }}} */
int my_rand(int min, int max)
{
    static int _seed = 0;
    assert(max > min);

    if (_seed == 0)
    {
        _seed = time(NULL);
        srand(_seed);
    }
    int _rand = rand();
    _rand = min + (int) ((double) ((double) (max) - (min) + 1.0) * ((_rand) / ((RAND_MAX) + 1.0)));
    return _rand;
}
/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_zqfHB_compiled(string arg)
   Return a string to confirm that the module is compiled in */

PHP_FUNCTION(hongbao)
{
	zval *zqfmoney;
  double moneyss;
  long zqfcount;
  long zqftype=0;
  const double min=0.01;
  double safe_total;
  double moneys;
  int i;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl|l", &zqfmoney,&zqfcount,&zqftype) == FAILURE) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING, "参数不正确!!!");
        RETURN_FALSE;
    }
    switch(Z_TYPE_P(zqfmoney)){
      case IS_LONG:
        moneyss=(double)Z_LVAL_P(zqfmoney);
        break;
      case IS_DOUBLE:
        moneyss=((int)Z_DVAL_P(zqfmoney)*100)/100.0;
       break;
      default:
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "参数不正确!!!");
        RETURN_FALSE;
        break;
    }
    array_init(return_value);
     if(zqftype){
        moneys=((int)(moneyss*100/zqfcount))/100.0;
        for (i = 0; i < zqfcount; ++i)
       {
          add_index_double(return_value,i,moneys);
        }
    }else{
      for (i = 1; i < zqfcount; ++i)
    {
      safe_total=(moneyss-(zqfcount-i)*min)/(zqfcount-i);
      moneys=my_rand((int)(min*100),(int)(safe_total*100))/100.0;
      moneyss -=moneys;
      add_index_double(return_value,i-1,moneys);
    }
  add_index_double(return_value,zqfcount-1,moneyss);
    }
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_zqfHB_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_zqfHB_init_globals(zend_zqfHB_globals *zqfHB_globals)
{
	zqfHB_globals->global_value = 0;
	zqfHB_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(zqfHB)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(zqfHB)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(zqfHB)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(zqfHB)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(zqfHB)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "zqfHB support", "enabled");
	php_info_print_table_row(2, "version", "1.0.0");
	php_info_print_table_row(2, "Author", "qieangel2013");
	php_info_print_table_row(2, "adress", "904208360@qq.com");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ zqfHB_functions[]
 *
 * Every user visible function must have an entry in zqfHB_functions[].
 */
const zend_function_entry zqfHB_functions[] = {
	PHP_FE(hongbao,	zqfHB_hongbao_arginfo)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in zqfHB_functions[] */
};
/* }}} */

/* {{{ zqfHB_module_entry
 */
zend_module_entry zqfHB_module_entry = {
	STANDARD_MODULE_HEADER,
	"zqfHB",
	zqfHB_functions,
	PHP_MINIT(zqfHB),
	PHP_MSHUTDOWN(zqfHB),
	PHP_RINIT(zqfHB),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(zqfHB),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(zqfHB),
	PHP_ZQFHB_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ZQFHB
ZEND_GET_MODULE(zqfHB)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
