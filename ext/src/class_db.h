
#ifndef PHP_LSENTITY_DB_H
#define PHP_LSENTITY_DB_H
#include "zend_API.h"


ZEND_BEGIN_ARG_INFO_EX(lsentity_db_quote_table_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, table)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_db_quote_column_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, column)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_db_quote_value_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, column_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_db_exec_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, sql)
    ZEND_ARG_ARRAY_INFO(0, data,0)
ZEND_END_ARG_INFO()


extern ZEND_API zend_class_entry *lsentity_db_ce_ptr;

void lsentity_db_class_init();

#endif