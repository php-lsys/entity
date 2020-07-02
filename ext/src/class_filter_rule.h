
#ifndef PHP_LSENTITY_FILTER_RULE_H
#define PHP_LSENTITY_FILTER_RULE_H

#include "zend_API.h"

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_rule_filter_arginfo, 0, 0, 3)
    ZEND_ARG_OBJ_INFO_ENTITYCLASS(0, entity, 0)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

extern zend_class_entry *lsentity_filter_rule_ce_ptr;

void lsentity_filter_rule_class_init();

#endif