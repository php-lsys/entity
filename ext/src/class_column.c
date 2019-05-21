


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

}

ZEND_METHOD(lsentity_column_class, name){

}

ZEND_METHOD(lsentity_column_class, setComment){}
ZEND_METHOD(lsentity_column_class, comment){}

ZEND_METHOD(lsentity_column_class, useDefault){}
ZEND_METHOD(lsentity_column_class, setDefault){}
ZEND_METHOD(lsentity_column_class, getDefault){}
ZEND_METHOD(lsentity_column_class, setType){}
ZEND_METHOD(lsentity_column_class, getType){}
ZEND_METHOD(lsentity_column_class, setAllowNull){}
ZEND_METHOD(lsentity_column_class, isAllowNull){}
ZEND_METHOD(lsentity_column_class, compare){}
ZEND_METHOD(lsentity_column_class, copy){}
ZEND_METHOD(lsentity_column_class, read){}
ZEND_METHOD(lsentity_column_class, __toString){}
ZEND_METHOD(lsentity_column_class, asArray){}


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
    zend_declare_property_bool(lsentity_column_ce_ptr,ZEND_STRL("_is_nullable"),0, ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_column_ce_ptr,ZEND_STRL("_comment"),0, ZEND_ACC_PROTECTED );
}




