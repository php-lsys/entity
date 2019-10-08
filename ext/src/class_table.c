
#include "entity.h"
#include "zend.h"
#include "php.h"
#include "zend_API.h"

#include "class_table.h"

ZEND_API zend_class_entry *lsentity_table_ce_ptr;

static zend_function_entry lsentity_table_class_method[] = {
    ZEND_ABSTRACT_ME(lsentity_table_class,db, NULL)
    ZEND_ABSTRACT_ME(lsentity_table_class,tableColumns, NULL)
    ZEND_ABSTRACT_ME(lsentity_table_class,tableName, NULL)
    ZEND_ABSTRACT_ME(lsentity_table_class,primaryKey, NULL)
    ZEND_FE_END
};

void lsentity_table_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Table",lsentity_table_class_method);
    lsentity_table_ce_ptr = zend_register_internal_interface(&ce);
}