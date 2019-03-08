

#include <Zend/zend_smart_str.h>
#include "zend.h"
#include "zend_hash.h"
#include "php.h"
#include "zend_API.h"
#include "zend_API.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "ext/standard/php_array.h"
#include "ext/standard/php_string.h"
#include "utils.h"
#include "entity.h"
#include "class_entity.h"
#include "class_table.h"
#include "class_filter.h"
#include "class_validation.h"
#include "class_column_set.h"
#include "class_column.h"
#include "class_entity_column_set.h"
#include "class_exception.h"
#include "class_column_save.h"
#include "class_db.h"

ZEND_API zend_class_entry *lsentity_entity_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_set_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_get_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_isset_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_construct_arginfo, 0, 0, 0)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, table, Table, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_load_data_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, data,0)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, table, EntityColumnSet, 1)
    ZEND_ARG_TYPE_INFO(0,loaded,IS_DOUBLE,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_columns_arginfo, 0, 0, 0)
    ZEND_ARG_TYPE_INFO(0,patch,IS_DOUBLE,1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_check_arginfo, 0, 0, 0)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, validation, Validation, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_values_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, values,0)
    ZEND_ARG_ARRAY_INFO(0, expected,1)
ZEND_END_ARG_INFO()



static int obj_check(zend_class_entry * ce,zval *retobj,int throw){
    if(zend_object_is_true(retobj)&&instanceof_function(Z_OBJCE_P(retobj),ce)) {
        return 1;
    }else{
        zval_ptr_dtor(retobj);
        if(throw)zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "the table method not return %s object",ZSTR_VAL(ce->name));
        return 0;
    }
}

static int get_table(zval *object,zval *table){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "table", table);
    return obj_check(lsentity_table_ce_ptr,table,1);
}
static int get_table_columns(zval *table,zval *table_columns){
    zend_call_method_with_0_params(table,Z_OBJCE_P(table), NULL, "tablecolumns", table_columns);
    return obj_check(lsentity_entity_column_set_ce_ptr,table_columns,1);
}
static int get_table_pk(zval *table,zval *pk){
    zend_call_method_with_0_params(table,Z_OBJCE_P(table), NULL, "primarykey", &pk);
    if (Z_TYPE(pk)!=IS_STRING){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "the table primarykey not return string");
        return 0;
    }
    return 1;
}
static int get_db(zval *object,zval *db){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "db", db);
    return obj_check(lsentity_db_ce_ptr,db,1);
}
static int get_filter(zval *object,zval *filter){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "filter", filter);
    return obj_check(lsentity_filter_ce_ptr,filter,1);
}
static int get_validation(zval *object,zval *validation,int throw){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "validation", validation);
    return obj_check(lsentity_validation_ce_ptr,validation,throw);
}
static int get_columns(zval *object,zval *columns,int patch){
    zval param;
    ZVAL_BOOL(&param,patch?1:0);
    zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "columns", columns,&param);
    return obj_check(lsentity_column_set_ce_ptr,columns,1);
}
static int get_column(zval *columns,zend_string *column_str,zval *column,int throw){
    zval param;
    ZVAL_STR(&param,column_str);
    zend_call_method_with_1_params(columns,Z_OBJCE_P(columns), NULL, "offsetGet",column,&param);
    return obj_check(lsentity_column_ce_ptr,column,throw);
}
static int check_bool_with_0_params(zval *object,const char* method){
    zval isexist;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, method, &isexist);
    int ret=zend_is_true(&isexist);
    zval_ptr_dtor(&isexist);
    return ret;
}
static int check_bool_with_1_params(zval *object,const char* method,zval *param){
    zval isexist;
    zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, method, &isexist,param);
    int ret=zend_is_true(&isexist);
    zval_ptr_dtor(&isexist);
    return ret;
}
static int check_bool_with_2_params(zval *object,const char* method,zval *param1,zval *param2){
    zval isexist;
    zend_call_method_with_2_params(object,Z_OBJCE_P(object), NULL, method, &isexist,param1,param2);
    int ret=zend_is_true(&isexist);
    zval_ptr_dtor(&isexist);
    return ret;
}

