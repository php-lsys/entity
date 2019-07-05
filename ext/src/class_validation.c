

#include "zend.h"
#include "zend_interfaces.h"
#include "php.h"
#include "zend_API.h"
#include "entity.h"
#include "class_validation.h"
#include "class_validation_rule.h"
#include "utils.h"
#include "class_entity.h"
#include "class_column_set.h"
#include "ext/standard/php_array.h"


ZEND_API zend_class_entry *lsentity_validation_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_construct_arginfo, 0, 0, 1)
                ZEND_ARG_OBJ_INFO_ENTITYCLASS(0, entity, 1)
                ZEND_ARG_ARRAY_INFO(0, rule_gruop,0)
                ZEND_ARG_INFO(0, allow_cache)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_label_arginfo, 0, 0, 2)
            ZEND_ARG_INFO(0, field)
            ZEND_ARG_INFO(0, label)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_labels_arginfo, 0, 0, 1)
        ZEND_ARG_ARRAY_INFO(0, labels,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rule_arginfo, 0, 0, 1)
                ZEND_ARG_OBJ_INFO_ENTITYNS(0, rule, ValidRule, 1)
                ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_rules_arginfo, 0, 0, 1)
                ZEND_ARG_ARRAY_INFO(0, rules,0)
                ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_valid_arginfo, 0, 0, 1)
                ZEND_ARG_ARRAY_INFO(0, check_data,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_validation_error_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, field)
                ZEND_ARG_INFO(0, error)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_validation_class, __construct){


    zval *entity,*rule_group=NULL,*object;
    zend_bool allow_cache;
    ZEND_PARSE_PARAMETERS_START(1, 3)
            Z_PARAM_OBJECT_OF_CLASS(entity,lsentity_entity_ce_ptr)
            Z_PARAM_OPTIONAL
            Z_PARAM_ARRAY(rule_group)
            Z_PARAM_BOOL(allow_cache)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();

    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_entity"),entity);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_allow_cache"),allow_cache);

    zval trr;
    array_init(&trr);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_global_rules"),&trr);
    zval_ptr_dtor(&trr);
    array_init(&trr);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_rules"),&trr);
    zval_ptr_dtor(&trr);
    array_init(&trr);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_labels"),&trr);
    zval_ptr_dtor(&trr);
    array_init(&trr);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_errors"),&trr);
    zval_ptr_dtor(&trr);



    zval ret;
    zval param;
    ZVAL_FALSE(&param);
    zend_call_method_with_1_params(entity,Z_OBJCE_P(entity),NULL,"columns",&ret,&param);
    zval_ptr_dtor(&param);
    if(!lsentity_obj_check(lsentity_column_set_ce_ptr,&ret,1,1)){
        zval_ptr_dtor(&ret);
        RETURN_NULL();
    }
    zval columns;
    zval param1;
    ZVAL_LONG(&param1,LSENTITY_COLUMN_SET_TYPE_FIELD);
    zend_call_method_with_1_params(&ret,Z_OBJCE(ret),NULL,"asarray",&columns,&param1);
    zval_ptr_dtor(&param1);
    zval_ptr_dtor(&ret);


    //array_combine
    int arr_num;
    zval combine_val;
    arr_num = zend_hash_num_elements(Z_ARR(columns));
    if(arr_num<=0){
        array_init(&combine_val);
    }else{

        HashTable *values=Z_ARR(columns), *keys=Z_ARR(columns);
        uint32_t pos_values = 0;
        zval *entry_keys, *entry_values;

        array_init_size(&combine_val, arr_num);

        ZEND_HASH_FOREACH_VAL(keys, entry_keys) {
            while (1) {
                if (pos_values >= values->nNumUsed) {
                    break;
                } else if (Z_TYPE(values->arData[pos_values].val) != IS_UNDEF) {
                    entry_values = &values->arData[pos_values].val;
                    if (Z_TYPE_P(entry_keys) == IS_LONG) {
                        entry_values = zend_hash_index_update(Z_ARRVAL(combine_val),
                                                              Z_LVAL_P(entry_keys), entry_values);
                    } else {
                        zend_string *key = zval_get_string(entry_keys);
                        entry_values = zend_symtable_update(Z_ARRVAL(combine_val),
                                                            key, entry_values);
                        zend_string_release(key);
                    }
                    zval_add_ref(entry_values);
                    pos_values++;
                    break;
                }
                pos_values++;
            }
        } ZEND_HASH_FOREACH_END();
    }
    zval_ptr_dtor(&columns);
    zval lables;
    zend_call_method_with_0_params(entity,Z_OBJCE_P(entity),NULL,"labels",&lables);
    if(Z_TYPE(lables)==IS_ARRAY){
        php_array_merge(Z_ARR(combine_val),Z_ARR(lables));
    }
    zval_ptr_dtor(&lables);

    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_labels"),&combine_val);
    zval_ptr_dtor(&combine_val);

    if(rule_group&&Z_TYPE_P(rule_group)==IS_ARRAY){
        zval *entry;
        zend_ulong num_idx;
        zend_string *str_idx;
        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(rule_group), num_idx, str_idx, entry) {
            if(Z_TYPE_P(entity)==IS_ARRAY){
                zval ret;
                ZVAL_STR(&ret,str_idx);
                zend_call_method_with_2_params(object,Z_OBJCE_P(object),NULL,"rules",NULL,entry,&ret);
                zval_ptr_dtor(&ret);
            }
        } ZEND_HASH_FOREACH_END();
    }
}
ZEND_METHOD(lsentity_validation_class, allowCache){
    RETURN_ZVAL(zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_allow_cache"),0,NULL),0,0);
}
ZEND_METHOD(lsentity_validation_class, label){
    zval *object;
    zend_string *field,*label;
    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_STR(field)
            Z_PARAM_STR(label)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    zval *labels=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),0,NULL);
    if(Z_TYPE_P(labels)!=IS_ARRAY){
        zval t;
        array_init(&t);
        zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),&t);
        zval_ptr_dtor(&t);
    }
    labels=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),0,NULL);
    zval *old=zend_hash_find(Z_ARR_P(labels),field);
    zval labelval;
    ZVAL_STR(&labelval,label);
    Z_REFCOUNTED(labelval)&&Z_ADDREF(labelval);
    if(!old)zend_hash_add(Z_ARR_P(labels),field,&labelval);
    else zend_hash_update(Z_ARR_P(labels),field,&labelval);
    zval_ptr_dtor(&labelval);
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_validation_class, labels){
    zval *labels,*object;
    zend_string *field,*label;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ARRAY(labels)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *olabels=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),0,NULL);
    zval sarr;
    array_init(&sarr);
    if(olabels&&Z_TYPE_P(olabels)==IS_ARRAY){
        php_array_merge(Z_ARR(sarr),Z_ARR_P(olabels));
    }
    if(labels&&Z_TYPE_P(labels)==IS_ARRAY){
        php_array_merge(Z_ARR(sarr),Z_ARR_P(labels));
    }
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_labels"),&sarr);
    zval_ptr_dtor(&sarr);
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_validation_class, rule){

    zval *rule,*object;
    zend_string *field;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_OBJECT_OF_CLASS(rule,lsentity_validation_rule_ce_ptr)
            Z_PARAM_OPTIONAL
            Z_PARAM_STR(field)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    if(!field){
        zval *gr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_global_rules"),0,NULL);
        add_next_index_zval(gr,rule);
    }else{
        zval *labels=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),0,NULL);
        if(Z_TYPE_P(labels)!=IS_ARRAY){
            zval t;
            array_init(&t);
            zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),&t);
            zval_ptr_dtor(&t);
        }
        labels=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),0,NULL);
        zval *old=zend_hash_find(Z_ARR_P(labels),field);
        if(!old){
            zval zfield;
            ZVAL_STR(&zfield,field);
            Z_REFCOUNTED(zfield)&&Z_ADDREF(zfield);
            zend_hash_add(Z_ARR_P(labels),field,&zfield);
            zval_ptr_dtor(&zfield);
        }

        zval *gr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_rules"),0,NULL);
        if(Z_TYPE_P(gr)==IS_ARRAY){
            zval *sgr=zend_hash_find(Z_ARR_P(gr),field);
            if(!sgr){
                zval ta;
                array_init(&ta);
                Z_REFCOUNTED(ta)&&Z_ADDREF(ta);
                zend_hash_add(Z_ARR_P(gr),field,&ta);
                zval_ptr_dtor(&ta);
            }
            sgr=zend_hash_find(Z_ARR_P(gr),field);
            Z_REFCOUNTED_P(rule)&&Z_ADDREF_P(rule);
            add_next_index_zval(sgr,rule);
        }
    }
    RETURN_ZVAL(object,1,0);

}
ZEND_METHOD(lsentity_validation_class, rules){

    zval *filter_rules,*object;
    zend_string *field;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_ARRAY(filter_rules)
            Z_PARAM_OPTIONAL
            Z_PARAM_STR(field)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();

    zval *entry;
    ZEND_HASH_FOREACH_VAL(Z_ARR_P(filter_rules),entry) {
                if(lsentity_obj_check(lsentity_validation_rule_ce_ptr,entry,1,0)){
                    RETURN_NULL();
                }
                zval ret;
                ZVAL_STR(&ret,field);
                zend_call_method_with_2_params(object,Z_OBJCE_P(object),NULL,"rule",NULL,&ret,entry);
                zval_ptr_dtor(&ret);
            } ZEND_HASH_FOREACH_END();

    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_validation_class, valid){
    zval t;
    array_init(&t);
    zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_errors"),&t);
    zval_ptr_dtor(&t);
    zval *check_data,*object;
    zend_string *field;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ARRAY(check_data)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();

    zend_array *arrval;
    zend_ulong elem_count;
    arrval = Z_ARRVAL_P(check_data);
    elem_count = zend_hash_num_elements(arrval);

    zval keys;

    if (elem_count>0) {
        zval *entry,
                new_val;				/* New value */
        zend_ulong num_idx;
        zend_string *str_idx;

        array_init_size(&keys, elem_count);
        zend_hash_real_init(Z_ARRVAL(keys), 1);
        ZEND_HASH_FILL_PACKED(Z_ARRVAL(keys)) {
            if (HT_IS_PACKED(arrval) && HT_IS_WITHOUT_HOLES(arrval)) {
                /* Optimistic case: range(0..n-1) for vector-like packed array */
                ZVAL_LONG(&new_val, 0);
                for (; Z_LVAL(new_val) < elem_count; ++Z_LVAL(new_val)) {
                    ZEND_HASH_FILL_ADD(&new_val);
                }
            } else {
                ZEND_HASH_FOREACH_KEY_VAL_IND(arrval, num_idx, str_idx, entry) {
                            if (str_idx) {
                                ZVAL_STR_COPY(&new_val, str_idx);
                            } else {
                                ZVAL_LONG(&new_val, num_idx);
                            }
                            ZEND_HASH_FILL_ADD(&new_val);
                        } ZEND_HASH_FOREACH_END();
            }
        } ZEND_HASH_FILL_END();
    }else{
        array_init(&keys);
    }
    zval *_rules=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_rules"),0,NULL);
    if(Z_TYPE_P(_rules)!=IS_ARRAY){
        zval_ptr_dtor(&keys);
        RETURN_FALSE;
    }
    zval rules;
    ZVAL_COPY_VALUE(&rules,_rules);
    zval *gr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_global_rules"),0,NULL);
    if(Z_TYPE_P(gr)==IS_ARRAY){
        zval *field;
        ZEND_HASH_FOREACH_VAL(Z_ARR(keys),field){
            zval *h=zend_hash_find(Z_ARR(rules),Z_STR_P(field));
            zval trule;
            array_init(&trule);
            if(h&&Z_TYPE_P(h)==IS_ARRAY){
                php_array_merge(Z_ARR(trule),Z_ARR_P(h));
            }
            php_array_merge(Z_ARR(trule),Z_ARR_P(gr));
            Z_REFCOUNTED(trule)&&Z_ADDREF(trule);
            zend_hash_update(Z_ARR(rules),Z_STR_P(field),&trule);
            zval_ptr_dtor(&trule);
        }ZEND_HASH_FOREACH_END();
    }

    zval *setkey,*rval;
    zend_string * fieldkey;
    zval emptyarr;
    array_init(&emptyarr);
    add_next_index_zval(&emptyarr,&emptyarr);
    add_next_index_null(&emptyarr);
    add_next_index_bool(&emptyarr,0);
    add_next_index_string(&emptyarr,"");

    zval *labels=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_labels"),0,NULL);
    zval *entity=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_entity"),0,NULL);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR(rules),fieldkey,setkey) {

        if(Z_TYPE_P(setkey)!=IS_ARRAY)continue;
        ZEND_HASH_FOREACH_VAL(Z_ARR_P(setkey),rval){

//            $rule->check($this,$field,$value,$this->_labels[$field],$this->,$check_data);
//            if (isset($this->_errors[$field])) {
//                break;
//            }
            zval val;
            zval *tval=zend_hash_find(Z_ARR_P(check_data),fieldkey);
            if(tval)ZVAL_COPY_VALUE(&val,tval);
            else ZVAL_NULL(&val);
            if(lsentity_check_bool_with_0_params(rval,"allowempty")&&lsentity_in_array(&emptyarr,&val)){
              continue;
            }

            zval rlable;
            ZVAL_NULL(&rlable);
            if(Z_TYPE_P(labels)==IS_ARRAY){
                zval *_label=zend_hash_find(Z_ARR_P(labels),fieldkey);
                if(_label)ZVAL_COPY_VALUE(&rlable,_label);
            }

            zend_fcall_info fci;
            zval retval;
            zval params[6];

            zval tfield;
            ZVAL_STR(&tfield,fieldkey);

            ZVAL_COPY_VALUE(&params[0], getThis());
            ZVAL_COPY_VALUE(&params[1], &tfield);
            ZVAL_COPY_VALUE(&params[2], &val);
            ZVAL_COPY_VALUE(&params[3], &rlable);
            ZVAL_COPY_VALUE(&params[4], entity);
            ZVAL_COPY_VALUE(&params[5], check_data);
            zval_ptr_dtor(&tfield);

            fci.size = sizeof(fci);
            fci.object = Z_OBJ_P(rval);
            fci.retval = &retval;
            fci.param_count = 6;
            fci.params = params;
            fci.no_separation = 1;

            zend_fcall_info_cache fcic;
            ZVAL_UNDEF(&fci.function_name); /* Unused */

            zend_class_entry * obj_ce = Z_OBJCE_P(rval);

            HashTable *function_table = &obj_ce->function_table;
            fcic.function_handler = zend_hash_str_find_ptr(
                    function_table, ZEND_STRL("check"));
            if (fcic.function_handler == NULL) {
                /* error at c-level */
                zend_error_noreturn(E_CORE_ERROR, "Couldn't find implementation for method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", "check");
                RETURN_NULL();
            }
            fcic.calling_scope = obj_ce;
            fcic.object = Z_OBJ_P(rval);
            int _result = zend_call_function(&fci, &fcic);
            if (_result == FAILURE) {
                if (!EG(exception)) {
                    zend_error_noreturn(E_CORE_ERROR, "Couldn't execute method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", "check");
                }
            }
            zval_ptr_dtor(&retval);

            zval *err=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_errors"),0,NULL);
            if(Z_TYPE_P(err)==IS_ARRAY&&zend_hash_find(Z_ARR_P(err),fieldkey)){
                break;
            }

        }ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FOREACH_END();
    zval_ptr_dtor(&emptyarr);
    zval_ptr_dtor(&keys);
    zval *err=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_errors"),0,NULL);
    RETURN_BOOL(zend_hash_num_elements(Z_ARR_P(err))==0);

}
ZEND_METHOD(lsentity_validation_class, error){
    zval *object;
    zend_string *field,*error;
    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_STR(field)
            Z_PARAM_STR(error)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    zval *errors=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_errors"),0,NULL);
    if(Z_TYPE_P(errors)!=IS_ARRAY){
        zval t;
        array_init(&t);
        zend_update_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_errors"),&t);
        zval_ptr_dtor(&t);
    }
    errors=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_errors"),0,NULL);
    zval *old=zend_hash_find(Z_ARR_P(errors),field);

    zval zerror;
    ZVAL_STR(&zerror,error);
    Z_REFCOUNTED(zerror)&&Z_ADDREF(zerror);
    if(!old)zend_hash_add(Z_ARR_P(errors),field,&zerror);
    else zend_hash_update(Z_ARR_P(errors),field,&zerror);
    zval_ptr_dtor(&zerror);
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_validation_class, errors){
    zval *labels=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_errors"),0,NULL);
    if(Z_TYPE_P(labels)!=IS_ARRAY){
        zval t;
        array_init(&t);
        RETURN_ZVAL(&t,1,1);
    }
    RETURN_ZVAL(labels,1,0);
}


static zend_function_entry lsentity_validation_class_method[] = {
    ZEND_ME(lsentity_validation_class,__construct, lsentity_validation_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,allowCache, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,label, lsentity_validation_label_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,labels, lsentity_validation_labels_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,rule, lsentity_validation_rule_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,rules, lsentity_validation_rules_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,valid, lsentity_validation_valid_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,error, lsentity_validation_error_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_validation_class,errors, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_validation_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Validation",lsentity_validation_class_method);
    lsentity_validation_ce_ptr = zend_register_internal_class(&ce );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_entity"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_global_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_labels"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_validation_ce_ptr,ZEND_STRL("_errors"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_validation_ce_ptr,ZEND_STRL("_allow_cache"),1, ZEND_ACC_PROTECTED );
}

