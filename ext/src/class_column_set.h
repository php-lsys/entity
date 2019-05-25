//
// Created by it015 on 2019-03-01.
//

#ifndef PHP_LSENTITY_COLUMN_SET_H
#define PHP_LSENTITY_COLUMN_SET_H

#include "zend_API.h"

extern ZEND_API zend_class_entry *lsentity_column_set_ce_ptr;

void lsentity_column_set_class_init();

/**
 * 只返回字段
 * @var integer
 */
#define LSENTITY_COLUMN_SET_TYPE_FIELD 1
/**
 * 返回字段和默认值
 * @var integer
 */
#define LSENTITY_COLUMN_SET_TYPE_ARRAY 2
/**
 * 返回字段和默认值
 * @var integer
 */
#define LSENTITY_COLUMN_SET_TYPE_DEFAULT 3

#endif
