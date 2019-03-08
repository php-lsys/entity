

#ifndef PHP_LSENTITY_I18N_H
#define PHP_LSENTITY_I18N_H
#include "zend_API.h"

ZEND_BEGIN_ARG_INFO_EX(lsentity_i18n____arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, string)
                ZEND_ARG_ARRAY_INFO(0, values,1)
ZEND_END_ARG_INFO()



extern ZEND_API zend_class_entry *lsentity_i18n_ce_ptr;

void lsentity_i18n_class_init();



#endif