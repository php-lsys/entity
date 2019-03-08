

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_validation_rule_callback.h"
#include "class_validation_rule.h"

ZEND_API zend_class_entry *lsentity_validation_rule_callback_ce_ptr;


ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rule_callback_construct_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, fun)
    ZEND_ARG_INFO(0, allow_null)
    ZEND_ARG_ARRAY_INFO(0, param,0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rule_callback_i18n_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, fun)
    ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_validation_rule_callback_class, __construct){
    zval *table_object,*object;
    zend_class_entry *table_class;
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() , getThis(), "|O",table_object,table_class) == FAILURE) {
        RETURN_FALSE;
    }
}
ZEND_METHOD(lsentity_validation_rule_callback_class, i18n){

}
ZEND_METHOD(lsentity_validation_rule_callback_class, allowEmpty){

}
ZEND_METHOD(lsentity_validation_rule_callback_class, check){

}

static zend_function_entry lsentity_validation_rule_callback_class_method[] = {
    ZEND_ME(lsentity_validation_rule_callback_class,__construct, lsentity_validation_rule_callback_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_rule_callback_class,i18n, lsentity_validation_rule_callback_i18n_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(lsentity_validation_rule_callback_class,allowEmpty, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_rule_callback_class,check, lsentity_validation_rule_check_arginfo, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_validation_rule_callback_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_VALIDATIONRULE_NS,"Callback",lsentity_validation_rule_callback_class_method);
    lsentity_validation_rule_callback_ce_ptr = zend_register_internal_class(&ce);

    zend_class_implements(lsentity_validation_rule_callback_ce_ptr,1,lsentity_validation_rule_ce_ptr);

    zend_declare_property_null(lsentity_validation_rule_callback_ce_ptr,ZEND_STRL("_fun"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_rule_callback_ce_ptr,ZEND_STRL("_param"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_validation_rule_callback_ce_ptr,ZEND_STRL("_allow_null"),0, ZEND_ACC_PROTECTED );
}

