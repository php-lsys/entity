
#include "entity.h"
#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "class_i18n.h"

ZEND_API zend_class_entry *lsentity_i18n_ce_ptr;



static zend_function_entry lsentity_i18n_class_method[] = {
    ZEND_ABSTRACT_ME(lsentity_table_class,__, lsentity_i18n____arginfo)
    ZEND_FE_END
};

void lsentity_i18n_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"I18n",lsentity_i18n_class_method);
    lsentity_i18n_ce_ptr = zend_register_internal_interface(&ce );
}