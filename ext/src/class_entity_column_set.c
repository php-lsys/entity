

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_entity_column_set.h"

ZEND_API zend_class_entry *lsentity_entity_column_set_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_column_set_construct_arginfo, 0, 0, 0)
    ZEND_ARG_ARRAY_INFO(0, column,1)
    ZEND_ARG_ARRAY_INFO(0, patch_columns,1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_column_set_ascs_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, table_columns, ColumnSet, 1)
    ZEND_ARG_INFO(0, patch)
ZEND_END_ARG_INFO()



ZEND_METHOD(lsentity_entity_column_set_class, __construct){
    zval *table_object,*object;
    zend_class_entry *table_class;
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() , getThis(), "|O",table_object,table_class) == FAILURE) {
        RETURN_FALSE;
    }
    object = getThis();
    zval temp_array;
    array_init(&temp_array);
    zend_update_property(lsentity_entity_column_set_ce_ptr,object,ZEND_STRL("_columns"),&temp_array);
    zend_update_property(lsentity_entity_column_set_ce_ptr,object,ZEND_STRL("_patch_columns"),&temp_array);
    zval_ptr_dtor(&temp_array);
}
ZEND_METHOD(lsentity_entity_column_set_class, isCustom){}
ZEND_METHOD(lsentity_entity_column_set_class, asColumnSet){}


static zend_function_entry lsentity_entity_column_set_class_method[] = {
    ZEND_ME(lsentity_entity_column_set_class,__construct, lsentity_entity_column_set_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_column_set_class,isCustom, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_column_set_class,asColumnSet, lsentity_entity_column_set_ascs_arginfo, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_entity_column_set_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"EntityColumnSet",lsentity_entity_column_set_class_method);
    lsentity_entity_column_set_ce_ptr = zend_register_internal_class(&ce );
    zend_declare_property_null(lsentity_entity_column_set_ce_ptr,ZEND_STRL("_columns"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_column_set_ce_ptr,ZEND_STRL("_patch_columns"), ZEND_ACC_PROTECTED );
}

