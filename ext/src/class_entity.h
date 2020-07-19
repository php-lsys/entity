
#ifndef PHP_LSENTITY_ENTITY_H
#define PHP_LSENTITY_ENTITY_H

#include "zend_API.h"

extern zend_class_entry *lsentity_entity_ce_ptr;

void lsentity_entity_class_init();
ZEND_METHOD(lsentity_entity_class, __construct);
ZEND_METHOD(lsentity_entity_class, labels);
ZEND_METHOD(lsentity_entity_class, filter);
ZEND_METHOD(lsentity_entity_class, validation);
ZEND_METHOD(lsentity_entity_class, table);
ZEND_METHOD(lsentity_entity_class, loadData);
ZEND_METHOD(lsentity_entity_class, exportData);
ZEND_METHOD(lsentity_entity_class, changed);
ZEND_METHOD(lsentity_entity_class, clear);
ZEND_METHOD(lsentity_entity_class, __isset);
ZEND_METHOD(lsentity_entity_class, __set);
ZEND_METHOD(lsentity_entity_class, __get);
ZEND_METHOD(lsentity_entity_class, setNotExist);
ZEND_METHOD(lsentity_entity_class, getNotExist);
ZEND_METHOD(lsentity_entity_class, columns);
ZEND_METHOD(lsentity_entity_class, pk);
ZEND_METHOD(lsentity_entity_class, saved);
ZEND_METHOD(lsentity_entity_class, __toString);
ZEND_METHOD(lsentity_entity_class, loaded);
ZEND_METHOD(lsentity_entity_class, update);
ZEND_METHOD(lsentity_entity_class, create);
ZEND_METHOD(lsentity_entity_class, save);
ZEND_METHOD(lsentity_entity_class, delete);
ZEND_METHOD(lsentity_entity_class, check);
ZEND_METHOD(lsentity_entity_class, values);
ZEND_METHOD(lsentity_entity_class, asArray);
ZEND_METHOD(lsentity_entity_class, jsonSerialize);
ZEND_METHOD(lsentity_entity_class, filterFactory);
ZEND_METHOD(lsentity_entity_class, validationFactory);
ZEND_METHOD(lsentity_entity_class, tableClass);



#endif