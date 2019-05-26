

#ifndef PHP_LSENTITY_COLUMN_SAVE_H
#define PHP_LSENTITY_COLUMN_SAVE_H
#include "zend_API.h"


ZEND_BEGIN_ARG_INFO_EX(lsentity_column_save_arginfo, 0, 0, 2)
                ZEND_ARG_OBJ_INFO_ENTITYCLASS(0, entity, 0)
                ZEND_ARG_INFO(0, column)
ZEND_END_ARG_INFO()

extern ZEND_API zend_class_entry *lsentity_column_save_ce_ptr;

void lsentity_column_save_class_init();



#endif