static zval * cache_obj_get(zval *object,const char *attr,const char *method,zend_class_entry *ce,zval * return_value){
    zval * value=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL(attr),1,NULL);
    if(zend_is_true(value)) return value;
    zend_call_method_with_0_params(object, Z_OBJCE_P(object), NULL, method, return_value);
    if(!obj_check(ce,return_value,1))return NULL;
    if(!check_bool_with_0_params(return_value,"allowcache")){
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL(attr),return_value);
    }
    return NULL;
}


ZEND_METHOD(lsentity_entity_class, __construct){
    zval *table_object,*object;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_OBJECT_OF_CLASS_EX(table_object, lsentity_table_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    if(zend_object_is_true(object)) zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),table_object);
    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),&temp_array);
    zval_ptr_dtor(&temp_array);
}
ZEND_METHOD(lsentity_entity_class, __isset){
    zend_string *column;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(column)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object;
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    if (zend_symtable_exists_ind(Z_ARR_P(data), column)) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}
ZEND_METHOD(lsentity_entity_class, __set){
    zend_string *column;
    zval *value;
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(column)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *object=getThis();

    zval columns;
    if(!get_columns(object,&columns,1)) RETURN_NULL();
    zval zval_column;
    ZVAL_STR(&zval_column,column);
    if(!check_bool_with_1_params(&columns,"offsetexists",&zval_column)){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "the %s property does not exist", ZSTR_VAL(column));
        RETURN_NULL();
    }
    zval filter;
    zval newval;
    if(get_filter(object,&filter)){
        zend_call_method_with_2_params(&filter,Z_OBJCE(filter), NULL, "runfilter",&newval,&zval_column,value);
    }
    zval_ptr_dtor(&filter);

    zval *newvalptr =zend_is_true(&newval)?&newval:value;

    int loaded=check_bool_with_0_params(object,"loaded");

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval columnobj;
    if(!get_column(&columns,column,&columnobj,1))RETURN_NULL();
    if (loaded){
        if (zend_symtable_exists_ind(Z_ARR_P(data), column)) {
            zval* oldval=zend_hash_find(data,column);
            zval nullval;
            if(!oldval)ZVAL_NULL(&nullval);
            if (check_bool_with_2_params(&columnobj,"compare",oldval?oldval:&nullval,newvalptr)){
                zend_hash_update(Z_ARR_P(data),column,newvalptr);
                zval_ptr_dtor(&newval);
                zval_ptr_dtor(&columns);
                zval_ptr_dtor(&columnobj);
                RETURN_NULL();
            }
        }
    }

    zval *change=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),1,NULL);
    if (!zend_symtable_exists_ind(Z_ARR_P(change), column)) {
        zval* oldval=zend_hash_find(data,column);
        zval nullval;
        if(!oldval)ZVAL_NULL(&nullval);
        zend_hash_update(Z_ARR_P(change),column,oldval?oldval:&nullval);
    }


    zval table,pk;
    if(!get_table(object,&table)) RETURN_NULL();
    if(!get_table_pk(&table,&pk))RETURN_NULL();
    if (loaded&&zend_string_equals(Z_STR(pk),column)){
        zval* oldval=zend_hash_find(change,column);
        zval nullval;
        if(!oldval)ZVAL_NULL(&nullval);
        if (check_bool_with_2_params(&columnobj,"compare",oldval?oldval:&nullval,newvalptr)){
            zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),1);
        }else{
            zval *val;
            zend_string * key;
            ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data),key,val) {
                if(!zend_symtable_exists_ind(Z_ARR_P(change),key)){
                    zend_hash_update(Z_ARR_P(change),key,val);
                }
            } ZEND_HASH_FOREACH_END();
            zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),0);
        }
    }
    zval_ptr_dtor(&pk);
    zval_ptr_dtor(&table);
    zval_ptr_dtor(&newval);
    zval_ptr_dtor(&columns);
    zval_ptr_dtor(&columnobj);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),0);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),0);

    zend_hash_update(Z_ARR_P(data),column,newvalptr);
}
ZEND_METHOD(lsentity_entity_class, __get){
    zend_string *column;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(column)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval* oldval=zend_hash_find(data,column);
    if(oldval) RETURN_ZVAL(oldval,0,0);
    zval columns;
    if(!get_columns(object,&columns,1)) RETURN_NULL();
    zval obj;
    if(get_column(&columns,column,&obj,0)){
        zval valobj;
        zend_call_method_with_0_params(&obj, Z_OBJCE(obj), NULL, "getdefault", &valobj);
        zval_ptr_dtor(&columns);
        zval_ptr_dtor(&obj);
        RETURN_ZVAL(&valobj, 1, 1);
    }
    zval_ptr_dtor(&columns);
    zend_throw_exception_ex(lsentity_exception_ce_ptr, -1, "The %s property does not exist in the %s entity", ZSTR_VAL(column),ZSTR_VAL(Z_OBJCE_P(object)->name));
}
ZEND_METHOD(lsentity_entity_class, __toString){
    zval *object=getThis();
    zval pk;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "pk",&pk);
    convert_to_string(&pk);
    RETURN_ZVAL(&pk,1,1);
}
ZEND_METHOD(lsentity_entity_class, labels){
    zval temp_array;
    array_init(&temp_array);
    RETURN_ZVAL(&temp_array,1,1);
}

