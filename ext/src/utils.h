
#ifndef PHP_LSENTITY_UTILS_H
#define PHP_LSENTITY_UTILS_H
#include "zend_API.h"
int lsentity_new_class(zval *class_name,zval *return_value,zval *params,int num_args);
int lsentity_throw_exception(zval *table,int code,const char *fentityat,zval * values);
#endif