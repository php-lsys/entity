

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_inheritance.h"

#include "entity.h"
#include "class_db.h"
#include "class_db_result.h"


ZEND_API zend_class_entry *lsentity_db_result_ce_ptr;


static zend_function_entry lsentity_db_result_class_method[] = {
    ZEND_ABSTRACT_ME(lsentity_db_result_class,get, lsentity_db_result_get_arginfo)
    ZEND_ABSTRACT_ME(lsentity_db_result_class,setFetchFree, NULL)
    ZEND_ABSTRACT_ME(lsentity_db_result_class,fetchCount, lsentity_db_result_fetch_count_arginfo)
    ZEND_FE_END
};

void lsentity_db_result_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_DB_NS,"Result",lsentity_db_result_class_method);
    lsentity_db_result_ce_ptr = zend_register_internal_interface(&ce);
    zend_do_inheritance(lsentity_db_result_ce_ptr, zend_ce_iterator);
}