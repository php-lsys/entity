




#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include "entity.h"
#include "utils.h"
#include "class_column_set.h"
#include "class_exception.h"
#include "class_column.h"



ZEND_API zend_class_entry *lsentity_column_set_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_set_construct_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, column,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_set_getname_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_set_add_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, column, Column, 0)
    ZEND_ARG_INFO(0, exist_copy)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(lsentity_column_set_asarray_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_column_set_class, __construct){
    zval *column,*object;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ARRAY_EX(column, 0, 1)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),&temp_array);
    zval_ptr_dtor(&temp_array);
    zval *val;
    ZEND_HASH_FOREACH_VAL(Z_ARR_P(column),val) {
        lsentity_obj_check(lsentity_column_ce_ptr,val,1);
        zend_call_method_with_1_params(object,Z_OBJCE_P(object),NULL,"add",NULL,val);
    } ZEND_HASH_FOREACH_END();
}
ZEND_METHOD(lsentity_column_set_class, getType){
    zval *name,*object;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());
    object=getThis();
    if(lsentity_check_bool_with_1_params(&object,"offsetexists",name)){
        zval column,type;
        zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "offsetget",&column,name);
        if(!zend_object_is_true(&column)){
            zval_ptr_dtor(&column);
            RETURN_NULL();
        }
        zend_call_method_with_1_params(&column,Z_OBJCE(column), NULL, "gettype",&type,name);
        RETURN_ZVAL(&type,1,1);
    }
    RETURN_NULL();
}
ZEND_METHOD(lsentity_column_set_class, add){
    zval *object, *column;
    zend_bool exist_copy = 0;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_OBJECT_OF_CLASS_EX(column, lsentity_column_ce_ptr, 0, 0)
            Z_PARAM_BOOL(exist_copy)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();
    zval name;
    zend_call_method_with_0_params(column,Z_OBJCE_P(column), NULL, "name",&name);
    if(Z_TYPE(name)!=IS_STRING){
        zval_ptr_dtor(&name);
        RETURN_ZVAL(object,0,0);
    }
    zval *columnarr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    zval * val=zend_hash_find(Z_ARR_P(columnarr),Z_STR(name));
    if(val&&exist_copy){
        zend_call_method_with_1_params(column,Z_OBJCE_P(column), NULL, "copy",NULL,val);
    }
    if(!val){
        zend_hash_add(Z_ARR_P(columnarr),Z_STR(name),column);
    }else{
        zend_hash_update(Z_ARR_P(columnarr),Z_STR(name),column);
    }
    zval_ptr_dtor(&name);
    RETURN_ZVAL(object,0,0);
}
ZEND_METHOD(lsentity_column_set_class, asArray){
    zend_long type;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object;
    object=getThis();
    zval *columnarr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    switch (type){
        case COLUMN_SET_TYPE_FIELD: {
            zval *input,				/* Input array */
                    *entry,
                    new_val;				/* New value */
            zend_bool strict = 0;		/* do strict comparison */
            zend_ulong num_idx;
            zend_string *str_idx;
            zend_array *arrval;
            zend_ulong elem_count;
            arrval = Z_ARRVAL_P(columnarr);
            elem_count = zend_hash_num_elements(arrval);
            if (!elem_count) {
                RETURN_ZVAL(columnarr, 1, 0)
            }
            array_init_size(return_value, elem_count);
            zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
            ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value))
                {
                    if (HT_IS_PACKED(arrval) && HT_IS_WITHOUT_HOLES(arrval)) {
                        ZVAL_LONG(&new_val, 0);
                        for (; (zend_ulong) Z_LVAL(new_val) < elem_count; ++Z_LVAL(new_val)) {
                            ZEND_HASH_FILL_ADD(&new_val);
                        }
                    } else {
                        ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(input), num_idx, str_idx, entry)
                                {
                                    if (str_idx) {
                                        ZVAL_STR_COPY(&new_val, str_idx);
                                    } else {
                                        ZVAL_LONG(&new_val, num_idx);
                                    }
                                    ZEND_HASH_FILL_ADD(&new_val);
                                }ZEND_HASH_FOREACH_END();
                    }
                }
            ZEND_HASH_FILL_END();
         }
            break;
        case COLUMN_SET_TYPE_DEFAULT:
            {
                zval *cval;
                zend_string *ckey;
                array_init(return_value);
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(columnarr),ckey,cval) {
                        zval tmp;
                        zend_call_method_with_0_params(&cval,Z_OBJCE_P(cval), NULL, "getdefault", &tmp);
                        zend_hash_add(Z_ARR_P(return_value),ckey,&tmp);
                        zval_ptr_dtor(&tmp);
                } ZEND_HASH_FOREACH_END();
            }
            break;
        case COLUMN_SET_TYPE_ARRAY:
            {
                zval *cval;
                zend_string *ckey;
                array_init(return_value);
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(columnarr),ckey,cval) {
                            zval tmp;
                            zend_call_method_with_0_params(&cval,Z_OBJCE_P(cval), NULL, "asarray", &tmp);
                            zend_hash_add(Z_ARR_P(return_value),ckey,&tmp);
                            zval_ptr_dtor(&tmp);
                        } ZEND_HASH_FOREACH_END();
            }
            break;
        default:
        {
            array_init(return_value);
        }
    }
}
ZEND_METHOD(lsentity_column_set_class, offsetExists){
    zend_string *name;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object;
    object=getThis();
    zval *columnarr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    RETURN_BOOL(zend_symtable_exists_ind(Z_ARR_P(columnarr),name));
}
ZEND_METHOD(lsentity_column_set_class, offsetUnset){
    zend_string *name;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object;
    object=getThis();
    zval *columnarr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    zend_hash_del(columnarr,name);
}
ZEND_METHOD(lsentity_column_set_class, offsetGet){
    zend_string *name;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object;
    object=getThis();
    zval *columnarr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    zval* oldval=zend_hash_find(Z_ARR_P(columnarr),name);
    if(!oldval) RETURN_NULL();
    RETURN_ZVAL(oldval,0,0);
}
ZEND_METHOD(lsentity_column_set_class, offsetSet){
    zend_string *name;
    zval *column;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(name)
            Z_PARAM_OBJECT_OF_CLASS_EX(column, lsentity_column_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object;
    object=getThis();
    zval *columnarr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    zval* oldval=zend_hash_find(Z_ARR_P(columnarr),name);
    if(!oldval){
        zend_hash_add(Z_ARR_P(columnarr),name,column);
    }else {
        zend_hash_update(Z_ARR_P(columnarr),name,column);
    }
}
ZEND_METHOD(lsentity_column_set_class, next){}
ZEND_METHOD(lsentity_column_set_class, valid){}
ZEND_METHOD(lsentity_column_set_class, current){}
ZEND_METHOD(lsentity_column_set_class, rewind){}
ZEND_METHOD(lsentity_column_set_class, key){}
ZEND_METHOD(lsentity_column_set_class, count){}


static zend_function_entry lsentity_column_set_class_method[] = {
    ZEND_ME(lsentity_column_set_class,__construct, lsentity_column_set_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,getType, lsentity_column_set_getname_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,add, lsentity_column_set_add_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,asArray, lsentity_column_set_asarray_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetExists, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetUnset, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetGet, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,offsetSet, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,next, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,valid, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_column_set_class,current, NULL, ZEND_ACC_PUBLIC)
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
    zend_declare_class_constant_long(lsentity_column_set_ce_ptr,ZEND_STRL("TYPE_FIELD"),COLUMN_SET_TYPE_FIELD);
    zend_declare_class_constant_long(lsentity_column_set_ce_ptr,ZEND_STRL("TYPE_ARRAY"),COLUMN_SET_TYPE_ARRAY);
    zend_declare_class_constant_long(lsentity_column_set_ce_ptr,ZEND_STRL("TYPE_DEFAULT"),COLUMN_SET_TYPE_DEFAULT);
}




