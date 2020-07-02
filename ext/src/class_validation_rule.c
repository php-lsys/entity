

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_validation_rule.h"


zend_class_entry *lsentity_validation_rule_ce_ptr;

static zend_function_entry lsentity_validation_rule_class_method[] = {
        ZEND_ABSTRACT_ME(lsentity_validation_rule_rule_class,allowEmpty,NULL)
        ZEND_ABSTRACT_ME(lsentity_validation_rule_rule_class,check,lsentity_validation_rule_check_arginfo)
        ZEND_FE_END
};
void lsentity_validation_rule_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"ValidRule",lsentity_validation_rule_class_method);
    lsentity_validation_rule_ce_ptr = zend_register_internal_interface(&ce );
}
