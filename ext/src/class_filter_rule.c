

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_filter_rule.h"

ZEND_API zend_class_entry *lsentity_filter_rule_ce_ptr;

static zend_function_entry lsentity_filter_class_method[] = {
      ZEND_ABSTRACT_ME(lsentity_filter_rule_class,filter,lsentity_filter_rule_filter_arginfo)
      ZEND_FE_END
};
void lsentity_filter_rule_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"FilterRule",lsentity_filter_class_method);
    lsentity_filter_rule_ce_ptr = zend_register_internal_interface(&ce );
}

