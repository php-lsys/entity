
#ifndef PHP_LSENTITY_DB_BUILDER_H
#define PHP_LSENTITY_DB_BUILDER_H

#include "zend_API.h"


extern ZEND_API zend_class_entry *lsentity_db_sqlbuilder_ce_ptr;

void lsentity_db_sqlbuilder_class_init();

#endif