


#include "zend.h"
#include "zend_hash.h"
#include "php.h"
#include "zend_API.h"
#include "zend_API.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "ext/standard/php_array.h"
#include "ext/standard/php_string.h"
#include "zend_smart_str.h"
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

ZEND_BEGIN_ARG_INFO_EX(lsentity_entity_unset_arginfo, 0, 0, 1)
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



static int get_table(zval *object,zval *table){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "table", table);
    return lsentity_obj_check(lsentity_table_ce_ptr,table,1);
}
static int get_table_columns(zval *table,zval *table_columns){
    zend_call_method_with_0_params(table,Z_OBJCE_P(table), NULL, "tablecolumns", table_columns);

    if(lsentity_obj_check(lsentity_column_set_ce_ptr,table_columns,0))return 1;
    zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "return not a %s object of columns table::tableColumns()",ZSTR_VAL(lsentity_column_set_ce_ptr->name));
    return 0;

}
static int get_table_pk(zval *table,zval *pk){
    zend_call_method_with_0_params(table,Z_OBJCE_P(table), NULL, "primarykey", pk);
    zend_bool  check=1;
    if (Z_TYPE_P(pk)!=IS_STRING)check=0;
    if(!check&&Z_TYPE_P(pk)==IS_ARRAY){
        if(zend_array_count(Z_ARR_P(pk))==0)check=0;
        else{
            zval *val;
            ZEND_HASH_FOREACH_VAL(Z_ARR_P(pk),val) {
                if(Z_TYPE_P(val)!=IS_STRING){
                    check=0;
                    break;
                }
            } ZEND_HASH_FOREACH_END();
        }
    }
    if(!check){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "the table::primaryKey method return not is: string,array string");
        return 0;
    }
    return check;
}
static int get_db(zval *object,zval *db){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "db", db);
    if(lsentity_obj_check(lsentity_db_ce_ptr,db,0))return 1;
    zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "return not a %s object of table::db method",ZSTR_VAL(lsentity_db_ce_ptr->name));
    return 0;
}
static int get_filter(zval *object,zval *filter){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "filter", filter);
    return lsentity_obj_check(lsentity_filter_ce_ptr,filter,0);
}
static int get_validation(zval *object,zval *validation,int throw){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "validation", validation);
    return lsentity_obj_check(lsentity_validation_ce_ptr,validation,throw);
}
static int get_columns(zval *object,zval *columns,int patch){
    zval param;
    ZVAL_BOOL(&param,patch?1:0);
    zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "columns", columns,&param);

    if(lsentity_obj_check(lsentity_column_set_ce_ptr,columns,0))return 1;
    zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "return not a %s object of Entity::columns method",ZSTR_VAL(lsentity_column_set_ce_ptr->name));
    return 0;
}
static int get_column(zval *columns,zend_string *column_str,zval *column,int throw){
    zval param;
    ZVAL_STR(&param,column_str);
    zend_call_method_with_1_params(columns,Z_OBJCE_P(columns), NULL, "offsetget",column,&param);
    return lsentity_obj_check(lsentity_column_ce_ptr,column,throw);
}

static zval * cache_obj_get(zval *object,const char *attr,const char *method,zend_class_entry *ce,zval * return_value){
    zval * value=zend_read_property(Z_OBJCE_P(object),object,attr,strlen(attr),1,NULL);
    if(zend_is_true(value)) return value;
    zend_call_method(object,Z_OBJCE_P(object), NULL, method, strlen(method), return_value, 0, NULL, NULL);
    if(!lsentity_obj_check(ce,return_value,0))return NULL;
    if(!lsentity_check_bool_with_0_params(return_value,"allowcache")){
        zend_update_property(Z_OBJCE_P(object),object,attr,strlen(attr),return_value);
    }
    return NULL;
}


static int comp_pkkey(zval *pk,zval *column){
    if(Z_TYPE_P(pk)==IS_ARRAY){
        return !lsentity_in_array(pk,column);
    }
    return string_compare_function(column,pk);
}

