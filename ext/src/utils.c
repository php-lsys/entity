

#include "zend.h"
#include "Zend/zend_interfaces.h"
#include "php.h"
#include "zend_API.h"
#include "Zend/zend_exceptions.h"
#include "class_exception.h"
#include "utils.h"
#include "class_table.h"

int lsentity_new_class(zend_class_entry *ce,zval *return_value,zval *params,int num_args){
    zval retval;
    zend_class_entry *old_scope;
    zend_function *constructor;
    if (UNEXPECTED(object_init_ex(return_value, ce) != SUCCESS)) {
        zend_throw_exception_ex(lsentity_exception_ce_ptr, -1, "Class %s does not init", ZSTR_VAL(ce->name));
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


int lsentity_check_bool_with_0_params(zval *object,const char* method){
    zval isexist;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, method, &isexist);
    int ret=zend_is_true(&isexist);
    zval_ptr_dtor(&isexist);
    return ret;
}

int lsentity_check_bool_with_1_params(zval *object,const char* method,zval *param){
    zval isexist;
    zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, method, &isexist,param);
    int ret=zend_is_true(&isexist);
    zval_ptr_dtor(&isexist);
    return ret;
}

int lsentity_check_bool_with_2_params(zval *object,const char* method,zval *param1,zval *param2){
    zval isexist;
    zend_call_method_with_2_params(object,Z_OBJCE_P(object), NULL, method, &isexist,param1,param2);
    int ret=zend_is_true(&isexist);
    zval_ptr_dtor(&isexist);
    return ret;
}

int lsentity_obj_check(zend_class_entry * ce,zval *retobj,int throw){
    if(zend_object_is_true(retobj)&&instanceof_function(Z_OBJCE_P(retobj),ce)) {
        return 1;
    }else{
        zval_ptr_dtor(retobj);
        if(throw)zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "the table method not return %s object",ZSTR_VAL(ce->name));
        return 0;
    }
}


int lsentity_in_array(zval *array,zval *value){
    zval *entry;
    zend_ulong num_idx;
    zend_string *str_idx;
    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
                ZVAL_DEREF(entry);
                if (fast_is_identical_function(value, entry)) {
                    return 1;
                }
            } ZEND_HASH_FOREACH_END();
    return 0;
}