ZEND_METHOD(lsentity_entity_class, filter){
    zval *object=getThis();
    zval obj;
    zval *objptr=cache_obj_get(object,"_filter","filterfactory",lsentity_filter_ce_ptr,&obj);
    if(!objptr)RETURN_NULL();
    RETURN_ZVAL(objptr,1,zend_is_true(&obj));
}
ZEND_METHOD(lsentity_entity_class, validation){
    zval *object=getThis();
    zval obj;
    zval *objptr=cache_obj_get(object,"_validation","validationfactory",lsentity_validation_ce_ptr,&obj);
    if(!objptr)RETURN_NULL();
    RETURN_ZVAL(objptr,1,zend_is_true(&obj));
}
ZEND_METHOD(lsentity_entity_class, table){
    zval *object=getThis();
    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),1,NULL);
    if(!ZVAL_IS_NULL(valid)) RETURN_ZVAL(valid,0,0);
    zval cls;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "tableclass", &cls);
    if (Z_TYPE(cls)!=IS_STRING){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "tableClass method not return string");
        RETURN_NULL();
    }
    zval ret;
    if(!lsentity_new_class(&cls,&ret,NULL,0)){
        zval_ptr_dtor(&cls);
        zval_ptr_dtor(&ret);
        RETURN_NULL();
    }
    zval_ptr_dtor(&cls);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),&ret);
    RETURN_ZVAL(&ret,1,1);
}
ZEND_METHOD(lsentity_entity_class, loadData){
    zval *data, *entity_column_set;
    zend_bool loaded = 1;
    ZEND_PARSE_PARAMETERS_START(1, 3)
            Z_PARAM_ARRAY_EX(data, 0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(entity_column_set, lsentity_entity_column_set_ce_ptr, 0, 0)
            Z_PARAM_BOOL(loaded)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object=getThis();
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "clear", NULL);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"),entity_column_set);
    zval columns;
    if(!get_columns(object,&columns,1)) RETURN_NULL();
    zval *val;
    zend_string * key;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data),key,val) {
        zval obj;
        if(get_column(&columns,key,&obj,0)){
            zval valobj;
            zend_call_method_with_1_params(&obj,Z_OBJCE(obj), NULL, "read", &valobj,val);
            zend_hash_update(Z_ARR_P(data),key,&valobj);
            zval_ptr_dtor(&valobj);
        }else{
            zend_hash_del(Z_ARR_P(data),key);//????
        }
        zval_ptr_dtor(&obj);
    } ZEND_HASH_FOREACH_END();
    zval_ptr_dtor(&columns);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),data);

    if (loaded){
        zval table,pk;
        if(get_table(object,&table)){
            if(get_table_pk(&table,&pk)){
                zval_ptr_dtor(&pk);
                if(!zend_symtable_exists_ind(Z_ARR_P(data), Z_STR(pk))) loaded=0;
            }
            zval_ptr_dtor(&table);
        }
    }
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),loaded);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),loaded);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),loaded);
    RETURN_ZVAL(object,0,0);
}
ZEND_METHOD(lsentity_entity_class, exportData){
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    RETURN_ZVAL(data,0,0);
}
ZEND_METHOD(lsentity_entity_class, changed){
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval *change=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),1,NULL);
    zval retval;
    zval params[2];
    ZVAL_ZVAL(&params[0],data,0,0);
    ZVAL_ZVAL(&params[1],change,0,0);
    zend_fcall_info fcif;
    fcif.size = sizeof(fcif);
    fcif.object = NULL;
    fcif.retval = &retval;
    fcif.param_count = 2;
    fcif.params = params;
    fcif.no_separation = 1;
    ZVAL_STRINGL(&fcif.function_name,"array_intersect_key",strlen("array_intersect_key"));
    if (zend_call_function(&fcif, NULL) == FAILURE||Z_TYPE(retval)!=IS_ARRAY){
        zval arr;
        array_init(&arr);
        RETURN_ZVAL(&arr,1,1);
    }
    zval_ptr_dtor(&fcif.function_name);
    RETURN_ZVAL(&retval,1,1);
}
ZEND_METHOD(lsentity_entity_class, clear){
    zval *object=getThis();
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),0);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),0);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),0);
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_columns"));
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"));
    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),&temp_array);
    zval_ptr_dtor(&temp_array);
    RETURN_ZVAL(object,0,0);
}
ZEND_METHOD(lsentity_entity_class, pk){
    zval *object,table,pk,*ret=NULL;
    object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);

    if(get_table(object,&table)){
        if(get_table_pk(&table,&pk)){
            if (zend_symtable_exists_ind(Z_ARR_P(data), Z_STR(pk))) {
                ret=zend_hash_find(Z_ARR_P(data),Z_STR(pk));
            }
            zval_ptr_dtor(&pk);
        }
        zval_ptr_dtor(&table);
    }
    if(ret) RETURN_ZVAL(ret,0,1);
    RETURN_NULL();
}
ZEND_METHOD(lsentity_entity_class, columns){

    zend_bool patch=0;
    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_BOOL(patch)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *object;
    object=getThis();
    zval *columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    if (!zend_object_is_true(columns)){
        zval table;
        if(get_table(object,&table)){
            zval table_columns;
            if(get_table_columns(&table,&table_columns)){
                zval *query_columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"),1,NULL);
                if(!obj_check(lsentity_entity_column_set_ce_ptr,query_columns,0)){
                    zval merge_table_columns,zpatch;
                    ZVAL_BOOL(&zpatch,patch);
                    zend_call_method_with_2_params(&table,Z_OBJCE(table), NULL, "ascolumnset", &merge_table_columns,&table_columns,&zpatch);
                    if(obj_check(lsentity_column_ce_ptr,&merge_table_columns,0)){
                        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),&merge_table_columns);
                    }
                    zval_ptr_dtor(&merge_table_columns);
                }else{
                    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),&table_columns);
                }
                zval_ptr_dtor(&table_columns);
            }
            zval_ptr_dtor(&table);
        }
    }
    columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    RETURN_ZVAL(columns,0,0);
}

