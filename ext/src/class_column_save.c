
#include "entity.h"
#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "class_column_save.h"

zend_class_entry *lsentity_column_save_ce_ptr;



static zend_function_entry lsentity_column_save_class_method[] = {
    ZEND_ABSTRACT_ME(lsentity_column_save_class,update, lsentity_column_save_arginfo)
    ZEND_ABSTRACT_ME(lsentity_column_save_class,create, lsentity_column_save_arginfo)
    ZEND_FE_END
};

void lsentity_column_save_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"ColumnSave",lsentity_column_save_class_method);
    lsentity_column_save_ce_ptr = zend_register_internal_interface(&ce);
}