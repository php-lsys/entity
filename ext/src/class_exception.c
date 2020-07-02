

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"
#include "ext/standard/php_string.h"


#include "entity.h"
#include "class_exception.h"
zend_class_entry *lsentity_exception_ce_ptr;


ZEND_BEGIN_ARG_INFO_EX(lsentity_exception_errsql_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, sql)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_exception_vaild_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, error,0)
ZEND_END_ARG_INFO()

ZEND_METHOD(lsentity_exception_class, setErrorSql){


    zend_string *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());
    zend_update_property_str(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_error_sql"),val);
    zval arr;
    array_init(&arr);
    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_validation_error"),&arr);
    zval_ptr_dtor(&arr);
    RETURN_ZVAL(getThis(),1,0);


}
ZEND_METHOD(lsentity_exception_class, setValidationError){


    zval *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());

    zval *entry;
    zend_string *glue;
    glue= zend_string_init(ZEND_STRL(" "), 0);
    zval *msg=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("message"),0,NULL);
    smart_str err = {0};
    smart_str_append(&err, Z_STR_P(msg));
    smart_str_appends(&err, " : ");
    int start=1;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(val), entry) {
        if(Z_TYPE_P(entry)==IS_STRING){
            if(!start)smart_str_appends(&err, ";");
            if(start)start=0;
            smart_str_append(&err, Z_STR_P(entry));
            smart_str_append(&err,glue);
        }
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&err);
    zend_update_property_str(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("message"),err.s);
    smart_str_free(&err);
    zend_string_release(glue);

    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_validation_error"),val);


    RETURN_ZVAL(getThis(),1,0);


}
ZEND_METHOD(lsentity_exception_class, getErrorSql){
    zval *val=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_error_sql"),0,NULL);
    zval tmp;
    ZVAL_DUP(&tmp,val);
    RETURN_ZVAL(&tmp,1,1);
}
ZEND_METHOD(lsentity_exception_class, getValidationError){
    zval *val=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_validation_error"),0,NULL);
    zval tmp;
    ZVAL_DUP(&tmp,val);
    RETURN_ZVAL(&tmp,1,1);
}


static zend_function_entry lsentity_exception_class_method[] = {
    ZEND_ME(lsentity_exception_class,setErrorSql, lsentity_exception_errsql_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_exception_class,setValidationError, lsentity_exception_vaild_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_exception_class,getErrorSql, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_exception_class,getValidationError, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};



void lsentity_exception_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Exception",lsentity_exception_class_method);
    lsentity_exception_ce_ptr = zend_register_internal_class_ex(&ce, zend_ce_exception);
    zend_declare_property_null(lsentity_exception_ce_ptr,ZEND_STRL("_error_sql"), ZEND_ACC_PRIVATE );
    zend_declare_property_null(lsentity_exception_ce_ptr,ZEND_STRL("_validation_error"), ZEND_ACC_PRIVATE );
}