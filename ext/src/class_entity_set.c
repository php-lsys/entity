

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "entity.h"
#include "zend_inheritance.h"
#include "class_db_result.h"
#include "class_entity_set.h"
#include "class_entity_column_set.h"
#include "class_table.h"
#include "class_exception.h"
#include "utils.h"
#include "class_entity.h"

zend_class_entry *lsentity_entity_set_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_set_construct_arginfo, 0, 0, 2)
    ZEND_ARG_OBJ_INFO_ENTITYDBNS(0, result, Result, 0)
    ZEND_ARG_INFO(0, entity_name)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, columns, EntityColumnSet, 1)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, table, Table, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_set_asarr_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_set_fetch_count_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, iterator)
ZEND_END_ARG_INFO()


ZEND_METHOD(lsentity_entity_set_class, __construct){
    zval *db_result,*columns=NULL,*table=NULL,*object;
    zend_string *entity_name;
    ZEND_PARSE_PARAMETERS_START(2,4)
            Z_PARAM_OBJECT_OF_CLASS(db_result, lsentity_db_result_ce_ptr)
            Z_PARAM_STR(entity_name)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(columns, lsentity_entity_column_set_ce_ptr,1,0)
            Z_PARAM_OBJECT_OF_CLASS_EX(table, lsentity_table_ce_ptr,1,0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    zend_update_property(lsentity_entity_set_ce_ptr,object,ZEND_STRL("_result"),db_result);
    zend_update_property_str(lsentity_entity_set_ce_ptr,object,ZEND_STRL("_entity"),entity_name);
    if(table)zend_update_property(lsentity_entity_set_ce_ptr,object,ZEND_STRL("_table"),table);
    if(columns)zend_update_property(lsentity_entity_set_ce_ptr,object,ZEND_STRL("_columns"),columns);
}
ZEND_METHOD(lsentity_entity_set_class, setFetchFree){

    zval *object;
    object = getThis();

    zval *result=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_result"),0,NULL);

    zend_call_method_with_0_params(result,Z_OBJCE_P(result),NULL,"setfetchfree",NULL);
    RETURN_ZVAL(object,1,0);

}
ZEND_METHOD(lsentity_entity_set_class, fetchCount){
    zval *object;
    zend_bool iterator = 0;
    ZEND_PARSE_PARAMETERS_START(0, 1)
    		Z_PARAM_OPTIONAL
            Z_PARAM_BOOL(iterator)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    zval *result=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_result"),0,NULL);
    if(Z_TYPE_P(result)==IS_OBJECT&&zend_object_is_true(result)){
    	zval ret;
    	zval param;
    	ZVAL_BOOL(&param,iterator);
        zend_call_method_with_1_params(result,Z_OBJCE_P(result),NULL,"fetchcount",&ret,&param);
        zval_ptr_dtor(&param);
        RETURN_ZVAL(&ret,1,1);
    }
    RETURN_LONG(0);
}
ZEND_METHOD(lsentity_entity_set_class, key){
    zval *object;
    object = getThis();
    zval *result=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_result"),0,NULL);
    zval ret;
    zend_call_method_with_0_params(result,Z_OBJCE_P(result),NULL,"key",&ret);
    RETURN_ZVAL(&ret,1,1);

}
ZEND_METHOD(lsentity_entity_set_class, next){

    zval *object;
    object = getThis();
    zval *result=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_result"),0,NULL);
    zval ret;
    zend_call_method_with_0_params(result,Z_OBJCE_P(result),NULL,"next",&ret);
    RETURN_ZVAL(&ret,1,1);

}
ZEND_METHOD(lsentity_entity_set_class, rewind){

    zval *object;
    object = getThis();
    zval *result=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_result"),0,NULL);
    zval ret;
    zend_call_method_with_0_params(result,Z_OBJCE_P(result),NULL,"rewind",&ret);
    RETURN_ZVAL(&ret,1,1);

}
ZEND_METHOD(lsentity_entity_set_class, asArray){
    zval *object;
    zend_string *key=NULL,*value=NULL;
    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR(key)
        Z_PARAM_STR(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    zval result;
    array_init(&result);

    zval keey_key;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"key",&keey_key);
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"rewind",NULL);

    if((!key||ZSTR_LEN(key)<=1)&&(!value||ZSTR_LEN(value)<=1)){
        while (lsentity_check_bool_with_0_params(object,"valid")){
            zval res,arr;
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"current",&res);
            if(lsentity_obj_check(lsentity_entity_ce_ptr,&res,0,0)){
                zend_call_method_with_0_params(&res,Z_OBJCE(res),NULL,"asarray",&arr);
                Z_REFCOUNTED(arr)&&Z_ADDREF(arr);
                zend_hash_next_index_insert(Z_ARR(result),&arr);
                zval_ptr_dtor(&arr);
            } else{
                array_init(&arr);
                Z_REFCOUNTED(arr)&&Z_ADDREF(arr);
                zend_hash_next_index_insert(Z_ARR(result),&arr);
                zval_ptr_dtor(&arr);
            }
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"next",NULL);
            zval_ptr_dtor(&res);
        }
    }else if( !key||ZSTR_LEN(key)<=1 ){
        zval zvalue;
        ZVAL_STR_COPY(&zvalue,value);
        while (lsentity_check_bool_with_0_params(object,"valid")){
            zval res,arr;
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"current",&res);
            if(lsentity_obj_check(lsentity_entity_ce_ptr,&res,0,0)){
                zend_call_method_with_1_params(&res,Z_OBJCE(res),NULL,"__get",&arr,&zvalue);
                zval tmp;
                ZVAL_DUP(&tmp,&arr);
				Z_REFCOUNTED(tmp)&&Z_ADDREF(tmp);
                zend_hash_next_index_insert(Z_ARR(result),&tmp);
                zval_ptr_dtor(&tmp);
				zval_ptr_dtor(&arr);
            } else{
                ZVAL_NULL(&arr);
				Z_REFCOUNTED(arr)&&Z_ADDREF(arr);
                zend_hash_next_index_insert(Z_ARR(result),&arr);
				zval_ptr_dtor(&arr);
            }
			zval_ptr_dtor(&res);
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"next",NULL);
        }
        zval_ptr_dtor(&zvalue);
    }else if( !value||ZSTR_LEN(value)<=1  ){
        while (lsentity_check_bool_with_0_params(object,"valid")){
            zval res,arr;
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"current",&res);
            if(lsentity_obj_check(lsentity_entity_ce_ptr,&res,0,0)){
                zend_call_method_with_0_params(&res,Z_OBJCE(res),NULL,"asarray",&arr);
                zval * val=zend_hash_find(Z_ARR(arr),key);
                Z_REFCOUNTED(arr)&&Z_ADDREF(arr);
                add_assoc_zval(&result,Z_STRVAL_P(val),&arr);
                zval_ptr_dtor(&arr);
            }
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"next",NULL);
            zval_ptr_dtor(&res);
        }
    }else{
        zval zkey,zvalue;
        ZVAL_STR_COPY(&zkey,key);
        ZVAL_STR_COPY(&zvalue,value);
        while (lsentity_check_bool_with_0_params(object,"valid")){
            zval res,rkey,rvalue;
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"current",&res);
            if(lsentity_obj_check(lsentity_entity_ce_ptr,&res,0,0)){
                zend_call_method_with_1_params(&res,Z_OBJCE(res),NULL,"__get",&rkey,&zkey);
                zend_call_method_with_1_params(&res,Z_OBJCE(res),NULL,"__get",&rvalue,&zvalue);
                convert_to_string(&rkey);
                Z_REFCOUNTED(rvalue)&&Z_ADDREF(rvalue);
                add_assoc_zval(&result,Z_STRVAL(rkey),&rvalue);
                zval_ptr_dtor(&rkey);
                zval_ptr_dtor(&zvalue);
            }
            zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"next",NULL);
            zval_ptr_dtor(&res);
        }
        zval_ptr_dtor(&zkey);
        zval_ptr_dtor(&zvalue);
    }
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"rewind",NULL);
    while (lsentity_check_bool_with_0_params(object,"valid")){
        zval stkey,sres;
        zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"key",&stkey);
        if(is_equal_function(&sres,&stkey,&keey_key)==SUCCESS){
            zval_ptr_dtor(&stkey);
            zval_ptr_dtor(&sres);
            break;
        }
        zval_ptr_dtor(&stkey);
        zval_ptr_dtor(&sres);
        zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"next",NULL);
    }
    zval_ptr_dtor(&keey_key);
    RETURN_ZVAL(&result,1,1);
}
ZEND_METHOD(lsentity_entity_set_class, current){

    zval *object;
    object = getThis();
    zval *result=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_result"),0,NULL);
    zval ret;
    zend_call_method_with_0_params(result,Z_OBJCE_P(result),NULL,"current",&ret);
    zval *entity;
    entity = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_entity"),0,NULL);

    if(Z_TYPE(ret)==IS_NULL)RETURN_NULL();

    zend_class_entry *ce;
    if ((ce = zend_lookup_class(Z_STR_P(entity))) == NULL) {
        if (!EG(exception)) {
            zend_throw_exception_ex(lsentity_exception_ce_ptr, -1, "Class %s does not exist", Z_STRVAL_P(entity));
        }
        RETURN_NULL();
    }

    zval *table;
    table = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),0,NULL);

    zval param[]={
        *table
    };
    lsentity_new_class(ce,return_value,param,1);
	
	if(Z_TYPE_P(result)!=IS_OBJECT&&Z_TYPE_P(return_value)==IS_OBJECT){
		zval outtable;
		zend_call_method_with_0_params(return_value,Z_OBJCE_P(return_value),NULL,"table",&outtable);
		zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),&outtable);
		zval_ptr_dtor(&outtable);
	}
	


    zval *columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),0,NULL);


    zend_fcall_info fci;
    zval retval;
    zval params[3];

    zval ptrue;
    ZVAL_TRUE(&ptrue);

    ZVAL_COPY_VALUE(&params[0], &ret);
    ZVAL_COPY_VALUE(&params[1], columns);
    ZVAL_COPY_VALUE(&params[2], &ptrue);
    zval_ptr_dtor(&ptrue);

    fci.size = sizeof(fci);
    fci.object = Z_OBJ_P(return_value);
    fci.retval = &retval;
    fci.param_count = 3;
    fci.params = params;
    fci.no_separation = 1;


    zend_fcall_info_cache fcic;
    #if PHP_VERSION_ID<70300 //@todo　//@todo php7.2版本不一样
    fcic.initialized = 1;
    #endif




    ZVAL_UNDEF(&fci.function_name); /* Unused */

    zend_class_entry * obj_ce = Z_OBJCE_P(return_value);

    HashTable *function_table = &obj_ce->function_table;
    fcic.function_handler = zend_hash_str_find_ptr(
            function_table, ZEND_STRL("loaddata"));
    if (fcic.function_handler == NULL) {
        /* error at c-level */
        zend_error_noreturn(E_CORE_ERROR, "Couldn't find implementation for method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", "loadData");
        RETURN_NULL();
    }
    fcic.calling_scope = obj_ce;
    fcic.object = Z_OBJ_P(return_value);
    int _result = zend_call_function(&fci, &fcic);
    if (_result == FAILURE) {
        if (!EG(exception)) {
            //@todo bug,,,,
            zend_error_noreturn(E_CORE_ERROR, "Couldn't execute method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", "loadData");
        }
    }
    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&ret);


}
ZEND_METHOD(lsentity_entity_set_class, valid){
    zval *object;
    object = getThis();
    zval *result=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_result"),0,NULL);
    zval ret;
    zend_call_method_with_0_params(result,Z_OBJCE_P(result),NULL,"valid",&ret);
    RETURN_ZVAL(&ret,1,1);
}

static zend_function_entry lsentity_entity_set_class_method[] = {
    ZEND_ME(lsentity_entity_set_class,__construct, lsentity_entity_set_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,setFetchFree, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,fetchCount, lsentity_entity_set_fetch_count_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,asArray, lsentity_entity_set_asarr_arginfo, ZEND_ACC_PUBLIC)

    ZEND_ME(lsentity_entity_set_class,key, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,next, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,rewind, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,current, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_set_class,valid, NULL, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

void lsentity_entity_set_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"EntitySet",lsentity_entity_set_class_method);
    lsentity_entity_set_ce_ptr = zend_register_internal_class(&ce);
    zend_do_implement_interface(lsentity_entity_set_ce_ptr, zend_ce_iterator);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_result"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_columns"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_table"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(lsentity_entity_set_ce_ptr,ZEND_STRL("_entity"), ZEND_ACC_PROTECTED);
}

