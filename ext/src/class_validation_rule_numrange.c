

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_validation_rule.h"
#include "class_validation_rule_numrange.h"

ZEND_API zend_class_entry *lsentity_validation_rule_numrange_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rule_numrange_construct_arginfo, 0, 0, 3)
    ZEND_ARG_INFO(0,min)
    ZEND_ARG_INFO(0,max)
    ZEND_ARG_INFO(0,allow_empty)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_validation_rule_numrange_class, __construct){

}
ZEND_METHOD(lsentity_validation_rule_numrange_class, check){

}
ZEND_METHOD(lsentity_validation_rule_numrange_class, allowEmpty){

}

static zend_function_entry lsentity_validation_rule_numrange_class_method[] = {
    ZEND_ME(lsentity_validation_rule_numrange_class,__construct, lsentity_validation_rule_numrange_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_rule_numrange_class,check, lsentity_validation_rule_check_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_rule_numrange_class,allowEmpty, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_validation_rule_numrange_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_VALIDATIONRULE_NS,"NumRange",lsentity_validation_rule_numrange_class_method);
    lsentity_validation_rule_numrange_ce_ptr = zend_register_internal_class(&ce);
    zend_class_implements(lsentity_validation_rule_numrange_ce_ptr,1,lsentity_validation_rule_ce_ptr);
    zend_declare_property_null(lsentity_validation_rule_numrange_ce_ptr,ZEND_STRL("_min"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_rule_numrange_ce_ptr,ZEND_STRL("_max"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_rule_numrange_ce_ptr,ZEND_STRL("_allow_empty"), ZEND_ACC_PROTECTED );
}

