

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_filter_rule_callback.h"
#include "class_filter_rule.h"

ZEND_API zend_class_entry *lsentity_filter_rule_callback_ce_ptr;


ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_rule_callback_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, fun)
    ZEND_ARG_ARRAY_INFO(0,param,0)
ZEND_END_ARG_INFO()

ZEND_METHOD(lsentity_filter_rule_callback_class, __construct){
    zval *table_object,*object;
    zend_class_entry *table_class;
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() , getThis(), "s|a",table_object,table_class) == FAILURE) {
        RETURN_FALSE;
    }
}
ZEND_METHOD(lsentity_filter_rule_callback_class, filter){}

static zend_function_entry lsentity_filter_rule_callback_class_method[] = {
    ZEND_ME(lsentity_filter_rule_callback_class,__construct, lsentity_filter_rule_callback_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_rule_callback_class,filter,lsentity_filter_rule_filter_arginfo,ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_filter_rule_callback_class_init(){
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce,LSENTITY_FILTERRULE_NS,"Callback",lsentity_filter_rule_callback_class_method);
    lsentity_filter_rule_callback_ce_ptr = zend_register_internal_class(&ce);
    zend_class_implements(lsentity_filter_rule_callback_ce_ptr,1,lsentity_filter_rule_ce_ptr);
    zend_declare_property_null(lsentity_filter_rule_callback_ce_ptr,ZEND_STRL("_fun"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_filter_rule_callback_ce_ptr,ZEND_STRL("_param"), ZEND_ACC_PROTECTED );
}