ZEND_METHOD(lsentity_entity_class, saved){
    zval *object=getThis();
    zval *loaded=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1,NULL);
    RETURN_ZVAL(loaded,0,0);
}
ZEND_METHOD(lsentity_entity_class, loaded){
    zval *object=getThis();
    zval *loaded=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),1,NULL);
    RETURN_ZVAL(loaded,0,0);
}
ZEND_METHOD(lsentity_entity_class, update){
    zval *valid_object,*object;
    ZEND_PARSE_PARAMETERS_START(0, 0)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();

    if (!check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot update %s model because it is not loaded",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }
    zval table,db;
    if(!get_table(object,&table))RETURN_NULL();
    if(!get_db(object,&db)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval _table_name;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"tablename",&_table_name);
    if(Z_TYPE(_table_name)!=IS_STRING){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zval table_name;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotetable",&table_name,&_table_name);
    zval_ptr_dtor(&_table_name);
    if(Z_TYPE(table_name)!=IS_STRING){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zval _pkcol,pkcol;
    if(!get_table_pk(&table,&_pkcol)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,&_pkcol);
    if(Z_TYPE(pkcol)!=IS_STRING){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&pkcol);
        RETURN_NULL();
    }
    zval columns;
    if(!get_columns(&object,&columns,0)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&pkcol);
        RETURN_NULL();
    }

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zend_object_iterator   *iter;
    zend_class_entry       *ce = Z_OBJCE(columns);
    zval                   *val;
    iter = ce->get_iterator(ce, &columns, 0);
    if (EG(exception)) {
        goto iterator_done;
    }
    if (iter->funcs->rewind) {
        iter->funcs->rewind(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
    }
    zval save_data;
    array_init(&save_data);
    while (iter->funcs->valid(iter) == SUCCESS) {
        if (EG(exception)) {
            goto iterator_done;
        }
        val = iter->funcs->get_current_data(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
        if(obj_check(lsentity_column_ce_ptr,val,0)){
            zval name;
            zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
            if(obj_check(lsentity_column_save_ce_ptr,val,0)){
                zend_call_method_with_2_params(val,Z_OBJCE_P(val),NULL,"update",NULL,object,&name);
            }
            zval * find=zend_hash_find(Z_ARR_P(data),Z_STR(name));
            if(find)zend_hash_add(Z_ARR(save_data),Z_STR(name),find);
        }
        iter->funcs->move_forward(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
    }

    if(zend_array_count(Z_ARR(save_data))==0){
        zval temp_array;
        array_init(&temp_array);
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
        zval_ptr_dtor(&temp_array);

        zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),1);
        zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1);

        RETURN_ZVAL(object,0,0);
    }

    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),1,NULL);
    if(!zend_is_true(valid)|| zend_object_is_true(valid_object)){
        zend_call_method_with_1_params(object,Z_OBJCE_P(object),NULL,"check",NULL,valid_object);
    }

    zval sets;
    array_init(&sets);
    zval *dval;
    zend_string *dkey;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data),dkey,dval) {
                zval _field;
                zval key;
                ZVAL_STR(&key,dkey);
                zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_field,&key);
                zval _type;
                zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&_type,&key);
                zval _sdata;
                zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_sdata,&dval,&_type);

                smart_str set = {0};
                smart_str_append(&set, Z_STR(key));
                smart_str_append(&set, " = ");
                smart_str_append(&set, Z_STR(_sdata));
                smart_str_0(&set);
                zval str;
                ZVAL_STR(&str,set.s);
                add_next_index_str(&sets,&str);

                smart_str_free(&set);
                zval_ptr_dtor(&str);
                zval_ptr_dtor(&_type);
                zval_ptr_dtor(&_field);
                zval_ptr_dtor(&key);
                zval_ptr_dtor(&_sdata);
    } ZEND_HASH_FOREACH_END();
    zval str_set;
    zend_string *glue = zend_string_init(ZEND_STRL(","), 0);
    php_implode(glue, &sets, &str_set);
    zend_string_release(glue);


    zval type,_pk,pk;
    ZVAL_NULL(&type);
    zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&_pkcol);
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"pk",&_pk);
    zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&pk,&_pk,&type);
    zval_ptr_dtor(&type);
    zval_ptr_dtor(&_pk);
    zval_ptr_dtor(&_pkcol);
    zval_ptr_dtor(&table);


    smart_str sql = {0};
    smart_str_appends(&sql, " UPDATE ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_append(&sql, " SET ");
    smart_str_append(&sql, Z_STR(str_set));
    smart_str_append(&sql, " WHERE ");
    smart_str_append(&sql, Z_STR(pkcol));
    smart_str_append(&sql, " = ");
    smart_str_append(&sql, Z_STR(pk));
    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR(&zsql,sql.s);
    zval status;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);
    zval_ptr_dtor(&status);
    zval_ptr_dtor(&table_name);
    zval_ptr_dtor(&str_set);
    zval_ptr_dtor(&zsql);


    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zval_ptr_dtor(&temp_array);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1);


    iterator_done:
    //
    RETURN_ZVAL(object,0,0);



}
ZEND_METHOD(lsentity_entity_class, create){
    zval *valid_object,*object;
    ZEND_PARSE_PARAMETERS_START(0, 0)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();

    if (check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot create %s model because it is already loaded",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }
    zval table,db;
    if(!get_table(object,&table))RETURN_NULL();
    if(!get_db(object,&db)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval _table_name;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"tablename",&_table_name);
    if(Z_TYPE(_table_name)!=IS_STRING){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zval table_name;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotetable",&table_name,&_table_name);
    zval_ptr_dtor(&_table_name);
    if(Z_TYPE(table_name)!=IS_STRING){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zval _pkcol,pkcol;
    if(!get_table_pk(&table,&_pkcol)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,&_pkcol);
    if(Z_TYPE(pkcol)!=IS_STRING){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&pkcol);
        RETURN_NULL();
    }
    zval columns;
    if(!get_table_columns(&table,&columns)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&pkcol);
        RETURN_NULL();
    }

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zend_object_iterator   *iter;
    zend_class_entry       *ce = Z_OBJCE(columns);
    zval                   *val;
    iter = ce->get_iterator(ce, &columns, 0);
    if (EG(exception)) {
        goto iterator_done;
    }
    if (iter->funcs->rewind) {
        iter->funcs->rewind(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
    }
    zval save_data;
    array_init(&save_data);
    while (iter->funcs->valid(iter) == SUCCESS) {
        if (EG(exception)) {
            goto iterator_done;
        }
        val = iter->funcs->get_current_data(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
        if(obj_check(lsentity_column_ce_ptr,val,0)){
            zval name;
            zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
            if(obj_check(lsentity_column_save_ce_ptr,val,0)){
                zend_call_method_with_2_params(val,Z_OBJCE_P(val),NULL,"create",NULL,object,&name);
            }
            if(!zend_symtable_exists_ind(Z_ARR_P(data),Z_STR(name))&&check_bool_with_0_params(val,"usedefault")){
                zval def;
                zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"getdefault",&def);
                zend_hash_update(Z_ARR_P(data),Z_STR(name),&def);
                zval_ptr_dtor(&def);
            }
            zval * find=zend_hash_find(Z_ARR_P(data),Z_STR(name));
            if(find)zend_hash_add(Z_ARR(save_data),Z_STR(name),find);
        }
        iter->funcs->move_forward(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
    }


    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),1,NULL);
    if(!zend_is_true(valid)|| zend_object_is_true(valid_object)){
        zend_call_method_with_1_params(object,Z_OBJCE_P(object),NULL,"check",NULL,valid_object);
    }

    zval field,sdata;
    array_init(&field);
    array_init(&sdata);
    zval *dval;
    zend_string *dkey;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data),dkey,dval) {
        zval _field;
        zval key;
        ZVAL_STR(&key,dkey);
        zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_field,&key);
        add_next_index_str(&field,&_field);
        zval _type;
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&_type,&key);
        zval _sdata;
        zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_sdata,&dval,&_type);
        add_next_index_str(&field,&_sdata);
        zval_ptr_dtor(&_type);
        zval_ptr_dtor(&_field);
        zval_ptr_dtor(&key);
        zval_ptr_dtor(&_sdata);
    } ZEND_HASH_FOREACH_END();
    zval str_field;
    zend_string *glue = zend_string_init(ZEND_STRL(","), 0);
    php_implode(glue, &field, &str_field);
    zval str_data;
    php_implode(glue, &sdata, &str_data);
    zend_string_release(glue);

    smart_str sql = {0};
    smart_str_appends(&sql, " INSERT INTO ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_append(&sql, " ( ");
    smart_str_append(&sql, Z_STR(str_field));
    smart_str_append(&sql, " ) VALUES (");
    smart_str_append(&sql, Z_STR(str_data));
    smart_str_append(&sql, " )");
    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR(&zsql,sql.s);
    zval status;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);
    zval_ptr_dtor(&status);
    zval_ptr_dtor(&table_name);
    zval_ptr_dtor(&str_field);
    zval_ptr_dtor(&str_data);
    zval_ptr_dtor(&zsql);


    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zval_ptr_dtor(&temp_array);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),0);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),0);


    if(zend_symtable_exists_ind(data,&pkcol)){
        zval ind;
        zend_call_method_with_0_params(&db,Z_OBJCE(db),NULL,"insertid",&ind);
        zend_hash_update(Z_ARR_P(data),Z_STR(pkcol),&ind);
        zval_ptr_dtor(&ind);
    }
    zval_ptr_dtor(&pkcol);
