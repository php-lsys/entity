

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_validation.h"

ZEND_API zend_class_entry *lsentity_validation_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_construct_arginfo, 0, 0, 1)
                ZEND_ARG_OBJ_INFO_ENTITYNS(0, entity, Entity, 1)
                ZEND_ARG_ARRAY_INFO(0, rule_gruop,0)
                ZEND_ARG_INFO(0, allow_cache)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_label_arginfo, 0, 0, 2)
            ZEND_ARG_INFO(0, field)
            ZEND_ARG_INFO(0, label)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_labels_arginfo, 0, 0, 1)
        ZEND_ARG_ARRAY_INFO(0, labels,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rule_arginfo, 0, 0, 1)
                ZEND_ARG_OBJ_INFO_ENTITYNS(0, rule, ValidRule, 1)
                ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rules_arginfo, 0, 0, 1)
                ZEND_ARG_ARRAY_INFO(0, rules,0)
                ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_valid_arginfo, 0, 0, 1)
                ZEND_ARG_ARRAY_INFO(0, check_data,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_error_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, field)
                ZEND_ARG_INFO(0, error)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_validation_class, __construct){
    zval *table_object,*object;
    zend_class_entry *table_class;
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() , getThis(), "|O",table_object,table_class) == FAILURE) {
        RETURN_FALSE;
    }
    object = getThis();
    zend_update_property(lsentity_validation_ce_ptr,object,ZEND_STRL("_loaded"),table_object);

}
ZEND_METHOD(lsentity_validation_class, allowCache){}
ZEND_METHOD(lsentity_validation_class, label){}
ZEND_METHOD(lsentity_validation_class, labels){}
ZEND_METHOD(lsentity_validation_class, rule){}
ZEND_METHOD(lsentity_validation_class, rules){}
ZEND_METHOD(lsentity_validation_class, valid){}
ZEND_METHOD(lsentity_validation_class, error){}
ZEND_METHOD(lsentity_validation_class, errors){}


static zend_function_entry lsentity_validation_class_method[] = {
    ZEND_ME(lsentity_validation_class,__construct, lsentity_validation_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,allowCache, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,label, lsentity_validation_label_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,labels, lsentity_validation_labels_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,rule, lsentity_validation_rule_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,rules, lsentity_validation_rules_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,valid, lsentity_validation_valid_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,error, lsentity_validation_error_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,errors, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_validation_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Validation",lsentity_validation_class_method);
    lsentity_validation_ce_ptr = zend_register_internal_class(&ce );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_entity"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_global_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_labels"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_errors"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_allow_cache"), ZEND_ACC_PROTECTED );
}

