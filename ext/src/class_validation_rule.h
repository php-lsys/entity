
#ifndef PHP_LSENTITY_VALIDATION_RULE_H
#define PHP_LSENTITY_VALIDATION_RULE_H

#include "zend_API.h"



ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rule_check_arginfo, 0, 0, 6)
                ZEND_ARG_OBJ_INFO_ENTITYNS(0, validation, Validation, 0)
                ZEND_ARG_INFO(0, value)
                ZEND_ARG_INFO(0, field)
                ZEND_ARG_INFO(0, label)
                ZEND_ARG_OBJ_INFO_ENTITYNS(0, entity, Entity, 0)
                ZEND_ARG_ARRAY_INFO(0, check_data,0)
ZEND_END_ARG_INFO()

extern ZEND_API zend_class_entry *lsentity_validation_rule_ce_ptr;

void lsentity_validation_rule_class_init();

#endif