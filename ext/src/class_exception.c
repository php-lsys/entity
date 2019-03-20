
#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "zend_exceptions.h"

#include "entity.h"
#include "class_exception.h"
ZEND_API zend_class_entry *lsentity_exception_ce_ptr;


ZEND_BEGIN_ARG_INFO_EX(lsentity_exception_errsql_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, sql)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_exception_vaild_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, error,0)
ZEND_END_ARG_INFO()

ZEND_METHOD(lsentity_exception_class, __construct){}
ZEND_METHOD(lsentity_exception_class, setErrorSql){}
ZEND_METHOD(lsentity_exception_class, setVaildationError){}
ZEND_METHOD(lsentity_exception_class, getErrorSql){}
ZEND_METHOD(lsentity_exception_class, getVaildationError){}


static zend_function_entry lsentity_exception_class_method[] = {
    ZEND_ME(lsentity_exception_class,__construct, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_exception_class,setErrorSql, lsentity_exception_errsql_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_exception_class,setVaildationError, lsentity_exception_vaild_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_exception_class,getErrorSql, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_exception_class,getVaildationError, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};



void lsentity_exception_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Exception",lsentity_exception_class_method);
    lsentity_exception_ce_ptr = zend_register_internal_class_ex(&ce, zend_ce_exception);
    zend_declare_property_null(lsentity_exception_ce_ptr,ZEND_STRL("_error_sql"), ZEND_ACC_PRIVATE );
    zend_declare_property_null(lsentity_exception_ce_ptr,ZEND_STRL("_validation_error"), ZEND_ACC_PRIVATE );
}