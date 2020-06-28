

#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "utils.h"
#include "class_entity_column_set.h"
#include "class_column.h"
#include "class_column_set.h"
#include "class_exception.h"

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
    zval *object,*column=NULL,*patch_columns=NULL;
    ZEND_PARSE_PARAMETERS_START(0, 2)
            Z_PARAM_OPTIONAL
            Z_PARAM_ARRAY(column)
            Z_PARAM_ARRAY(patch_columns)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();

    if(column){
        zval *tmp;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(column), tmp) {
            convert_to_string(tmp);
        } ZEND_HASH_FOREACH_END();
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),column);
    }

    if(patch_columns){
        zval *tmp;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(patch_columns), tmp) {
            if(!lsentity_obj_check(lsentity_column_ce_ptr,tmp,1,0)){
                RETURN_FALSE;
            }
        } ZEND_HASH_FOREACH_END();
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_patch_columns"),patch_columns);
    }else{
        zval _column;
        array_init(&_column);
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_patch_columns"),&_column);
        zval_ptr_dtor(&_column);
    }
}
ZEND_METHOD(lsentity_entity_column_set_class, notCustom){
    zval *mzval,*pzval;
    mzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_columns"),0,NULL);
    pzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_patch_columns"),0,NULL);
    RETURN_BOOL(Z_TYPE_P(mzval)!=IS_ARRAY&&zend_array_count(Z_ARR_P(pzval))==0);
}
ZEND_METHOD(lsentity_entity_column_set_class, asColumnSet){

    zend_bool patch=0;
    zval *column;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_OBJECT_OF_CLASS(column, lsentity_column_set_ce_ptr)
            Z_PARAM_OPTIONAL
            Z_PARAM_BOOL(patch)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *mzval,*pzval;
    mzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_columns"),0,NULL);
    if(Z_TYPE_P(mzval)!=IS_ARRAY)RETURN_ZVAL(column,1,0);

    zval columntype;
    array_init(&columntype);

    zval *tmp;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(mzval), tmp) {

        if(lsentity_check_bool_with_1_params(column,"offsetexists",tmp)){
            zval param1;
            zend_call_method_with_1_params(column,Z_OBJCE_P(column), NULL, "offsetget", &param1,tmp);
            Z_REFCOUNTED(param1)&&Z_ADDREF_P(&param1);
            zend_hash_add(Z_ARR(columntype),Z_STR_P(tmp),&param1);
            zval_ptr_dtor(&param1);
        }

    } ZEND_HASH_FOREACH_END();
    if(patch){
        pzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_patch_columns"),0,NULL);
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pzval), tmp) {
            zval param1;
            zend_call_method_with_0_params(tmp,Z_OBJCE_P(tmp), NULL, "name", &param1);
            Z_REFCOUNTED_P(tmp)&&Z_ADDREF_P(tmp);
            zend_hash_add(Z_ARR(columntype),Z_STR(param1),tmp);
            zval_ptr_dtor(&param1);
        } ZEND_HASH_FOREACH_END();

    }
    zval param[]={
            columntype
    };
    if(!lsentity_new_class(lsentity_column_set_ce_ptr,return_value,param,1)){
        zval_ptr_dtor(&columntype);
        RETURN_NULL();
    }
    zval_ptr_dtor(&columntype);
}


static zend_function_entry lsentity_entity_column_set_class_method[] = {
    ZEND_ME(lsentity_entity_column_set_class,__construct, lsentity_entity_column_set_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_column_set_class,notCustom, NULL, ZEND_ACC_PUBLIC)
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