static int arr_intersect_key(zval* retval,zval *data,zval *change){
    zval params[2];
    ZVAL_ZVAL(&params[0],data,0,0);
    ZVAL_ZVAL(&params[1],change,0,0);
    zend_fcall_info fcif;
    fcif.size = sizeof(fcif);
    fcif.object = NULL;
    fcif.retval = retval;
    fcif.param_count = 2;
    fcif.params = params;
    fcif.no_separation = 1;
    ZVAL_STRINGL(&fcif.function_name,"array_intersect_key",strlen("array_intersect_key"));
    if (zend_call_function(&fcif, NULL) == FAILURE||Z_TYPE_P(retval)!=IS_ARRAY){
        array_init(retval);
        return 0;
    }
    zval_ptr_dtor(&fcif.function_name);
    return 1;
}

ZEND_METHOD(lsentity_entity_class, __construct) {
    zval *table_object = NULL, *object;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_OBJECT_OF_CLASS_EX(table_object, lsentity_table_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    object = getThis();
    if (table_object){
        zend_update_property(Z_OBJCE_P(object), object, ZEND_STRL("_table"), table_object);
    }
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
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    if (zend_symtable_exists_ind(Z_ARR_P(data), column)) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}
ZEND_METHOD(lsentity_entity_class, __unset){
    zend_string *column;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(column)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *object=getThis();
    zval table,pk;
    if(!get_table(object,&table)) RETURN_NULL();
    if(!get_table_pk(&table,&pk)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval zcolumn;
    ZVAL_STR(&zcolumn,column);
    if(comp_pkkey(&pk,&zcolumn)==0){
        zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),0);
    }
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zend_hash_del(Z_ARR_P(data),column);

    zval_ptr_dtor(&pk);
    zval_ptr_dtor(&table);
    zval_ptr_dtor(&zcolumn);
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
    if(!lsentity_check_bool_with_1_params(&columns,"offsetexists",&zval_column)){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "the %s property does not exist", ZSTR_VAL(column));
        RETURN_NULL();
    }
    zval filter;
    zval newval;
    if(get_filter(object,&filter)){
        zend_call_method_with_2_params(&filter,Z_OBJCE(filter), NULL, "runfilter",&newval,&zval_column,value);
    }else{
        ZVAL_COPY_VALUE(&newval,value);

    }





    zval_ptr_dtor(&filter);


    int loaded=lsentity_check_bool_with_0_params(object,"loaded");

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval columnobj;
    if(!get_column(&columns,column,&columnobj,1))RETURN_NULL();
    if (loaded){
        if (zend_symtable_exists_ind(Z_ARR_P(data), column)) {
            zval* oldval=zend_hash_find(Z_ARR_P(data),column);
            zval nullval;
            if(!oldval)ZVAL_NULL(&nullval);
            if (lsentity_check_bool_with_2_params(&columnobj,"compare",oldval?oldval:&nullval,&newval)){
                Z_ADDREF_P(&newval);
                zend_hash_update(Z_ARR_P(data),column,&newval);
                zval_ptr_dtor(&newval);
                zval_ptr_dtor(&columns);
                zval_ptr_dtor(&columnobj);
                RETURN_NULL();
            }
        }
    }



    zval *change=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),1,NULL);




    if (!zend_symtable_exists_ind(Z_ARR_P(change), column)) {
        zval* oldval=zend_hash_find(Z_ARR_P(data),column);
        zval nullval;
        if(!oldval)ZVAL_NULL(&nullval);
        else if(Z_REFCOUNTED_P(oldval))Z_ADDREF_P(oldval);
        zend_hash_update(Z_ARR_P(change),column,oldval?oldval:&nullval);
    }


    zval table,pk;
    if(!get_table(object,&table)) RETURN_NULL();
    if(!get_table_pk(&table,&pk)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }




    if(loaded){
        zval zcolumn;
        ZVAL_STR(&zcolumn,column);
        if(comp_pkkey(&pk,&zcolumn)==0){

            zval* oldval=zend_hash_find(Z_ARR_P(change),column);
            zval nullval;
            if(!oldval)ZVAL_NULL(&nullval);
            int is_change=!lsentity_check_bool_with_2_params(&columnobj,"compare",oldval?oldval:&nullval,value);
            if(Z_TYPE(pk)==IS_ARRAY){
                zend_hash_del(Z_ARR(pk),column);
                if(!is_change){
                    zval *col;
                    ZEND_HASH_FOREACH_VAL(Z_ARR(pk),col) {
                        zval* _change=zend_hash_find(Z_ARR_P(change),Z_STR_P(col));
                        if(_change){
                            if(!is_change){
                                zval *_data=zend_hash_find(Z_ARR_P(data),Z_STR_P(col));
                                if(!_data){
                                    zval _sdata;
                                    ZVAL_NULL(&_sdata);
                                    _data=&_sdata;
                                }
                                is_change=!lsentity_check_bool_with_2_params(&columnobj,"compare",_change,_data);
                            }
                            if(!is_change)break;
                        }
                    } ZEND_HASH_FOREACH_END();
                }
            }
            if(is_change){
                zval *val;
                zend_string * key;
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data),key,val) {
                    if(!zend_symtable_exists_ind(Z_ARR_P(change),key)){
                        Z_ADDREF_P(val);
                        zend_hash_update(Z_ARR_P(change),key,val);
                    }
                } ZEND_HASH_FOREACH_END();
                zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),0);
            }else{

                zval *val;
                zend_string * key;
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(change),key,val) {
                    zval *_data=zend_hash_find(Z_ARR_P(data),key);
                    if(!_data){
                        zval _sdata;
                        ZVAL_NULL(&_sdata);
                        _data=&_sdata;
                    }
                    zval obj;
                    if(get_column(&columns,key,&obj,0)&&Z_TYPE(obj)==IS_OBJECT){
                        if(lsentity_check_bool_with_2_params(&obj,"compare",val,_data)){
                            zend_hash_del(Z_ARR_P(change),key);
                        }
                    }
                    zval_ptr_dtor(&obj);
                } ZEND_HASH_FOREACH_END();

                zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),1);
            }
        }
    }

    zval_ptr_dtor(&pk);
    zval_ptr_dtor(&table);
    zval_ptr_dtor(&columns);
    zval_ptr_dtor(&columnobj);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),0);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),0);
    if(Z_REFCOUNTED(newval))Z_ADDREF(newval);


    zend_hash_update(Z_ARR_P(data),column,&newval);

    zval_ptr_dtor(&newval);
}
ZEND_METHOD(lsentity_entity_class, __get){
    zend_string *column;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(column)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval* oldval=zend_hash_find(Z_ARR_P(data),column);
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
    if(Z_TYPE(pk)==IS_ARRAY){
        zval str_set;
        zend_string *glue = zend_string_init(ZEND_STRL(","), 0);
        php_implode(glue, &pk, &str_set);
        zend_string_release(glue);
        RETURN_ZVAL(&str_set,1,1);
    }
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
    RETURN_ZVAL(valid,1,0);
}
ZEND_METHOD(lsentity_entity_class, loadData){
    zval *data, *entity_column_set=NULL;
    zend_bool loaded = 1;
    ZEND_PARSE_PARAMETERS_START(1, 3)
            Z_PARAM_ARRAY_EX(data, 0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(entity_column_set, lsentity_entity_column_set_ce_ptr, 1, 0)
            Z_PARAM_BOOL(loaded)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *object=getThis();
    if(entity_column_set){
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"),entity_column_set);
    }
    zval columns;
    if(!get_columns(object,&columns,1)) RETURN_NULL();
    zval *val;
    zend_string * key;

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data),key,val) {
        zval obj;
        if(get_column(&columns,key,&obj,0)){
            zval valobj;
            zend_call_method_with_1_params(&obj,Z_OBJCE(obj), NULL, "read", &valobj,val);
            Z_ADDREF(valobj);
            zend_hash_update(Z_ARR_P(data),key,&valobj);
           zval_ptr_dtor(&valobj);
        }else{
            zend_hash_del(Z_ARR_P(data),key);
        }
        zval_ptr_dtor(&obj);
    } ZEND_HASH_FOREACH_END();

    zval_ptr_dtor(&columns);
    //zend_print_zval_r(data,0);
    zval *sdata=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    //zend_print_zval_r(data,0);
    zval mdata;
    array_init(&mdata);
    php_array_merge(Z_ARR(mdata),Z_ARR_P(sdata));
    php_array_merge(Z_ARR(mdata),Z_ARR_P(data));

    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),&mdata);
    zval_ptr_dtor(&mdata);

    if (loaded){
        zval table,pk;
        if(get_table(object,&table)){
            if(get_table_pk(&table,&pk)){
                if(Z_TYPE(pk)==IS_ARRAY){
                    zval *col;
                    ZEND_HASH_FOREACH_VAL(Z_ARR(pk),col) {
                        if(!zend_symtable_exists_ind(Z_ARR_P(data), Z_STR_P(col))) loaded=0;
                        if(!loaded)break;
                    } ZEND_HASH_FOREACH_END();
                }else{
                    if(!zend_symtable_exists_ind(Z_ARR_P(data), Z_STR(pk))) loaded=0;
                }
                zval_ptr_dtor(&pk);
            }else loaded=0;
            zval_ptr_dtor(&table);
        }else loaded=0;
    }
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),loaded);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),loaded);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),loaded);
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_entity_class, exportData){
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    RETURN_ZVAL(data,1,0);
}
ZEND_METHOD(lsentity_entity_class, changed){
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval *change=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),1,NULL);
    zval retval;
    arr_intersect_key(&retval,data,change);
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
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_entity_class, pk){
    zval *object,table,pk;
    object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval out,*outp=NULL;
    ZVAL_NULL(&out);
    if(get_table(object,&table)){
        if(get_table_pk(&table,&pk)){
            if(Z_TYPE(pk)==IS_ARRAY){
                zval retval;
                zval params[1];
                ZVAL_ZVAL(&params[0],&pk,0,0);
                zend_fcall_info fcif;
                fcif.size = sizeof(fcif);
                fcif.object = NULL;
                fcif.retval = &retval;
                fcif.param_count = 1;
                fcif.params = params;
                fcif.no_separation = 1;
                ZVAL_STRINGL(&fcif.function_name,"array_flip",strlen("array_flip"));
                if (zend_call_function(&fcif, NULL) == FAILURE||Z_TYPE(retval)!=IS_ARRAY){
                    array_init(&retval);
                }
                zval_ptr_dtor(&fcif.function_name);
                arr_intersect_key(&out,data,&retval);
                zval_ptr_dtor(&retval);
            }else{
                if (zend_symtable_exists_ind(Z_ARR_P(data), Z_STR(pk))) {
                    outp=zend_hash_find(Z_ARR_P(data),Z_STR(pk));
                }
            }
            zval_ptr_dtor(&pk);
        }
        zval_ptr_dtor(&table);
    }
    if(Z_TYPE(out)!=IS_NULL) RETURN_ZVAL(&out,1,1);
    if(outp) RETURN_ZVAL(outp,0,1);
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
    if (Z_TYPE_P(columns)!=IS_OBJECT||!zend_object_is_true(columns)){
        zval table;
        if(get_table(object,&table)){
            zval table_columns;
            if(get_table_columns(&table,&table_columns)){
                zval *query_columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"),1,NULL);
                if(lsentity_obj_check(lsentity_entity_column_set_ce_ptr,query_columns,0)){
                    zval merge_table_columns,zpatch;
                    ZVAL_BOOL(&zpatch,patch);
                    zend_call_method_with_2_params(query_columns,Z_OBJCE_P(query_columns), NULL, "ascolumnset", &merge_table_columns,&table_columns,&zpatch);
                    if(lsentity_obj_check(lsentity_column_ce_ptr,&merge_table_columns,0)){
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
    RETURN_ZVAL(columns,1,0);
}

ZEND_METHOD(lsentity_entity_class, saved){
    zval *object=getThis();
    zval *loaded=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1,NULL);
    RETURN_ZVAL(loaded,1,0);
}
ZEND_METHOD(lsentity_entity_class, loaded){
    zval *object=getThis();
    zval *loaded=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),1,NULL);
    RETURN_ZVAL(loaded,1,0);
}
ZEND_METHOD(lsentity_entity_class, update){
    zval *valid_object=NULL,*object;
    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 1, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();


    if (!lsentity_check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot update %s model because it is not loaded",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }

    zval table,db;
    if(!get_table(object,&table))RETURN_NULL();
    if(!get_db(&table,&db)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval _table_name;
    zend_call_method_with_0_params(&table,Z_OBJCE(table),NULL,"tablename",&_table_name);

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
    zval _pkcol;
    if(!get_table_pk(&table,&_pkcol)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }


    zval columns;
    if(!get_columns(object,&columns,0)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&_pkcol);
        RETURN_NULL();
    }

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval *change=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),1,NULL);
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
        if(lsentity_obj_check(lsentity_column_ce_ptr,val,0)){
            zval name;
            zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
            if(lsentity_obj_check(lsentity_column_save_ce_ptr,val,0)){
                zend_call_method_with_2_params(val,Z_OBJCE_P(val),NULL,"update",NULL,object,&name);
            }
            zval * dfind=zend_hash_find(Z_ARR_P(data),Z_STR(name));
            if(!lsentity_check_bool_with_0_params(val,"isallownull")
               &&dfind
               &&ZVAL_IS_NULL(dfind)){
                zend_update_property_string(Z_OBJCE_P(object),object,ZEND_STRL("_data"),"");
                data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
            }
            zval * find=zend_hash_find(Z_ARR_P(change),Z_STR(name));
            if(find){
                zend_hash_add(Z_ARR(save_data),Z_STR(name),find);
            }
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

        RETURN_ZVAL(object,1,0);
    }


    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),1,NULL);
    if(!zend_is_true(valid)||valid_object){
        zval _valid_object;
        ZVAL_NULL(&_valid_object);
        if(!valid_object)valid_object=&_valid_object;
        zend_call_method_with_1_params(object,Z_OBJCE_P(object),NULL,"check",NULL,valid_object);
    }

    zval sets;
    array_init(&sets);
    zval *dval;
    zend_string *dkey;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR(save_data),dkey,dval) {
                zval _field;
                zval key;
                ZVAL_STR(&key,dkey);
                zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_field,&key);
                zval _type;
                zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&_type,&key);
                zval _sdata;
                zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_sdata,dval,&_type);
                //@todo 有问题...
                smart_str set = {0};
                smart_str_append(&set, Z_STR(key));
                smart_str_appends(&set, " = ");
                smart_str_append(&set, Z_STR(_sdata));
                smart_str_0(&set);
                zval str;
                ZVAL_STR(&str,set.s);
                add_next_index_str(&sets,Z_STR(str));

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




    smart_str sql = {0};//@todo ？？？？
    smart_str_appends(&sql, " UPDATE ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_appends(&sql, " SET ");
    smart_str_append(&sql, Z_STR(str_set));
    smart_str_appends(&sql, " WHERE ");



    if(Z_TYPE(_pkcol)==IS_ARRAY){
        zval *col,wheres;
        array_init(&wheres);
        ZEND_HASH_FOREACH_VAL(Z_ARR(_pkcol),col) {

            zval pkcol;
            zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,col);
            convert_to_string(&pkcol);


            zval pk;
            zval *_pk=zend_hash_find(Z_ARR_P(data),Z_STR_P(col));
            if(_pk){
                zval type;
                ZVAL_NULL(&type);
                zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&_pkcol);
                zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&pk,_pk,&type);
                zval_ptr_dtor(&type);
            }else ZVAL_EMPTY_STRING(&pk);


            smart_str _where = {0};
            smart_str_append(&_where, Z_STR(pkcol));
            smart_str_appends(&_where, " = ");
            smart_str_append(&_where, Z_STR(pk));
            smart_str_0(&_where);
            zval str;
            ZVAL_STR(&str,_where.s);
            smart_str_free(&_where);
            add_next_index_str(&wheres,Z_STR(str));
            zval_ptr_dtor(&pkcol);
            zval_ptr_dtor(&pk);
        } ZEND_HASH_FOREACH_END();
        zval str_where;
        zend_string *glue = zend_string_init(ZEND_STRL(" and "), 0);
        php_implode(glue, &wheres, &str_where);
        smart_str_append(&sql, Z_STR(str_where));

    }else{

        zval type,_pk,pk;
        ZVAL_NULL(&type);
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&_pkcol);
        zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"pk",&_pk);
        zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&pk,&_pk,&type);
        zval_ptr_dtor(&type);
        zval_ptr_dtor(&_pk);

        zval pkcol;
        zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,&_pkcol);
        convert_to_string(&pkcol);
        smart_str_append(&sql, Z_STR(pkcol));
        smart_str_appends(&sql, " = ");
        smart_str_append(&sql, Z_STR(pk));
        zval_ptr_dtor(&pkcol);
    }

    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR(&zsql,sql.s);
    zval status;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);

    zval_ptr_dtor(&status);
    zval_ptr_dtor(&str_set);
    zval_ptr_dtor(&zsql);


    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zval_ptr_dtor(&temp_array);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1);


    iterator_done:



    zval_ptr_dtor(&table_name);
    zval_ptr_dtor(&table);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&columns);
    zval_ptr_dtor(&_pkcol);


    RETURN_ZVAL(object,1,0);



}
ZEND_METHOD(lsentity_entity_class, create){
    zval *valid_object=NULL,*object;
    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 1, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();

    if (lsentity_check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot create %s model because it is already loaded",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }
    zval table,db;
    if(!get_table(object,&table))RETURN_NULL();
    if(!get_db(&table,&db)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval _table_name;
    zend_call_method_with_0_params(&table,Z_OBJCE(table),NULL,"tablename",&_table_name);
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
    if(Z_TYPE(_pkcol)!=IS_STRING||Z_TYPE(_pkcol)!=IS_ARRAY){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&pkcol);
        RETURN_NULL();
    }
    zval_ptr_dtor(&_pkcol);
    convert_to_string(&pkcol);
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
        if(lsentity_obj_check(lsentity_column_ce_ptr,val,0)){
            zval name;
            zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
            if(lsentity_obj_check(lsentity_column_save_ce_ptr,val,0)){
                zend_call_method_with_2_params(val,Z_OBJCE_P(val),NULL,"create",NULL,object,&name);
            }
            if(!zend_symtable_exists_ind(Z_ARR_P(data),Z_STR(name))&&lsentity_check_bool_with_0_params(val,"usedefault")){
                zval def;
                zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"getdefault",&def);
                Z_ADDREF(def);
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
    if(!zend_is_true(valid)|| valid_object){
        zval _valid_object;
        ZVAL_NULL(&_valid_object);
        if(!valid_object)valid_object=&_valid_object;
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
        add_next_index_str(&field,Z_STR(_field));
        zval _type;
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&_type,&key);
        zval _sdata;
        zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_sdata,dval,&_type);
        add_next_index_str(&field,Z_STR(_sdata));
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
    smart_str_appends(&sql, " ( ");
    smart_str_append(&sql, Z_STR(str_field));
    smart_str_appends(&sql, " ) VALUES (");
    smart_str_append(&sql, Z_STR(str_data));
    smart_str_appends(&sql, " )");
    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR(&zsql,sql.s);
    zval status;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);
    zval_ptr_dtor(&status);

    zval_ptr_dtor(&str_field);
    zval_ptr_dtor(&str_data);
    zval_ptr_dtor(&zsql);


    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zval_ptr_dtor(&temp_array);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),0);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),0);


    if(Z_TYPE(pkcol)==IS_STRING&&zend_symtable_exists_ind(Z_ARR_P(data),Z_STR(pkcol))){
        zval ind;
        zend_call_method_with_0_params(&db,Z_OBJCE(db),NULL,"insertid",&ind);
        Z_ADDREF(ind);
        zend_hash_update(Z_ARR_P(data),Z_STR(pkcol),&ind);
        zval_ptr_dtor(&ind);
    }

    iterator_done:

    zval_ptr_dtor(&table_name);
    zval_ptr_dtor(&table);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&columns);
    zval_ptr_dtor(&pkcol);

    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_entity_class, delete){
    zval *object=getThis();
    if (!lsentity_check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot delete %s model because it is not loaded.",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }
    zval table,db;
    if(!get_table(object,&table))RETURN_NULL();
    if(!get_db(&table,&db)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval _table_name;
    zend_call_method_with_0_params(&table,Z_OBJCE(table),NULL,"tablename",&_table_name);
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
    zval _pkcol;
    if(!get_table_pk(&table,&_pkcol)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }
    zval columns;
    if(!get_table_columns(&table,&columns)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        zval_ptr_dtor(&_pkcol);
        RETURN_NULL();
    }


    zval_ptr_dtor(&table);
    convert_to_string(&table_name);

    smart_str sql = {0};

    smart_str_appends(&sql, " DELETE FROM ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_appends(&sql, " WHERE ");


    if(Z_TYPE(_pkcol)==IS_ARRAY){
        zval *col,wheres;
        array_init(&wheres);
        ZEND_HASH_FOREACH_VAL(Z_ARR(_pkcol),col) {
                    zval pkcol;
                    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,col);
                    convert_to_string(&pkcol);


                    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
                    zval pk;
                    zval *_pk=zend_hash_find(Z_ARR_P(data),Z_STR_P(col));
                    if(_pk){
                        zval type;
                        ZVAL_NULL(&type);
                        zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&_pkcol);
                        zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&pk,_pk,&type);
                        zval_ptr_dtor(&type);
                    }else ZVAL_EMPTY_STRING(&pk);
                    convert_to_string(&pk);


                    smart_str _where = {0};
                    smart_str_append(&_where, Z_STR(pkcol));
                    smart_str_appends(&_where, " = ");
                    smart_str_append(&_where, Z_STR(pk));
                    smart_str_0(&_where);
                    zval str;
                    ZVAL_STR(&str,_where.s);
                    smart_str_free(&_where);
                    add_next_index_str(&wheres,Z_STR(str));
                    zval_ptr_dtor(&pkcol);
                    zval_ptr_dtor(&pk);
                } ZEND_HASH_FOREACH_END();
        zval str_where;
        zend_string *glue = zend_string_init(ZEND_STRL(" and "), 0);
        php_implode(glue, &wheres, &str_where);
        smart_str_append(&sql, Z_STR(str_where));

    }else{

        zval type,_pk,pk;
        ZVAL_NULL(&type);
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&_pkcol);
        zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"pk",&_pk);
        zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&pk,&_pk,&type);
        zval_ptr_dtor(&type);
        zval_ptr_dtor(&_pk);
        convert_to_string(&pk);


        zval pkcol;
        zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,&_pkcol);
        convert_to_string(&pkcol);
        smart_str_append(&sql, Z_STR(pkcol));
        smart_str_appends(&sql, " = ");
        smart_str_append(&sql, Z_STR(pk));
        zval_ptr_dtor(&pkcol);
        zval_ptr_dtor(&pk);
    }

    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR(&zsql,sql.s);

    zval status;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);
    zval_ptr_dtor(&status);

    zval_ptr_dtor(&_pkcol);
    zval_ptr_dtor(&table_name);
    zval_ptr_dtor(&zsql);
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"clear",return_value);
}
ZEND_METHOD(lsentity_entity_class, values){
    zval *values,*expected;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_ARRAY(values)
            Z_PARAM_OPTIONAL
            Z_PARAM_ARRAY(expected)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object=getThis();
    zval arr;
    array_init(&arr);
    if (zend_is_true(expected)) {
        php_array_merge(Z_ARR(arr), Z_ARR_P(values));
    }else{
        zval columns;
        if(get_columns(object,&columns,1)){
            zval param1,defarr;
            ZVAL_LONG(&param1,LSENTITY_COLUMN_SET_TYPE_FIELD);
            zend_call_method_with_1_params(&columns,Z_OBJCE(columns), NULL, "asarray", &defarr,&param1);
            php_array_merge(Z_ARR(arr),Z_ARR(defarr));
            zval_ptr_dtor(&defarr);
            zval_ptr_dtor(&columns);
        }
        zval table,pk;
        if(get_table(object,&table)){
            if(get_table_pk(&table,&pk)){
                if(Z_TYPE(pk)==IS_ARRAY){
                    zval *col;
                    ZEND_HASH_FOREACH_VAL(Z_ARR(pk),col) {
                        zval* oldval=zend_hash_find(Z_ARR_P(values),Z_STR_P(col));
                        if(oldval)zend_hash_del(Z_ARR_P(values),Z_STR_P(col));
                    } ZEND_HASH_FOREACH_END();
                }else{
                    zend_hash_del(Z_ARR(arr),Z_STR(pk));
                }
                zval_ptr_dtor(&pk);
            }
            zval_ptr_dtor(&table);
        }
    }
    zval *col;
    ZEND_HASH_FOREACH_VAL(Z_ARR(arr),col) {
        zval* oldval=zend_hash_find(Z_ARR_P(values),Z_STR_P(col));
        if(oldval)zend_call_method_with_2_params(object,Z_OBJCE_P(object),&Z_OBJCE_P(object)->__set, "__set", NULL,col,oldval);
    } ZEND_HASH_FOREACH_END();
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_entity_class, save){
    zval *valid_object=NULL,*object;
    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 1, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();
    zval _valid_object;
    ZVAL_NULL(&_valid_object);
    if(!valid_object)valid_object=&_valid_object;
    if (!lsentity_check_bool_with_0_params(object,"loaded")){
        zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "create", NULL,valid_object);
    }else{
        zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "update", NULL,valid_object);
    }
    RETURN_ZVAL(object,1,0);
}
ZEND_METHOD(lsentity_entity_class, check){
    zval *valid_object=NULL,*object;
    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 1, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();

    zval valid_object_obj;
    zval *valid_ok=NULL;
    if(!valid_object){
        if(get_validation(object,&valid_object_obj,0)) valid_ok=&valid_object_obj;
    }else valid_ok=valid_object;

    if(!valid_ok||Z_TYPE_P(valid_ok)!=IS_OBJECT||!zend_object_is_true(valid_ok)){
        if(Z_TYPE(valid_object_obj)==IS_OBJECT&&zend_object_is_true(&valid_object_obj))zval_ptr_dtor(&valid_object_obj);
        RETURN_ZVAL(object,1,0);
    }

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval arr;
    array_init(&arr);
    php_array_merge(Z_ARR(arr),Z_ARR_P(data));
    zval columns;
    if(get_columns(object,&columns,1)){
        zval param1,defarr;
        ZVAL_LONG(&param1,LSENTITY_COLUMN_SET_TYPE_DEFAULT);
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns), NULL, "asarray", &defarr,&param1);
        php_array_merge(Z_ARR(arr),Z_ARR(defarr));
        zval_ptr_dtor(&defarr);
        zval_ptr_dtor(&columns);
    }

    zval valid,param;
    ZVAL_LONG(&param,1);
    zend_call_method_with_1_params(valid_ok,Z_OBJCE_P(valid_ok), NULL, "valid", &valid,&arr);
    zend_bool is_valid=zend_is_true(&valid);
    zval_ptr_dtor(&valid);
    if(Z_TYPE(valid_object_obj)==IS_OBJECT&&zend_object_is_true(&valid_object_obj))zval_ptr_dtor(&valid_object_obj);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),is_valid);

    if(is_valid)RETURN_ZVAL(object,1,0);

    zend_object *obj=zend_throw_exception_ex(lsentity_exception_ce_ptr, 2, "validation data fail");

    zval errdata;
    zend_call_method_with_0_params(valid_ok,Z_OBJCE_P(valid_ok),NULL,"errors",&errdata);
    if(Z_TYPE(errdata)==IS_ARRAY){
        zval throw;
        ZVAL_OBJ(&throw,obj);
        zend_call_method_with_1_params(&throw,Z_OBJCE(throw),NULL,"setvalidationerror",NULL,&errdata);
    }
    zval_ptr_dtor(&errdata);
    RETURN_ZVAL(object,1,0);
}

ZEND_METHOD(lsentity_entity_class, asArray){
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval columns;
    if(get_columns(object,&columns,1)){
        zval param1,defarr;
        ZVAL_LONG(&param1,LSENTITY_COLUMN_SET_TYPE_DEFAULT);
        zend_call_method_with_1_params(&columns,Z_OBJCE(columns), NULL, "asarray", &defarr,&param1);
        zval arr;
        array_init(&arr);
        php_array_merge(Z_ARR(arr),Z_ARR_P(data));
        php_array_merge(Z_ARR(arr),Z_ARR(defarr));
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
    ZEND_ME(lsentity_entity_class,__unset, lsentity_entity_unset_arginfo, ZEND_ACC_PUBLIC)
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
    ZEND_FE_END
};


void lsentity_entity_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LS_NS,"Entity",lsentity_entity_class_method);
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

