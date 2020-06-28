#ifndef PHP_LSENTITY_H
#define PHP_LSENTITY_H

#include "php.h"

#if PHP_VERSION_ID<70200
#include "zend.h"
#include "zend_API.h"
#endif

#define LS_NS "LSYS"
#define LSENTITY_NS LS_NS "\\Entity"
#define LSENTITY_DB_NS LSENTITY_NS "\\Database"

#if PHP_VERSION_ID>=70200

#define ZEND_ARG_OBJ_INFO_ENTITYCLASS(pass_by_ref, name, allow_null) \
         { #name, ZEND_TYPE_ENCODE_CLASS_CONST(LSENTITY_NS, allow_null), pass_by_ref, 0 },

#define ZEND_ARG_OBJ_INFO_ENTITYNS(pass_by_ref, name, classname, allow_null) \
         { #name, ZEND_TYPE_ENCODE_CLASS_CONST(LSENTITY_NS"\\"#classname, allow_null), pass_by_ref, 0 },

#define ZEND_ARG_OBJ_INFO_ENTITYDBNS(pass_by_ref, name, classname, allow_null) \
        { #name, ZEND_TYPE_ENCODE_CLASS_CONST(LSENTITY_NS"\\Database\\"#classname, allow_null), pass_by_ref, 0 },

#else

    #define ZEND_ARG_OBJ_INFO_ENTITYCLASS(pass_by_ref, name, allow_null) \
                    { #name, LSENTITY_NS, IS_OBJECT, pass_by_ref, allow_null, 0 },

    #define ZEND_ARG_OBJ_INFO_ENTITYNS(pass_by_ref, name, classname, allow_null) \
                    { #name, LSENTITY_NS"\\"#classname, IS_OBJECT, pass_by_ref, allow_null, 0 },

    #define ZEND_ARG_OBJ_INFO_ENTITYDBNS(pass_by_ref, name, classname, allow_null) \
                    { #name, LSENTITY_NS"\\Database\\"#classname, IS_OBJECT, pass_by_ref, allow_null, 0 },

#endif

#endif