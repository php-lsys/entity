


#include "zend.h"
#include "php.h"
#include "zend_interfaces.h"
#include "zend_API.h"
#include "entity.h"
#include "utils.h"
#include "class_entity.h"
#include "class_filter.h"
#include "class_filter_rule.h"
#include "ext/standard/php_array.h"

zend_class_entry *lsentity_filter_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO_ENTITYCLASS(0, entity, 0)
    ZEND_ARG_ARRAY_INFO(0, rule_gruop,0)
    ZEND_ARG_INFO(0, allow_cache)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_rule_arginfo, 0, 0, 1)
        ZEND_ARG_OBJ_INFO_ENTITYNS(0, rule, FilterRule, 1)
        ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_rules_arginfo, 0, 0, 1)
                ZEND_ARG_ARRAY_INFO(0, rules,0)
                ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_filter_run_arginfo, 0, 0, 2)
                ZEND_ARG_INFO(0, field)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_filter_class, __construct){

    zval *entity,*object,*rule_group=NULL;
    zend_bool  allow_cache=1;
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

    if(rule_group){
        zval *entry;
        zend_ulong num_idx;
        zend_string *str_idx;
        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(rule_group), num_idx, str_idx, entry) {
            if(Z_TYPE_P(entry)==IS_ARRAY&&str_idx){
                zval ret;
                ZVAL_STR_COPY(&ret,str_idx);
                zend_call_method_with_2_params(object,Z_OBJCE_P(object),NULL,"rules",NULL,entry,&ret);
                zval_ptr_dtor(&ret);
            }
        } ZEND_HASH_FOREACH_END();
    }



}
ZEND_METHOD(lsentity_filter_class, rule){

    zval *filter_rule,*object;
    zend_string *field=NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_OBJECT_OF_CLASS(filter_rule,lsentity_filter_rule_ce_ptr)
            Z_PARAM_OPTIONAL
            Z_PARAM_STR(field)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    if(!field){
        zval *gr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_global_rules"),0,NULL);
        Z_REFCOUNTED_P(filter_rule)&&Z_ADDREF_P(filter_rule);
        add_next_index_zval(gr,filter_rule);
    }else{
        zval *gr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_rules"),0,NULL);
        zval *sgr=zend_hash_find(Z_ARR_P(gr),field);
        if(!sgr){
            zval ta;
            array_init(&ta);
            Z_REFCOUNTED(ta)&&Z_ADDREF(ta);
            zend_hash_add(Z_ARR_P(gr),field,&ta);
            zval_ptr_dtor(&ta);
        }
        sgr=zend_hash_find(Z_ARR_P(gr),field);
        Z_REFCOUNTED_P(filter_rule)&&Z_ADDREF_P(filter_rule);
        add_next_index_zval(sgr,filter_rule);
    }
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_filter_class, rules){

    zval *filter_rules,*object;
    zend_string *field=NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_ARRAY(filter_rules)
            Z_PARAM_OPTIONAL
            Z_PARAM_STR(field)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();

    zval *entry;
    ZEND_HASH_FOREACH_VAL(Z_ARR_P(filter_rules),entry) {
        if(!lsentity_obj_check(lsentity_filter_rule_ce_ptr,entry,1,0)){

            RETURN_NULL();
        }

        if(field) {
            zval ret;
            ZVAL_STR_COPY(&ret, field);
            zend_call_method_with_2_params(object,Z_OBJCE_P(object),NULL,"rule",NULL,entry,&ret);
            zval_ptr_dtor(&ret);
        }else{
            zend_call_method_with_1_params(object,Z_OBJCE_P(object),NULL,"rule",NULL,entry);
        }


    } ZEND_HASH_FOREACH_END();

    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_filter_class, runFilter){

    zval *filter_value,*object;
    zend_string *field;
    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_STR(field)
            Z_PARAM_ZVAL(filter_value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval valuecopy;
    ZVAL_DUP(&valuecopy, filter_value);
    zval *value=&valuecopy;

    object = getThis();

    zval *gr=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_rules"),0,NULL);

    zval *grs=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_global_rules"),0,NULL);


    zval *rule=zend_hash_find(Z_ARR_P(gr),field);

    zval rules;
    array_init(&rules);
    if(rule&&Z_TYPE_P(rule)==IS_ARRAY){
        zval tmp;
        ZVAL_DUP(&tmp,rule);
        php_array_merge(Z_ARR(rules),Z_ARR(tmp));
        zval_ptr_dtor(&tmp);
    }

    if(Z_TYPE_P(grs)==IS_ARRAY){
        zval tmp;
        ZVAL_DUP(&tmp,grs);
        php_array_merge(Z_ARR(rules),Z_ARR(tmp));
        zval_ptr_dtor(&tmp);
    }
    zval *entity = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_entity"),0,NULL);
    zval *entry;


    ZEND_HASH_FOREACH_VAL(Z_ARR(rules),entry) {

                zend_fcall_info fci;
                zval retval;
                zval params[3];

                zval tfield;
                ZVAL_STR_COPY(&tfield,field);

                ZVAL_DUP(&params[0], entity);
                ZVAL_DUP(&params[1], value);
                ZVAL_DUP(&params[2], &tfield);
                zval_ptr_dtor(&tfield);

                fci.size = sizeof(fci);
                fci.object = Z_OBJ_P(entry);
                fci.retval = &retval;
                fci.param_count = 3;
                fci.params = params;
                fci.no_separation = 1;

                zend_fcall_info_cache fcic;
                ZVAL_UNDEF(&fci.function_name); /* Unused */
                #if PHP_VERSION_ID<70300 //@todo　//@todo php7.2版本不一样
                fcic.initialized = 1;
                #endif
                zend_class_entry * obj_ce = Z_OBJCE_P(entry);

                HashTable *function_table = &obj_ce->function_table;
                fcic.function_handler = zend_hash_str_find_ptr(
                        function_table, ZEND_STRL("filter"));
                if (fcic.function_handler == NULL) {
                    /* error at c-level */
                    zval_ptr_dtor(&rules);
                    zval_ptr_dtor(value);
                    zend_error_noreturn(E_CORE_ERROR, "Couldn't find implementation for method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", "filter");
                    RETURN_NULL();
                }
                fcic.calling_scope = obj_ce;
                fcic.object = Z_OBJ_P(entry);
                int _result = zend_call_function(&fci, &fcic);

                if (_result == FAILURE) {
                    zval_ptr_dtor(value);
                    if (!EG(exception)) {
                        zend_error_noreturn(E_CORE_ERROR, "Couldn't execute method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", "filter");
                        break;
                    }
                }

                zval_ptr_dtor(value);
                ZVAL_DUP(value, &retval);
                zval_ptr_dtor(&retval);
    } ZEND_HASH_FOREACH_END();
    zval_ptr_dtor(&rules);
    RETURN_ZVAL(value,1,1);
}
ZEND_METHOD(lsentity_filter_class, allowCache){
    zval *val=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_allow_cache"),0,NULL);
    zval tmp;
    ZVAL_DUP(&tmp,val);
    RETURN_ZVAL(&tmp,1,1);
}

static zend_function_entry lsentity_filter_class_method[] = {
    ZEND_ME(lsentity_filter_class,__construct, lsentity_filter_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,rule, lsentity_filter_rule_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,rules, lsentity_filter_rules_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,runFilter, lsentity_filter_run_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_filter_class,allowCache, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


void lsentity_filter_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Filter",lsentity_filter_class_method);
    lsentity_filter_ce_ptr = zend_register_internal_class(&ce );
    zend_declare_property_null(lsentity_filter_ce_ptr,ZEND_STRL("_entity"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_filter_ce_ptr,ZEND_STRL("_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_filter_ce_ptr,ZEND_STRL("_global_rules"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_filter_ce_ptr,ZEND_STRL("_allow_cache"),1,ZEND_ACC_PROTECTED );
}

