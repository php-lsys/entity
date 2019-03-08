




#include "zend.h"
#include "php.h"
#include "zend_API.h"

#include "entity.h"
#include "class_column_set.h"

ZEND_API zend_class_entry *lsentity_column_set_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_set_construct_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, column,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_set_getname_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_set_asarray_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_column_set_class, __construct){

}
ZEND_METHOD(lsentity_column_set_class, getType){}
ZEND_METHOD(lsentity_column_set_class, asArray){}
ZEND_METHOD(lsentity_column_set_class, offsetExists){}
ZEND_METHOD(lsentity_column_set_class, offsetUnset){}
ZEND_METHOD(lsentity_column_set_class, offsetGet){}
ZEND_METHOD(lsentity_column_set_class, offsetSet){}
ZEND_METHOD(lsentity_column_set_class, next){}
ZEND_METHOD(lsentity_column_set_class, valid){}
ZEND_METHOD(lsentity_column_set_class, current){}
ZEND_METHOD(lsentity_column_set_class, read){}
ZEND_METHOD(lsentity_column_set_class, rewind){}
ZEND_METHOD(lsentity_column_set_class, key){}
ZEND_METHOD(lsentity_column_set_class, count){}


static zend_function_entry lsentity_column_set_class_method[] = {
    ZEND_ME(lsentity_column_set_class,__construct, lsentity_column_set_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,getType, lsentity_column_set_getname_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,asArray, lsentity_column_set_asarray_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetExists, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetUnset, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetGet, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetSet, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,next, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,valid, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,current, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,read, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,rewind, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,key, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,count, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

void lsentity_column_set_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"ColumnSet",lsentity_column_set_class_method);
    lsentity_column_set_ce_ptr = zend_register_internal_class(&ce);
    zend_declare_property_null(lsentity_column_set_ce_ptr,ZEND_STRL("_columns"), ZEND_ACC_PROTECTED);
}




