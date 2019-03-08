

#include "zend.h"
#include "Zend/zend_interfaces.h"
#include "php.h"
#include "zend_API.h"
#include "Zend/zend_exceptions.h"
#include "class_exception.h"
#include "utils.h"
#include "class_table.h"

int lsentity_new_class(zval *class_name,zval *return_value,zval *params,int num_args){
    zend_class_entry *ce;
    if ((ce = zend_lookup_class(Z_STR_P(class_name))) == NULL) {
        if (!EG(exception)) {
            zend_throw_exception_ex(lsentity_exception_ce_ptr, -1, "Class %s does not exist", Z_STRVAL_P(class_name));
        }
        return 0;
    }
    zval retval;
    zend_class_entry *old_scope;
    zend_function *constructor;
    if (UNEXPECTED(object_init_ex(return_value, ce) != SUCCESS)) {
        zend_throw_exception_ex(lsentity_exception_ce_ptr, -1, "Class %s does not init", Z_STRVAL_P(class_name));
        return 0;
    }
    old_scope = EG(fake_scope);
    EG(fake_scope) = ce;
    constructor = Z_OBJ_HT_P(return_value)->get_constructor(Z_OBJ_P(return_value));
    EG(fake_scope) = old_scope;

    /* Run the constructor if there is one */
    if (constructor) {
        int ret, i;
        zend_fcall_info fci;
        zend_fcall_info_cache fcc;

        if (!(constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
            zend_throw_exception_ex(ce, 0, "Access to non-public constructor of class %s", ZSTR_VAL(ce->name));
            zval_dtor(return_value);
            return 0;
        }

        for (i = 0; i < num_args; i++) {
            if (Z_REFCOUNTED(params[i])) Z_ADDREF(params[i]);
        }

        fci.size = sizeof(fci);
        ZVAL_UNDEF(&fci.function_name);
        fci.object = Z_OBJ_P(return_value);
        fci.retval = &retval;
        fci.param_count = num_args;
        fci.params = params;
        fci.no_separation = 1;

        fcc.initialized = 1;
        fcc.function_handler = constructor;
        fcc.calling_scope = zend_get_executed_scope();
        fcc.called_scope = Z_OBJCE_P(return_value);
        fcc.object = Z_OBJ_P(return_value);

        ret = zend_call_function(&fci, &fcc);
        zval_ptr_dtor(&retval);
        for (i = 0; i < num_args; i++) {
            zval_ptr_dtor(&params[i]);
        }
        if (ret == FAILURE) {
            php_error_docref(NULL, E_WARNING, "Invocation of %s's constructor failed", ZSTR_VAL(ce->name));
            zval_dtor(return_value);
            return 0;
        }
    }
    return 1;
}
