

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_filter.h"

ZEND_API zend_class_entry *lsentity_filter_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, entity, Entity, 0)
    ZEND_ARG_ARRAY_INFO(0, rule_gruop,0)
    ZEND_ARG_INFO(0, allow_cache)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_rule_arginfo, 0, 0, 1)
        ZEND_ARG_OBJ_INFO_ENTITYNS(0, rule, FilterRule, 1)
        ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_rules_arginfo, 0, 0, 1)
                ZEND_ARG_ARRAY_INFO(0, rules,0)
                ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_run_arginfo, 0, 0, 2)
                ZEND_ARG_INFO(0, field)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_filter_class, __construct){
    zval *table_object,*object;
    zend_class_entry *table_class;
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() , getThis(), "|O",table_object,table_class) == FAILURE) {
        RETURN_FALSE;
    }
}
ZEND_METHOD(lsentity_filter_class, rule){}
ZEND_METHOD(lsentity_filter_class, rules){}
ZEND_METHOD(lsentity_filter_class, runFilter){}
ZEND_METHOD(lsentity_filter_class, allowCache){}

static zend_function_entry lsentity_filter_class_method[] = {
    ZEND_ME(lsentity_filter_class,__construct, lsentity_filter_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,rule, lsentity_filter_rule_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,rules, lsentity_filter_rules_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,runFilter, lsentity_filter_run_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,allowCache, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_filter_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Filter",lsentity_filter_class_method);
    lsentity_filter_ce_ptr = zend_register_internal_class(&ce );
    zend_declare_property_null(lsentity_filter_ce_ptr,ZEND_STRL("_entity"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_filter_ce_ptr,ZEND_STRL("_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_filter_ce_ptr,ZEND_STRL("_global_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_filter_ce_ptr,ZEND_STRL("_allow_cache"),0,ZEND_ACC_PROTECTED );
}

