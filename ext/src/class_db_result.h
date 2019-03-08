
#ifndef PHP_LSENTITY_DB_RESULT_H
#define PHP_LSENTITY_DB_RESULT_H

#include "zend_API.h"

ZEND_BEGIN_ARG_INFO_EX(lsentity_db_result_get_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, name)
                ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()


extern ZEND_API zend_class_entry *lsentity_db_result_ce_ptr;

void lsentity_db_result_class_init();

#endif