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
#include "php.h"
#include "php_ini.h"
#include "php_zqfHB.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_var.h"
#include <stdio.h>
#if PHP_MAJOR_VERSION <7 
#include "ext/standard/php_smart_str.h"
#include "./lib/php5/php_memcache.h"
#else
#include "ext/standard/php_smart_string.h"
//#include "ext/json/php_json.h"
#endif
#include <hiredis/hiredis.h>
#include "ext/standard/basic_functions.h"
#include "ext/standard/info.h"
#include <sys/time.h>

/* If you declare any globals in php_zqfHB.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(zqfHB)
*/
typedef struct _zqfHBEntry
{ 
  //自增序号
  //long id;

  //当前请求的GET
  zval *GET;

  //当前请求的POST
  zval *POST;

  //当前请求的COOKIE
  zval *COOKIE;

  //当前执行的PHP文件名
  zval *filename;

  //执行时间 1s=1000000
  char duration[100];

}zqfHBEntry;
redisReply* r;
redisContext *c;
/* True global resources - no need for thread safety here */
ZEND_DECLARE_MODULE_GLOBALS(zqfHB)
static int le_zqfHB;
static long ustime(void) {
  // struct timeval{
  //  long int tv_sec;  // 秒数
  //  long int tv_usec; // 微秒数
  // }
    struct timeval tv;
    long ust;

    //获得当前精确时间（UNIX到现在的时间）
    gettimeofday(&tv, NULL);
    ust = ((long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}
/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("zqfHB.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_zqfHB_globals, zqfHB_globals)
    STD_PHP_INI_ENTRY("zqfHB.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_zqfHB_globals, zqfHB_globals)
PHP_INI_END()
*/
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("zqfHB.slow_maxtime",      "0", PHP_INI_ALL, OnUpdateLong, slow_maxtime, zend_zqfHB_globals, zqfHB_globals)
    STD_PHP_INI_ENTRY("zqfHB.type",      "1", PHP_INI_ALL, OnUpdateLong, type, zend_zqfHB_globals, zqfHB_globals)
    STD_PHP_INI_ENTRY("zqfHB.auth",      "", PHP_INI_ALL, OnUpdateString, auth, zend_zqfHB_globals, zqfHB_globals)
    STD_PHP_INI_ENTRY("zqfHB.host",      "127.0.0.1", PHP_INI_ALL, OnUpdateString, host, zend_zqfHB_globals, zqfHB_globals)
    STD_PHP_INI_ENTRY("zqfHB.port",      "6379", PHP_INI_ALL, OnUpdateLong, port, zend_zqfHB_globals, zqfHB_globals)
PHP_INI_END()
/* }}} */
/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */
static void php_zqfHB_init_globals(zend_zqfHB_globals *zqfHB_globals)
{
  zqfHB_globals->slow_maxtime = 0;
}
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_zqfHB_compiled(string arg)
   Return a string to confirm that the module is compiled in */
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
#if PHP_MAJOR_VERSION <7 
static zval * global_query(uint type, char *name, uint len TSRMLS_DC){
  zval **globals, **ret;

  switch(type){
    case TRACK_VARS_POST:
      (void) zend_hash_find(&EG(symbol_table),ZEND_STRS("_POST"), (void **)&globals);
      break;
    case TRACK_VARS_GET:
      (void) zend_hash_find(&EG(symbol_table),ZEND_STRS("_GET"), (void **)&globals);
      break;
    case TRACK_VARS_COOKIE:
      globals = &PG(http_globals)[type];
      break;
    case TRACK_VARS_SERVER:
      zend_is_auto_global(ZEND_STRL("_SERVER") TSRMLS_CC);
      globals = &PG(http_globals)[type];
      break;
    default :
      break;
  }

  if (!globals || !(*globals)) {
    zval *empty;
    MAKE_STD_ZVAL(empty);
    ZVAL_NULL(empty);
    return empty;
  }

  //返回整个数组
  if(!len){
    Z_ADDREF_P(*globals);
    return *globals;
  }
  //返回数组中的键值
  if(zend_hash_find(Z_ARRVAL_PP(globals), name, len + 1, (void **)&ret) == FAILURE){
    zval *empty;
    MAKE_STD_ZVAL(empty);
    ZVAL_NULL(empty);
    return empty;
  }
  Z_ADDREF_P(*ret);
  return *ret;
}

static char* zqfHBEntry_to_string(zqfHBEntry *log){
php_serialize_data_t var_hash;
  zval *arr;
  smart_str buf = {0};
  MAKE_STD_ZVAL(arr);
  array_init(arr);
  add_assoc_zval(arr, "GET", log->GET);
  add_assoc_zval(arr, "POST", log->POST);
  add_assoc_zval(arr, "COOKIE", log->COOKIE);
  //add_assoc_long(arr, "id", log->id);
  add_assoc_stringl(arr, "duration", log->duration, strlen(log->duration), 1);
  add_assoc_zval(arr, "filename", log->filename);
  Z_ADDREF_P(log->POST);
  Z_ADDREF_P(log->GET);
  Z_ADDREF_P(log->COOKIE);
  Z_ADDREF_P(log->filename);
  PHP_VAR_SERIALIZE_INIT(var_hash);
  php_var_serialize(&buf, &arr, &var_hash TSRMLS_CC);
  PHP_VAR_SERIALIZE_DESTROY(var_hash);
  if(buf.c){
    return buf.c;
  }else{
    return NULL;
  }
}

#else
static zval * global_query_p7(uint type, char *name, uint len){
  zval *globals, *ret;

  switch(type){
    case TRACK_VARS_POST:
      globals=zend_hash_str_find(&EG(symbol_table),ZEND_STRS("_POST"));
      //globals = &PG(http_globals)[type];
      break;
    case TRACK_VARS_GET:
      globals=zend_hash_str_find(&EG(symbol_table),ZEND_STRS("_GET"));
      //globals = &PG(http_globals)[type];
      break;
    case TRACK_VARS_COOKIE:
      globals=zend_hash_str_find(&EG(symbol_table),ZEND_STRS("_COOKIE"));
      //globals = &PG(http_globals)[type];
      break;
    case TRACK_VARS_SERVER:
      globals = &PG(http_globals)[type];
      break;
    default :
      break;
  }
  if (!globals) {
    zval empty;
    return &empty;
  }

  if (!len) {
    return globals;
  }

  if((ret=zend_hash_str_find(Z_ARRVAL_P(globals), name,len)) == NULL){
    return NULL;
  }
  return ret;
}

static char* zqfHBEntry_to_string_p7(zqfHBEntry *log){
  php_serialize_data_t var_hash;
  smart_str buf = {0};
  zval arr;
    array_init(&arr);
    add_assoc_zval(&arr, "GET", log->GET);
    add_assoc_zval(&arr, "POST", log->POST);
    add_assoc_zval(&arr, "COOKIE", log->COOKIE);
    //add_assoc_long(&arr, "id", log->id);
    add_assoc_string(&arr, "duration", log->duration);
    add_assoc_zval(&arr, "filename", log->filename);
    Z_TRY_ADDREF_P(log->GET);
    Z_TRY_ADDREF_P(log->POST);
    Z_TRY_ADDREF_P(log->COOKIE);
    Z_TRY_ADDREF_P(log->filename);
  PHP_VAR_SERIALIZE_INIT(var_hash);
  php_var_serialize(&buf, &arr, &var_hash);
  PHP_VAR_SERIALIZE_DESTROY(var_hash);
  //php_json_encode(&buf, &arr, 0 TSRMLS_CC);
  //php_printf("get的buttf：'%s' \r\n",log->filename);
  if(buf.s){
    return estrndup(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
    //return buf.s;
  }
  smart_str_free(&buf);
  return NULL;
}

#endif

/*static void memcazqf(long duration,char *host ,long port){
  zend_function *func;
   zval *return_value;
    mmc_pool_t *pool;
    char *value, zqfHB_idx[100];
    mmc_t *mmc;
    int errnum;
    char *error_string;

    MAKE_STD_ZVAL(return_value);
    mmc = mmc_server_new(host, strlen(host), port, 0, 1, MMC_DEFAULT_RETRY TSRMLS_CC);
    mmc->timeout = 1;
    mmc->connect_timeoutms = 1000;

    if(mmc_open(mmc, 1, &error_string, &errnum TSRMLS_CC)){
      zqfHBEntry current_log;
      char new_slowkey[100];
      char *strval;

      //获取$_SERVER['PHP_SELF']
      current_log.filename = global_query(TRACK_VARS_SERVER, "PHP_SELF", sizeof("PHP_SELF") - 1);
      //获取$_COOKIE
      current_log.COOKIE   = global_query(TRACK_VARS_COOKIE, NULL, 0);
      //获取$_GET
      current_log.GET   = global_query(TRACK_VARS_GET, NULL, 0);
      //获取$_POST
      current_log.POST   = global_query(TRACK_VARS_POST, NULL, 0);

      pool = mmc_pool_new(TSRMLS_C);
      mmc_pool_add(pool, mmc, 1);

      sprintf(current_log.duration, "%ld", duration);

      mmc_exec_retrieval_cmd(pool, "zqfHB_idx", 11, &return_value, NULL TSRMLS_CC);

      convert_to_long(return_value);
      current_log.id = Z_LVAL_P(return_value)+1;
      sprintf(new_slowkey, "slowlog_%ld", current_log.id);
      sprintf(zqfHB_idx, "%ld", current_log.id);

      strval = slowlogEntry_to_string(&current_log);

      //将slowlogEntry序列化后存储到memcache
      mmc_pool_store(
          pool, "set", 3, new_slowkey, strlen(new_slowkey), 0, 0, strval, strlen(strval) TSRMLS_CC);

      //设置自增号
      mmc_pool_store(
          pool, "set", 3, "zqfHB_idx", 11, 0, 0, 
          zqfHB_idx, strlen(zqfHB_idx) TSRMLS_CC);
  #ifdef DEBUG
      php_printf("insert zqfHB, key : %s\n", new_slowkey);
  #endif
      
    }

}
*/

static void rediszqf(long duration){
    zend_function *func;
    char *value;
    char tmpzqf[256];
    zqfHBEntry current_log;
    char *strval;        
      //获取$_SERVER['PHP_SELF']
      #if PHP_MAJOR_VERSION <7 
      current_log.filename = global_query(TRACK_VARS_SERVER, "PHP_SELF", sizeof("PHP_SELF") - 1);
      //获取$_COOKIE
      current_log.COOKIE   = global_query(TRACK_VARS_COOKIE, NULL, 0);
      //获取$_GET
      current_log.GET   = global_query(TRACK_VARS_GET, NULL, 0);
      //获取$_POST
      current_log.POST   = global_query(TRACK_VARS_POST, NULL, 0);
      sprintf(current_log.duration, "%ld", duration);
      strval = zqfHBEntry_to_string(&current_log);
      #else
      current_log.filename = global_query_p7(TRACK_VARS_SERVER, "PHP_SELF", sizeof("PHP_SELF") - 1);
           //获取$_COOKIE
      current_log.COOKIE   = global_query_p7(TRACK_VARS_COOKIE, NULL, 0);
      //获取$_GET
      current_log.GET   = global_query_p7(TRACK_VARS_GET, NULL,0);
      //获取$_POST
      current_log.POST   = global_query_p7(TRACK_VARS_POST, NULL, 0);
      sprintf(current_log.duration, "%ld", duration);
      strval = zqfHBEntry_to_string_p7(&current_log);
      #endif
      if(ZQFHB_G(auth)[0] !='\0'){
        sprintf(tmpzqf, "auth %s",ZQFHB_G(auth));
        r = (redisReply*)redisCommand(c,tmpzqf);
        //php_printf("get的redis的AUTH：'%s' \r\n",tmpzqf);
      }
      sprintf(tmpzqf, "lpush zqfHB %s",strval);
      //php_printf("get的redis的COMAND：'%s' \r\n",tmpzqf);
      r = (redisReply*)redisCommand(c,tmpzqf);
      freeReplyObject(r);  
      #if PHP_MAJOR_VERSION <7 
      efree(tmpzqf);
      #endif
  }

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
	/* If you have INI entries, uncomment these lines */
	REGISTER_INI_ENTRIES();
  c = redisConnect(ZQFHB_G(host),ZQFHB_G(port));
    if (c->err)  
    {  
        redisFree(c);
        //php_error_docref(NULL TSRMLS_CC, E_WARNING,"Connect to redisServer faile\n");
    }  
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(zqfHB)
{
	/* uncomment this line if you have INI entries*/
	UNREGISTER_INI_ENTRIES();
	redisFree(c);
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(zqfHB)
{
  ZQFHB_G(start) = ustime();
	return SUCCESS;
}
/* }}} */



/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(zqfHB)
{
  long end = ustime(), duration;

  duration = end - ZQFHB_G(start);

  if(duration > ZQFHB_G(slow_maxtime)){
    switch(ZQFHB_G(type)){
      case 1:
        rediszqf(duration);
        break;
      case 2:
        //memcazqf(duration,ZQFHB_G(host),ZQFHB_G(port));
       break;
      default:
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "参数不正确!!!");
        break;
    }
   }
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
  DISPLAY_INI_ENTRIES();
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