//    RETURN_ZVAL(object,0,0);
    iterator_done:
        RETURN_ZVAL(object,0,0);
}
ZEND_METHOD(lsentity_entity_class, delete){
    zval *object=getThis();
    if (!check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot delete %s model because it is not loaded.",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }
    zval table,db;
    if(!get_table(object,&table))RETURN_NULL();
    if(!get_db(object,&db)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval _table_name;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"tablename",&_table_name);
    if(Z_TYPE(_table_name)!=IS_STRING){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zval table_name;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotetable",&table_name,&_table_name);
    zval_ptr_dtor(&_table_name);
    if(Z_TYPE(table_name)!=IS_STRING){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zval _pkcol,pkcol;
    if(!get_table_pk(&table,&_pkcol)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,&_pkcol);
    if(Z_TYPE(pkcol)!=IS_STRING){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&pkcol);
        RETURN_NULL();
    }
    zval columns;
    if(!get_table_columns(&table,&columns)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&pkcol);
        RETURN_NULL();
    }
    zval type,_pk,pk;
    ZVAL_NULL(&type);
    zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&_pkcol);
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"pk",&_pk);
    zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&pk,&_pk,&type);
    zval_ptr_dtor(&type);
    zval_ptr_dtor(&_pk);
    zval_ptr_dtor(&_pkcol);
    zval_ptr_dtor(&table);

    convert_to_string(&pk);
    convert_to_string(&pkcol);
    convert_to_string(&table_name);

    smart_str sql = {0};

    smart_str_appends(&sql, " DELETE FROM ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_append(&sql, " WHERE ");
    smart_str_append(&sql, Z_STR(pkcol));
    smart_str_append(&sql, " = ");
    smart_str_append(&sql, Z_STR(pk));
    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR(&zsql,sql.s);

    zval status;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);
    zval_ptr_dtor(&status);
    zval_ptr_dtor(&pk);
    zval_ptr_dtor(&pkcol);
    zval_ptr_dtor(&table_name);
    zval_ptr_dtor(&zsql);
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"clear",return_value);
}
ZEND_METHOD(lsentity_entity_class, values){
    zval *values,*expected;
    ZEND_PARSE_PARAMETERS_START(0, 0)
            Z_PARAM_ARRAY(values)
            Z_PARAM_OPTIONAL
            Z_PARAM_ARRAY(expected)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object=getThis();
    zval arr;
    array_init(&arr);
    if (zend_is_true(expected)) {
        php_array_merge(&arr, expected);
    }else{
        zval columns;
        if(get_columns(object,&columns,1)){
            zval param1,defarr;
            ZVAL_LONG(&param1,COLUMN_SET_TYPE_FIELD);
            zend_call_method_with_1_params(&columns,Z_OBJCE(columns), NULL, "asarray", &defarr,&param1);
            php_array_merge(&arr,Z_ARR(defarr));
            zval_ptr_dtor(&defarr);
            zval_ptr_dtor(&columns);
        }
        zval table,pk;
        if(get_table(object,&table)){
            if(get_table_pk(&table,&pk)){
                zend_hash_del(&arr,Z_STR(pk));
                zval_ptr_dtor(&pk);
            }
            zval_ptr_dtor(&table);
        }
    }
    zval *col;
    ZEND_HASH_FOREACH_VAL(Z_ARR(arr),col) {
        zval* oldval=zend_hash_find(Z_ARR_P(values),Z_STR_P(col));
        if(oldval)zend_call_method_with_2_params(object,Z_OBJCE_P(object),Z_OBJCE_P(object)->__set, "__set", NULL,col,oldval);
    } ZEND_HASH_FOREACH_END();
    RETURN_ZVAL(object,0,0);
}
ZEND_METHOD(lsentity_entity_class, save){
    zval *valid_object,*object;
    ZEND_PARSE_PARAMETERS_START(0, 0)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();
    if (check_bool_with_0_params(object,"loaded")){
        zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "create", NULL,valid_object);
    }else{
        zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "update", NULL,valid_object);
    }
    RETURN_ZVAL(object,0,0);
}
ZEND_METHOD(lsentity_entity_class, check){
    zval *valid_object,*object;
    ZEND_PARSE_PARAMETERS_START(0, 0)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();

    zval valid_object_obj;
    zval *valid_ok=NULL;
    if(!zend_object_is_true(valid_object)){
        if(get_validation(object,&valid_object_obj,0)) valid_ok=&valid_object_obj;
    }else valid_ok=valid_object;

    if(!zend_object_is_true(valid_ok)){
        if(zend_object_is_true(&valid_object_obj))zval_ptr_dtor(&valid_object_obj);
        RETURN_ZVAL(object,0,0);
    }

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval arr;
    array_init(&arr);
    php_array_merge(&arr,Z_ARR_P(data));
    zval columns;
    if(get_columns(object,&columns,1)){
        zval param1,defarr;
        ZVAL_LONG(&param1,COLUMN_SET_TYPE_DEFAULT);
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns), NULL, "asarray", &defarr,&param1);
        php_array_merge(&arr,Z_ARR(defarr));
        zval_ptr_dtor(&defarr);
        zval_ptr_dtor(&columns);
    }

    zval valid,param;
    ZVAL_LONG(&param,1);
    zend_call_method_with_1_params(valid_ok,Z_OBJCE_P(valid_ok), NULL, "valid", &valid,&arr);
    zend_bool is_valid=zend_is_true(&valid);
    zval_ptr_dtor(&valid);
    if(zend_object_is_true(&valid_object_obj))zval_ptr_dtor(&valid_object_obj);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),is_valid);

    if(is_valid)RETURN_ZVAL(object,0,0);

    zend_object *obj=zend_throw_exception_ex(lsentity_exception_ce_ptr, 2, "validation data fail");

    zval errdata;
    zend_call_method_with_0_params(valid_ok,Z_OBJCE_P(valid_ok),NULL,"errors",&errdata);
    if(Z_TYPE(errdata)==IS_ARRAY){
        zval throw;
        ZVAL_OBJ(&throw,obj);
        zend_call_method_with_1_params(&throw,Z_OBJCE(throw),NULL,"setvalidationerror",NULL,&errdata);
    }
    zval_ptr_dtor(&errdata);
}

