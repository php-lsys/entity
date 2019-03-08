

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_entity_set.h"

ZEND_API zend_class_entry *lsentity_entity_set_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_set_construct_arginfo, 0, 0, 2)
    ZEND_ARG_OBJ_INFO_ENTITYDBNS(0, result, Result, 0)
    ZEND_ARG_INFO(0, entity_name)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, columns, EntityColumnSet, 1)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, table, Table, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_set_asarr_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_METHOD(lsentity_entity_set_class, __construct){
    zval *table_object,*object;
    zend_class_entry *table_class;
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() , getThis(), "|O",table_object,table_class) == FAILURE) {
        RETURN_FALSE;
    }
    object = getThis();
    zend_update_property(lsentity_entity_set_ce_ptr,object,ZEND_STRL("_loaded"),table_object);
}
ZEND_METHOD(lsentity_entity_set_class, offsetExists){}
ZEND_METHOD(lsentity_entity_set_class, offsetGet){}
ZEND_METHOD(lsentity_entity_set_class, offsetSet){}
ZEND_METHOD(lsentity_entity_set_class, offsetUnset){}
ZEND_METHOD(lsentity_entity_set_class, key){}
ZEND_METHOD(lsentity_entity_set_class, next){}
ZEND_METHOD(lsentity_entity_set_class, rewind){}
ZEND_METHOD(lsentity_entity_set_class, asArray){}
ZEND_METHOD(lsentity_entity_set_class, seek){}
ZEND_METHOD(lsentity_entity_set_class, current){}
ZEND_METHOD(lsentity_entity_set_class, count){}

static zend_function_entry lsentity_entity_set_class_method[] = {
    ZEND_ME(lsentity_entity_set_class,__construct, lsentity_entity_set_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,asArray, lsentity_entity_set_asarr_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,offsetExists, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,offsetGet, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,offsetUnset, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,key, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,next, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,rewind, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,seek, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,current, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,count, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

void lsentity_entity_set_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"EntitySet",lsentity_entity_set_class_method);
    lsentity_entity_set_ce_ptr = zend_register_internal_class(&ce);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_result"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_columns"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_table"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_entity"), ZEND_ACC_PROTECTED);
}

