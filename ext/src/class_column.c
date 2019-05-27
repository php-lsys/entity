


#include "zend.h"
#include "php.h"
#include "zend_API.h"

#include "entity.h"
#include "class_column.h"

ZEND_API zend_class_entry *lsentity_column_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_setdefault_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_setcomment_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, comment)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_settype_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_setspk_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, set_is_primary_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_setsal_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, set_allow_nullable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_compare_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, old_val)
    ZEND_ARG_INFO(0, new_val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_column_read_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(lsentity_column_copy_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, table, Column, 0)
ZEND_END_ARG_INFO()



ZEND_METHOD(lsentity_column_class, __construct){
    zend_string *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());

    zend_update_property_str(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_name"),val);
}

ZEND_METHOD(lsentity_column_class, name){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_name"),0,NULL),0,0);
}

ZEND_METHOD(lsentity_column_class, setComment){
    zend_string *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());
    zend_update_property_str(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_comment"),val);

    RETURN_ZVAL(getThis(),1,0);

}
ZEND_METHOD(lsentity_column_class, comment){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_comment"),0,NULL),0,0);
}

ZEND_METHOD(lsentity_column_class, useDefault){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_is_default"),0,NULL),0,0);
}
ZEND_METHOD(lsentity_column_class, setDefault){
    zend_string *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());


    zend_update_property_str(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_default"),val);
    zend_update_property_bool(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_is_default"),1);

    RETURN_ZVAL(getThis(),1,0);

}
ZEND_METHOD(lsentity_column_class, getDefault){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_default"),0,NULL),0,0);
}
ZEND_METHOD(lsentity_column_class, setType){
    zend_string *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());
    zend_update_property_str(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_type"),val);
    RETURN_ZVAL(getThis(),1,0);
}
ZEND_METHOD(lsentity_column_class, getType){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_type"),0,NULL),0,0);
}
ZEND_METHOD(lsentity_column_class, setAllowNull){
    zend_bool val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());
    zend_update_property_bool(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_is_nullable"),val);

    zval *def=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_default"),0,NULL);
    if(val){
        if(Z_TYPE_P(def)==IS_STRING&&!zend_is_true(def)){
            zend_update_property_null(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_default"));
        }
    }else{
        if(Z_TYPE_P(def)==IS_NULL){
            zend_update_property_string(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_default"),"");
        }
    }
    RETURN_ZVAL(getThis(),1,0);
}
ZEND_METHOD(lsentity_column_class, isAllowNull){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_is_nullable"),0,NULL),0,0);
}

ZEND_METHOD(lsentity_column_class, read){
    zend_string *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());
    RETURN_STR(val);
}
ZEND_METHOD(lsentity_column_class, __toString){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_name"),0,NULL),0,0);
}
ZEND_METHOD(lsentity_column_class, asArray){

    zval temp_array;
    array_init(&temp_array);

    zval *mzval;
    //zend_hash_str_add 比较坑 有些版本是宏　有些版本时函数
    mzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_name"),0,NULL);
    zend_hash_str_add(Z_ARR(temp_array),"name",sizeof("name")-1,mzval);
    mzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_type"),0,NULL);
    zend_hash_str_add(Z_ARR(temp_array),"name",sizeof("type")-1,mzval);
    mzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_default"),0,NULL);
    zend_hash_str_add(Z_ARR(temp_array),"default",sizeof("default")-1,mzval);
    mzval=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_is_nullable"),0,NULL);
    zend_hash_str_add(Z_ARR(temp_array),"allowNull",sizeof("allowNull")-1,mzval);

    RETURN_ZVAL(&temp_array,1,1);

}
ZEND_METHOD(lsentity_column_class, compare){
    zval *old_val,*new_val;
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(old_val)
        Z_PARAM_ZVAL(new_val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_NULL());
    if((Z_TYPE_P(old_val)!=IS_NULL&&Z_TYPE_P(old_val)==IS_NULL)
            ||(Z_TYPE_P(old_val)==IS_NULL&&Z_TYPE_P(old_val)!=IS_NULL))RETURN_FALSE;
    RETURN_BOOL(zend_is_identical(old_val,new_val));
}
ZEND_METHOD(lsentity_column_class, copy){
    zval *column;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_OBJECT_OF_CLASS_EX(column, lsentity_column_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *mzval;
    mzval=zend_read_property(Z_OBJCE_P(column),column,ZEND_STRL("_type"),0,NULL);
    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_type"),mzval);
    mzval=zend_read_property(Z_OBJCE_P(column),column,ZEND_STRL("_is_default"),0,NULL);
    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_is_default"),mzval);
    mzval=zend_read_property(Z_OBJCE_P(column),column,ZEND_STRL("_default"),0,NULL);
    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_default"),mzval);
    mzval=zend_read_property(Z_OBJCE_P(column),column,ZEND_STRL("_is_nullable"),0,NULL);
    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_is_nullable"),mzval);
    mzval=zend_read_property(Z_OBJCE_P(column),column,ZEND_STRL("_comment"),0,NULL);
    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_comment"),mzval);

    RETURN_ZVAL(getThis(),1,0);

}

static zend_function_entry lsentity_column_class_method[] = {
        ZEND_ME(lsentity_column_class,__construct, lsentity_column_construct_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,name, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,setComment, lsentity_column_setcomment_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,comment, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,useDefault, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,setDefault, lsentity_column_setdefault_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,getDefault, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,setType, lsentity_column_settype_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,getType, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,setAllowNull, lsentity_column_setsal_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,isAllowNull, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,compare, lsentity_column_compare_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,read, lsentity_column_read_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,asArray, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,copy, lsentity_column_copy_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_column_class,__toString, NULL, ZEND_ACC_PUBLIC)
        ZEND_FE_END
};

void lsentity_column_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Column",lsentity_column_class_method);
    lsentity_column_ce_ptr = zend_register_internal_class(&ce );
    zend_declare_property_null(lsentity_column_ce_ptr,ZEND_STRL("_name"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_column_ce_ptr,ZEND_STRL("_type"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_column_ce_ptr,ZEND_STRL("_is_default"),0, ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_column_ce_ptr,ZEND_STRL("_default"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_column_ce_ptr,ZEND_STRL("_is_nullable"),1, ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_column_ce_ptr,ZEND_STRL("_comment"),0, ZEND_ACC_PROTECTED );
}