ZEND_METHOD(lsentity_entity_class, asArray){
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval columns;
    if(get_columns(object,&columns,1)){
        zval param1,defarr;
        ZVAL_LONG(&param1,COLUMN_SET_TYPE_DEFAULT);
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns), NULL, "asarray", &defarr,&param1);
        zval arr;
        array_init(&arr);
        php_array_merge(&arr,Z_ARR_P(data));
        php_array_merge(&arr,Z_ARR(defarr));
        zval_ptr_dtor(&columns);
        zval_ptr_dtor(&defarr);
        RETVAL_ZVAL(&arr,1,1);
    }
    RETVAL_ZVAL(data,0,0);
}

ZEND_METHOD(lsentity_entity_class, jsonSerialize){
    zval *object=getThis();
    zend_call_method_with_0_params(object, Z_OBJCE_P(object), NULL, "asarray", return_value);
}
ZEND_METHOD(lsentity_entity_class, filterFactory){
    RETURN_NULL();
}
ZEND_METHOD(lsentity_entity_class, validationFactory){
    RETURN_NULL();
}

static zend_function_entry lsentity_entity_class_method[] = {
    ZEND_ME(lsentity_entity_class,__construct, lsentity_entity_construct_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,__isset, lsentity_entity_isset_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,__set, lsentity_entity_set_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,__get, lsentity_entity_get_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,__toString, NULL, ZEND_ACC_PUBLIC)

    ZEND_ME(lsentity_entity_class,labels, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,filter, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,validation, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,table, NULL, ZEND_ACC_PUBLIC)

    ZEND_ME(lsentity_entity_class,loadData, lsentity_entity_load_data_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,exportData, NULL, ZEND_ACC_PUBLIC)

    ZEND_ME(lsentity_entity_class,changed, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,clear, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,pk, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,columns, lsentity_entity_columns_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,saved, NULL, ZEND_ACC_PUBLIC)

    ZEND_ME(lsentity_entity_class,loaded, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,update, lsentity_entity_check_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,create, lsentity_entity_check_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,save, lsentity_entity_check_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,delete, NULL, ZEND_ACC_PUBLIC)

    ZEND_ME(lsentity_entity_class,check, lsentity_entity_check_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,values, lsentity_entity_values_arginfo, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,asArray, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,jsonSerialize, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,filterFactory, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,validationFactory, NULL, ZEND_ACC_PUBLIC)
    ZEND_ABSTRACT_ME(lsentity_entity_class,tableClass, NULL)

    ZEND_FE_END
};


void lsentity_entity_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_NS,"Entity",lsentity_entity_class_method);
    lsentity_entity_ce_ptr = zend_register_internal_class(&ce );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_table"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_query_column_set"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_columns"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_change"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_data"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_validation"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_filter"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_entity_ce_ptr,ZEND_STRL("_loaded"),0,ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_entity_ce_ptr,ZEND_STRL("_valid"),0, ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_entity_ce_ptr,ZEND_STRL("_saved"),0,ZEND_ACC_PROTECTED );
}

