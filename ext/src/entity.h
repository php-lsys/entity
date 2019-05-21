#ifndef PHP_LSENTITY_H
#define PHP_LSENTITY_H

#include "php.h"


#define LSENTITY_NS "LSYS\\Entity"

#define LSENTITY_DB_NS LSENTITY_NS "\\Database"

#define ZEND_ARG_OBJ_INFO_ENTITYNS(pass_by_ref, name, classname, allow_null) \
         { #name, ZEND_TYPE_ENCODE_CLASS_CONST(LSENTITY_NS"\\"#classname, allow_null), pass_by_ref, 0 },

#define ZEND_ARG_OBJ_INFO_ENTITYDBNS(pass_by_ref, name, classname, allow_null) \
        { #name, ZEND_TYPE_ENCODE_CLASS_CONST(LSENTITY_NS"\\Database\\"#classname, allow_null), pass_by_ref, 0 },

#endif

