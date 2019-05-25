
#ifndef PHP_LSENTITY_UTILS_H
#define PHP_LSENTITY_UTILS_H
#include "zend_API.h"
int lsentity_new_class(zend_class_entry *ce,zval *return_value,zval *params,int num_args);
int lsentity_check_bool_with_0_params(zval *object,const char* method);
int lsentity_check_bool_with_1_params(zval *object,const char* method,zval *param);
int lsentity_check_bool_with_2_params(zval *object,const char* method,zval *param1,zval *param2);
int lsentity_obj_check(zend_class_entry * ce,zval *retobj,int throw);
int lsentity_in_array(zval *array,zval *value);
#endif