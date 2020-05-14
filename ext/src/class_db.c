

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_db.h"

ZEND_API zend_class_entry *lsentity_db_ce_ptr;




static zend_function_entry lsentity_db_class_method[] = {
    ZEND_ABSTRACT_ME(lsentity_db_class,quoteTable, lsentity_db_quote_table_arginfo)
    ZEND_ABSTRACT_ME(lsentity_db_class,quoteTable, lsentity_db_quote_table_arginfo)
    ZEND_ABSTRACT_ME(lsentity_db_class,quoteColumn, lsentity_db_quote_column_arginfo)
    ZEND_ABSTRACT_ME(lsentity_db_class,quoteValue, lsentity_db_quote_value_arginfo)
    ZEND_ABSTRACT_ME(lsentity_db_class,exec, lsentity_db_exec_arginfo)
    ZEND_ABSTRACT_ME(lsentity_db_class,insertId, NULL)
    ZEND_ABSTRACT_ME(lsentity_db_class,SQLBuilder, lsentity_db_sqlbuilder_value_arginfo)
    ZEND_FE_END
};

void lsentity_db_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Database",lsentity_db_class_method);
    lsentity_db_ce_ptr = zend_register_internal_interface(&ce);
}