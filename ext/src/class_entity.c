#include "zend.h"
#include "zend_hash.h"
#include "php.h"
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
#include "class_db_builder.h"

#ifdef HAVE_JSON
#include "ext/json/php_json.h"
#endif


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
    return lsentity_obj_check(lsentity_table_ce_ptr,table,1,1);
}
static int get_db_builder(zval *object,zval *db_builder){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "dbbuilder", db_builder);
    if(lsentity_obj_check(lsentity_db_builder_ce_ptr,db_builder,0,1))return 1;
    zend_throw_exception_ex(lsentity_db_builder_ce_ptr, 1, "return not a %s object of table::dbBuilder method",ZSTR_VAL(lsentity_db_builder_ce_ptr->name));
    return 0;
}
static int get_filter(zval *object,zval *filter){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "filter", filter);
    return lsentity_obj_check(lsentity_filter_ce_ptr,filter,0,1);
}
static int get_validation(zval *object,zval *validation,int throw){
    zend_call_method_with_0_params(object,Z_OBJCE_P(object), NULL, "validation", validation);
    return lsentity_obj_check(lsentity_validation_ce_ptr,validation,throw,1);
}
static int get_columns(zval *object,zval *columns,int patch){
    zval param;
    ZVAL_BOOL(&param,patch?1:0);
    zend_call_method_with_1_params(object,Z_OBJCE_P(object), NULL, "columns", columns,&param);
    if(lsentity_obj_check(lsentity_column_set_ce_ptr,columns,0,1)){
        return 1;
    }
    zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "return not a %s object of Entity::columns method",ZSTR_VAL(lsentity_column_set_ce_ptr->name));
    return 0;
}
static int get_column(zval *columns,zend_string *column_str,zval *column,int throw){
    zval param;
    ZVAL_STR_COPY(&param,column_str);
    zend_call_method_with_1_params(columns,Z_OBJCE_P(columns), NULL, "offsetget",column,&param);
    zval_ptr_dtor(&param);
    return lsentity_obj_check(lsentity_column_ce_ptr,column,throw,1);
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
    zval_ptr_dtor(&temp_array);
    array_init(&temp_array);
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
    if(!lsentity_get_table_pk(&table,&pk)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval zcolumn;
    ZVAL_STR_COPY(&zcolumn,column);
    if(comp_pkkey(&pk,&zcolumn)==0){
        zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"));
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
    zval *set_value;
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(column)
        Z_PARAM_ZVAL(set_value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *object=getThis();
    zval valuecopy;
    ZVAL_DUP(&valuecopy,set_value);
    zval* value=&valuecopy;

    zval columns;
    if(!get_columns(object,&columns,1)) RETURN_NULL();
    zval zval_column;
    ZVAL_STR_COPY(&zval_column,column);
    if(!lsentity_check_bool_with_1_params(&columns,"offsetexists",&zval_column)){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "the %s property does not exist", ZSTR_VAL(column));
        zval_ptr_dtor(value);
        RETURN_NULL();
    }

    zval filter,filterval;
    if(get_filter(object,&filter)){
        zend_call_method_with_2_params(&filter,Z_OBJCE(filter), NULL, "runfilter",&filterval,&zval_column,value);
        zval_ptr_dtor(&filter);
       zval_ptr_dtor(&valuecopy);
        value=&filterval;
    }

    zval_ptr_dtor(&zval_column);


    int loaded=lsentity_check_bool_with_0_params(object,"loaded");

    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval columnobj;
    if(!get_column(&columns,column,&columnobj,1)){
    	zval_ptr_dtor(&columns);
    	zval_ptr_dtor(value);
    	RETURN_NULL();
	}

    if (loaded){
        if (zend_symtable_exists_ind(Z_ARR_P(data), column)) {
            zval* oldval=zend_hash_find(Z_ARR_P(data),column);
            zval nullval;
            if(!oldval)ZVAL_NULL(&nullval);
            if (lsentity_check_bool_with_2_params(&columnobj,"compare",oldval?oldval:&nullval,value)){
                Z_REFCOUNTED_P(value)&&Z_ADDREF_P(value);
                zend_hash_update(Z_ARR_P(data),column,value);
                zval_ptr_dtor(&columns);
                zval_ptr_dtor(&columnobj);
				zval_ptr_dtor(value);
                RETURN_NULL();
            }
        }
    }


    zval *change=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),1,NULL);


    if (!zend_symtable_exists_ind(Z_ARR_P(change), column)) {
        zval* oldval=zend_hash_find(Z_ARR_P(data),column);
        zval nullval;
        if(oldval){
            ZVAL_DUP(&nullval,oldval);
            Z_REFCOUNTED(nullval)&&Z_ADDREF(nullval);
        }
        else ZVAL_NULL(&nullval);
        zend_hash_add(Z_ARR_P(change),column,&nullval);
        if(oldval)zval_ptr_dtor(&nullval);
    }



    zval table,pk;
    if(!get_table(object,&table)){
        zval_ptr_dtor(&columns);
        zval_ptr_dtor(&columnobj);
        zval_ptr_dtor(value);
        RETURN_NULL();
    }
    if(!lsentity_get_table_pk(&table,&pk)){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&columns);
        zval_ptr_dtor(&columnobj);
        zval_ptr_dtor(value);
        RETURN_NULL();
    }

    zval zcolumn;
    ZVAL_STR_COPY(&zcolumn,column);
    if(comp_pkkey(&pk,&zcolumn)==0){

        zval* oldval=zend_hash_find(Z_ARR_P(change),column);
        zval nullval;
        if(!oldval)ZVAL_NULL(&nullval);
        int is_change=!lsentity_check_bool_with_2_params(&columnobj,"compare",oldval?oldval:&nullval,value);
        if(Z_TYPE(pk)==IS_ARRAY){
            zval _pk;
            ZVAL_DUP(&_pk,&pk);
            zval *entry;
            zend_ulong num_idx;
            zend_string *str_idx;
            ZEND_HASH_FOREACH_KEY_VAL(Z_ARR(_pk), num_idx, str_idx, entry) {
                ZVAL_DEREF(entry);
                if (zend_string_equals(column, Z_STR_P(entry))) {
                    if (str_idx) {
                        zend_hash_del(Z_ARR(_pk),str_idx);
                    } else {
                        zend_hash_index_del(Z_ARR(_pk),num_idx);
                    }
                    break;
                }
            } ZEND_HASH_FOREACH_END();
            if(!is_change){
                zval *col;
                ZEND_HASH_FOREACH_VAL(Z_ARR(_pk),col) {
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
            zval_ptr_dtor(&_pk);
        }
        if(is_change){
            if(loaded) {
                zval * val;
                zend_string * key;
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data), key, val)
                {
                    if (!zend_symtable_exists_ind(Z_ARR_P(change), key)) {
                        //Z_ADDREF_P(val);
                        zval tmp;
                        ZVAL_DUP(&tmp,val);
                        Z_REFCOUNTED(tmp)&&Z_ADDREF(tmp);
                        zend_hash_update(Z_ARR_P(change), key, &tmp);
                        zval_ptr_dtor(&tmp);
                    }
                }ZEND_HASH_FOREACH_END();
                zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"),value);
            }
            zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),0);
        }else{
            zval *change_pk=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"),1,NULL);
            if(!loaded&&change_pk&&zend_is_true(change_pk)){
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
                zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"));
            }
        }
    }

    zval_ptr_dtor(&zcolumn);
    zval_ptr_dtor(&pk);
    zval_ptr_dtor(&table);
    zval_ptr_dtor(&columns);
    zval_ptr_dtor(&columnobj);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),0);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),0);


    Z_REFCOUNTED_P(value)&&Z_ADDREF_P(value);
    zend_hash_update(Z_ARR_P(data),column,value);
    zval_ptr_dtor(value);

    RETURN_NULL();

}
ZEND_METHOD(lsentity_entity_class, __get){
    zend_string *column;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(column)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval* oldval=zend_hash_find(Z_ARR_P(data),column);
    if(oldval){
        zval tmp;
        ZVAL_DUP(&tmp,oldval);
        RETURN_ZVAL(&tmp,1,1);
    }
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
        zval_ptr_dtor(&pk);
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
    zval * value=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_filter"),1,NULL);
    if(Z_TYPE_P(value)==IS_OBJECT&&zend_object_is_true(value)){
        RETURN_ZVAL(value,1,0);
    }
    zval obj;
    zend_call_method(object,Z_OBJCE_P(object), NULL,ZEND_STRL("filterfactory"), &obj, 0, NULL, NULL);
    if(!lsentity_obj_check(lsentity_filter_ce_ptr,&obj,0,1)){
        RETURN_NULL();
    }
    if(lsentity_check_bool_with_0_params(&obj,"allowcache")){
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_filter"),&obj);
    }
    RETURN_ZVAL(&obj,1,1);
}
ZEND_METHOD(lsentity_entity_class, validation){
    zval *object=getThis();
    zval * value=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_validation"),1,NULL);
    if(Z_TYPE_P(value)==IS_OBJECT&&zend_object_is_true(value)){
        RETURN_ZVAL(value,1,0);
    }
    zval obj;
    zend_call_method(object,Z_OBJCE_P(object), NULL,ZEND_STRL("validationfactory"), &obj, 0, NULL, NULL);
    if(!lsentity_obj_check(lsentity_validation_ce_ptr,&obj,0,1)){
        RETURN_NULL();
    }
    if(lsentity_check_bool_with_0_params(&obj,"allowcache")){
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_validation"),&obj);
    }
    RETURN_ZVAL(&obj,1,1);
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

    zval zdata;
    ZVAL_DUP(&zdata,data);
    data=&zdata;

    zval *object=getThis();
    if(entity_column_set){
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"),entity_column_set);
    }
    zval columns;
    if(!get_columns(object,&columns,1)) RETURN_NULL();
    zval *val;
    zend_string * key;

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(data),key,val) {
        if(!key)continue;
        zval obj;
        if(get_column(&columns,key,&obj,0)){
            zval valobj;
            zend_call_method_with_1_params(&obj,Z_OBJCE(obj), NULL, "read", &valobj,val);
            Z_REFCOUNTED(valobj)&&Z_ADDREF(valobj);
            zend_hash_update(Z_ARR_P(data),key,&valobj);
           zval_ptr_dtor(&valobj);
        }else{
            zend_hash_del(Z_ARR_P(data),key);
        }
        zval_ptr_dtor(&obj);
    } ZEND_HASH_FOREACH_END();

    zval_ptr_dtor(&columns);

    zval *sdata=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);

    zval mdata;
    zval tmp;
    ZVAL_DUP(&tmp,sdata);
    array_init(&mdata);
    php_array_merge(Z_ARR(mdata),Z_ARR(tmp));
    zval_ptr_dtor(&tmp);
    php_array_merge(Z_ARR(mdata),Z_ARR_P(data));

    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),&mdata);
    zval_ptr_dtor(&mdata);

    if (loaded){
        zval table,pk;
        if(get_table(object,&table)){
            if(lsentity_get_table_pk(&table,&pk)){
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

    zval_ptr_dtor(data);
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"));
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
ZEND_METHOD(lsentity_entity_class, createData){

    zval *object=getThis();
    if (lsentity_check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot create %s model because it is already loaded",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }
    zval table;
    if(!get_table(object,&table))RETURN_NULL();

    zval columns;
    if(!lsentity_get_table_columns(&table,&columns)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }

    zval save_data;
    array_init(&save_data);

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

    while (iter->funcs->valid(iter) == SUCCESS) {
        if (EG(exception)) {
            goto iterator_done;
        }
        val = iter->funcs->get_current_data(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
        if(lsentity_obj_check(lsentity_column_ce_ptr,val,0,0)){
            zval name;
            zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
            zval * find=zend_hash_find(Z_ARR_P(data),Z_STR(name));
            if(find){
                zval tmp;
                ZVAL_DUP(&tmp,find);
                Z_REFCOUNTED(tmp)&&Z_ADDREF(tmp);
                zend_hash_add(Z_ARR(save_data),Z_STR(name),&tmp);
                zval_ptr_dtor(&tmp);
            }
            zval_ptr_dtor(&name);
        }
        iter->funcs->move_forward(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
    }
    zend_iterator_dtor(iter);

    iterator_done:

    zval_ptr_dtor(&table);
    zval_ptr_dtor(&columns);

    RETURN_ZVAL(&save_data,1,1);

}
ZEND_METHOD(lsentity_entity_class, updateData){

    zval *object=getThis();
    object=getThis();

    if (!lsentity_check_bool_with_0_params(object,"loaded")){
        zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "Cannot update %s model because it is not loaded",ZSTR_VAL(Z_OBJCE_P(object)->name));
        RETURN_NULL();
    }

    zval table;
    if(!get_table(object,&table))RETURN_NULL();

    zval columns;
    if(!lsentity_get_table_columns(&table,&columns)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }

    zval save_data;
    array_init(&save_data);

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

    while (iter->funcs->valid(iter) == SUCCESS) {
        if (EG(exception)) {
            goto iterator_done;
        }
        val = iter->funcs->get_current_data(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
        if(lsentity_obj_check(lsentity_column_ce_ptr,val,0,0)){
            zval name;
            zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
            zval * find=zend_hash_find(Z_ARR_P(change),Z_STR(name));
            if(find){
                zval tmp;
                ZVAL_DUP(&tmp,find);
                Z_REFCOUNTED(tmp)&&Z_ADDREF(tmp);
                zend_hash_add(Z_ARR(save_data),Z_STR(name),&tmp);
                zval_ptr_dtor(&tmp);
            }
            zval_ptr_dtor(&name);
        }
        iter->funcs->move_forward(iter);
        if (EG(exception)) {
            goto iterator_done;
        }
    }
    zend_iterator_dtor(iter);
    iterator_done:

    zval_ptr_dtor(&table);
    zval_ptr_dtor(&columns);

    RETURN_ZVAL(&save_data,1,1);

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
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"));
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_columns"));
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_patch_columns"));
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"));
    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zval_ptr_dtor(&temp_array);
    array_init(&temp_array);
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
        if(lsentity_get_table_pk(&table,&pk)){
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
                    Z_REFCOUNTED_P(outp)&&Z_ADDREF_P(outp);
                }
            }
            zval_ptr_dtor(&pk);
        }
        zval_ptr_dtor(&table);
    }
    if(Z_TYPE(out)!=IS_NULL) RETURN_ZVAL(&out,1,1);
    if(outp) RETURN_ZVAL(outp,1,1);
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
    zval *columns;
    if(patch){
        columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_patch_columns"),1,NULL);
    }else{
        columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    }

    if (Z_TYPE_P(columns)!=IS_OBJECT||!zend_object_is_true(columns)){
        zval table;
        if(get_table(object,&table)){
            zval table_columns;
            if(lsentity_get_table_columns(&table,&table_columns)){
                zval *query_columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_query_column_set"),1,NULL);
                if(lsentity_obj_check(lsentity_entity_column_set_ce_ptr,query_columns,0,0)){
                    zval merge_table_columns,zpatch;
                    ZVAL_BOOL(&zpatch,patch);
                    zend_call_method_with_2_params(query_columns,Z_OBJCE_P(query_columns), NULL, "ascolumnset", &merge_table_columns,&table_columns,&zpatch);
                    if(lsentity_obj_check(lsentity_column_set_ce_ptr,&merge_table_columns,0,0)){

                        if(patch){
                            zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_patch_columns"),&merge_table_columns);
                        }else{
                            zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),&merge_table_columns);
                        }


                    }
                    zval_ptr_dtor(&merge_table_columns);
                }else{
                    if(patch){
                        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_patch_columns"),&table_columns);
                    }else{
                        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),&table_columns);
                    }

                }
                zval_ptr_dtor(&table_columns);
            }
            zval_ptr_dtor(&table);
        }
    }

    if(patch){
        columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_patch_columns"),1,NULL);
    }else{
        columns=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_columns"),1,NULL);
    }
    RETURN_ZVAL(columns,1,0);
}

ZEND_METHOD(lsentity_entity_class, saved){
    zval *val=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_saved"),0,NULL);
    zval tmp;
    ZVAL_DUP(&tmp,val);
    RETURN_ZVAL(&tmp,1,1);
}
ZEND_METHOD(lsentity_entity_class, loaded){
    zval *val=zend_read_property(Z_OBJCE_P(getThis()),getThis(),ZEND_STRL("_loaded"),0,NULL);
    zval tmp;
    ZVAL_DUP(&tmp,val);
    RETURN_ZVAL(&tmp,1,1);
}
ZEND_METHOD(lsentity_entity_class, update){
    zval *valid_object=NULL,*object;
    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 1, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();


    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),1,NULL);
    if(!zend_is_true(valid)|| valid_object){
        zval _valid_object;
        ZVAL_NULL(&_valid_object);
        if(!valid_object)valid_object=&_valid_object;
        zend_call_method_with_1_params(object,Z_OBJCE_P(object),NULL,"check",NULL,valid_object);
        if (EG(exception)){
            RETURN_NULL();
        }
    }
    zval csave_data;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"updatedata",&csave_data);
    if(Z_TYPE(csave_data)!=IS_ARRAY){
        zval_ptr_dtor(&csave_data);
        RETURN_NULL();
    }

    zval table,db,dbbuilder;
    if(!get_table(object,&table)){
        zval_ptr_dtor(&csave_data);
        RETURN_NULL();
    }

    if(!lsentity_get_db(&table,&db)){
        zval_ptr_dtor(&csave_data);
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    if(!get_db_builder(&table,&dbbuilder)){
        zval_ptr_dtor(&csave_data);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }


    if(zend_array_count(Z_ARR(csave_data))==0){
        zval temp_array;
        array_init(&temp_array);
        zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
        zval_ptr_dtor(&temp_array);
        zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"));
        zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),1);
        zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1);
        goto iterator_done;
    }


    zend_call_method_with_2_params(&dbbuilder,Z_OBJCE(dbbuilder),NULL,"update",NULL,&csave_data,object);



    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zval_ptr_dtor(&temp_array);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1);
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"));

    iterator_done:

    zval_ptr_dtor(&table);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&csave_data);


    RETURN_ZVAL(object,1,0);



}
ZEND_METHOD(lsentity_entity_class, create){
    zval *valid_object=NULL,*object;
    zend_bool  unique_replace=0;
    ZEND_PARSE_PARAMETERS_START(0, 2)
            Z_PARAM_OPTIONAL
            Z_PARAM_OBJECT_OF_CLASS_EX(valid_object, lsentity_validation_ce_ptr, 1, 0)
            Z_PARAM_BOOL(unique_replace)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object=getThis();
    zval *data=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_data"),1,NULL);
    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),1,NULL);
    if(!zend_is_true(valid)|| valid_object){
        zval _valid_object;
        ZVAL_NULL(&_valid_object);
        if(!valid_object)valid_object=&_valid_object;
        zend_call_method_with_1_params(object,Z_OBJCE_P(object),NULL,"check",NULL,valid_object);
        if (EG(exception)){
            RETURN_NULL();
        }
    }
    zval csave_data;
    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"createdata",&csave_data);
    if(Z_TYPE(csave_data)!=IS_ARRAY){
        zval_ptr_dtor(&csave_data);
        RETURN_NULL();
    }

    zval table,db,dbbuilder;
    if(!get_table(object,&table)){
        zval_ptr_dtor(&csave_data);
        RETURN_NULL();
    }
    if(!lsentity_get_db(&table,&db)){
        zval_ptr_dtor(&csave_data);
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    if(!get_db_builder(&table,&dbbuilder)){
        zval_ptr_dtor(&csave_data);
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }

    zval save_arr;
    array_init(&save_arr);
    Z_REFCOUNTED(csave_data)&&Z_ADDREF(csave_data);
    zend_hash_next_index_insert(Z_ARR(save_arr),&csave_data);
    zval zunique_replace;
    ZVAL_BOOL(&zunique_replace,unique_replace);
    zend_call_method_with_2_params(&dbbuilder,Z_OBJCE(dbbuilder),NULL,"insert",NULL,&save_arr,&zunique_replace);

    zval_ptr_dtor(&zunique_replace);
    zval_ptr_dtor(&save_arr);
    zval_ptr_dtor(&csave_data);




    zval temp_array;
    array_init(&temp_array);
    zend_update_property(Z_OBJCE_P(object),object,ZEND_STRL("_change"),&temp_array);
    zval_ptr_dtor(&temp_array);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_loaded"),1);
    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_saved"),1);
    zend_update_property_null(Z_OBJCE_P(object),object,ZEND_STRL("_change_pk"));


    zval pk;
    if(!lsentity_get_table_pk(&table,&pk)){
        goto end_done;
    }

    if(Z_TYPE(pk)==IS_STRING&&!zend_symtable_exists_ind(Z_ARR_P(data),Z_STR(pk))){
        zval ind;
        zend_call_method_with_0_params(&db,Z_OBJCE(db),NULL,"insertid",&ind);
        Z_REFCOUNTED(ind)&&Z_ADDREF(ind);
        zend_hash_update(Z_ARR_P(data),Z_STR(pk),&ind);
        zval_ptr_dtor(&ind);
    }

    end_done:

    zval_ptr_dtor(&table);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&dbbuilder);

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
    if(!lsentity_get_db(&table,&db)){
        zval_ptr_dtor(&table);
        RETURN_NULL();
    }
    zval dbbuilder;
    if(!get_db_builder(&table,&dbbuilder)){
        zval_ptr_dtor(&table);
        zval_ptr_dtor(&db);
        RETURN_NULL();
    }


    zend_call_method_with_1_params(&dbbuilder,Z_OBJCE(dbbuilder),NULL,"delete",NULL,object);



    zval_ptr_dtor(&dbbuilder);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&table);

    zend_call_method_with_0_params(object,Z_OBJCE_P(object),NULL,"clear",NULL);
    RETURN_ZVAL(object,1,0);

}
ZEND_METHOD(lsentity_entity_class, values){
    zval *set_values,*expected=NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_ARRAY(set_values)
            Z_PARAM_OPTIONAL
            Z_PARAM_ARRAY(expected)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    zval *object=getThis();
    zval arr;
    array_init(&arr);
    zval valuecopy;
    ZVAL_DUP(&valuecopy,set_values);
    zval *value=&valuecopy;
    if (expected&&Z_TYPE_P(expected)==IS_ARRAY) {
        php_array_merge(Z_ARR(arr), Z_ARR_P(value));
    }else{
        zval columns;
        if(get_columns(object,&columns,1)){
            zval param1,defarr;
            ZVAL_LONG(&param1,LSENTITY_COLUMN_SET_TYPE_FIELD);
            zend_call_method_with_1_params(&columns,Z_OBJCE(columns), NULL, "asarray", &defarr,&param1);
            php_array_merge(Z_ARR(arr),Z_ARR(defarr));
            zval_ptr_dtor(&defarr);
            zval_ptr_dtor(&param1);
            zval_ptr_dtor(&columns);
        }
        zval table,pk;
        if(get_table(object,&table)){
            if(lsentity_get_table_pk(&table,&pk)){
                if(Z_TYPE(pk)==IS_ARRAY){
                    zval *col;
                    ZEND_HASH_FOREACH_VAL(Z_ARR(pk),col) {
                        zval* oldval=zend_hash_find(Z_ARR_P(value),Z_STR_P(col));
                        if(oldval){
                            zend_hash_del(Z_ARR_P(value),Z_STR_P(col));
                        }
                    } ZEND_HASH_FOREACH_END();
                }else{
                    convert_to_string(&pk);
                    zend_hash_del(Z_ARR_P(value),Z_STR(pk));
                }
                zval_ptr_dtor(&pk);
            }
            zval_ptr_dtor(&table);
        }
    }
    zval *col;
    ZEND_HASH_FOREACH_VAL(Z_ARR(arr),col) {
        zval* val=zend_hash_find(Z_ARR_P(value),Z_STR_P(col));
        if(val){
            zend_call_method_with_2_params(object,Z_OBJCE_P(object),&Z_OBJCE_P(object)->__set, "__set", NULL,col,val);
        }
    } ZEND_HASH_FOREACH_END();
    zval_ptr_dtor(&arr);
    zval_ptr_dtor(value);
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
    zval table;
    if(!get_table(object,&table)){
        RETURN_NULL();
    }
    zval columns;
    if(!lsentity_get_table_columns(&table,&columns)){
        zval_ptr_dtor(&table);
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

    zval pk;
    if(!lsentity_get_table_pk(&table,&pk)){
        goto iterator_done;
    }

    if (!lsentity_check_bool_with_0_params(object,"loaded")){

        while (iter->funcs->valid(iter) == SUCCESS) {
            if (EG(exception)) {
                goto iterator_done;
            }
            val = iter->funcs->get_current_data(iter);
            if (EG(exception)) {
                goto iterator_done;
            }
            if(lsentity_obj_check(lsentity_column_ce_ptr,val,0,0)){
                zval name;
                zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
                if(lsentity_obj_check(lsentity_column_save_ce_ptr,val,0,0)){
                    zend_call_method_with_2_params(val,Z_OBJCE_P(val),NULL,"create",NULL,object,&name);
                }
                if(!zend_symtable_exists_ind(Z_ARR_P(data),Z_STR(name))&&lsentity_check_bool_with_0_params(val,"usedefault")){

                    if(comp_pkkey(&pk,val)!=0){
                        zval def;
                        zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"getdefault",&def);
                        Z_REFCOUNTED(def)&&Z_ADDREF(def);
                        zend_hash_update(Z_ARR_P(data),Z_STR(name),&def);
                        zval_ptr_dtor(&def);
                    }

                }
                zval_ptr_dtor(&name);
            }
            iter->funcs->move_forward(iter);
            if (EG(exception)) {
                goto iterator_done;
            }
        }

    }else{

        while (iter->funcs->valid(iter) == SUCCESS) {
            if (EG(exception)) {
                goto iterator_done;
            }
            val = iter->funcs->get_current_data(iter);
            if (EG(exception)) {
                goto iterator_done;
            }
            if(lsentity_obj_check(lsentity_column_ce_ptr,val,0,0)){
                zval name;
                zend_call_method_with_0_params(val,Z_OBJCE_P(val),NULL,"name",&name);
                if(lsentity_obj_check(lsentity_column_save_ce_ptr,val,0,0)){
                    zend_call_method_with_2_params(val,Z_OBJCE_P(val),NULL,"update",NULL,object,&name);
                }
                zval * dfind=zend_hash_find(Z_ARR_P(data),Z_STR(name));
                if(!lsentity_check_bool_with_0_params(val,"isallownull")
                   &&dfind
                   &&ZVAL_IS_NULL(dfind)){
                    zend_update_property_string(Z_OBJCE_P(object),object,ZEND_STRL("_data"),"");
                }
                zval_ptr_dtor(&name);
            }
            iter->funcs->move_forward(iter);
            if (EG(exception)) {
                goto iterator_done;
            }
        }

    }
    zend_iterator_dtor(iter);
    iterator_done:

    zval_ptr_dtor(&pk);
    zval_ptr_dtor(&table);
    zval_ptr_dtor(&columns);

    zval valid_object_obj;
    zval *valid_ok=NULL;
    if(!valid_object){
        if(get_validation(object,&valid_object_obj,0)) valid_ok=&valid_object_obj;
    }else valid_ok=valid_object;

    if(!valid_ok||Z_TYPE_P(valid_ok)!=IS_OBJECT||!zend_object_is_true(valid_ok)){
        if(Z_TYPE(valid_object_obj)==IS_OBJECT&&zend_object_is_true(&valid_object_obj))zval_ptr_dtor(&valid_object_obj);
        RETURN_ZVAL(object,1,0);
    }


    zval arr;
    array_init(&arr);
    zval tmp1;
    ZVAL_DUP(&tmp1,data);
    php_array_merge(Z_ARR(arr),Z_ARR(tmp1));
    zval_ptr_dtor(&tmp1);

    zval columns_patch;
    if(get_columns(object,&columns_patch,1)){
        zval param1,defarr;
        ZVAL_LONG(&param1,LSENTITY_COLUMN_SET_TYPE_DEFAULT);
        zend_call_method_with_1_params(&columns_patch,Z_OBJCE(columns_patch), NULL, "asarray", &defarr,&param1);
        php_array_merge(Z_ARR(defarr),Z_ARR(arr));
        zval_ptr_dtor(&defarr);
        zval_ptr_dtor(&columns_patch);
    }



    zval valid,param;
    ZVAL_LONG(&param,1);

    zend_call_method_with_1_params(valid_ok,Z_OBJCE_P(valid_ok), NULL, "valid", &valid,&arr);

    zend_bool is_valid=zend_is_true(&valid);
    zval_ptr_dtor(&valid);

    if(Z_TYPE(valid_object_obj)==IS_OBJECT&&zend_object_is_true(&valid_object_obj))zval_ptr_dtor(&valid_object_obj);

    zend_update_property_bool(Z_OBJCE_P(object),object,ZEND_STRL("_valid"),is_valid);


    zval_ptr_dtor(&arr);



    if(is_valid)RETURN_ZVAL(object,1,0);


    zval ex, tmp,tmpc;
    object_init_ex(&ex, lsentity_exception_ce_ptr);
    ZVAL_STRING(&tmp, "validation data fail");
    #if PHP_VERSION_ID<70200
        zend_update_property_ex(lsentity_exception_ce_ptr, &ex, CG(known_strings)[ZEND_STR_MESSAGE], &tmp);
    #else
        zend_update_property_ex(lsentity_exception_ce_ptr, &ex, ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);
    #endif

    zval_ptr_dtor(&tmp);
    ZVAL_LONG(&tmpc, 2);
    #if PHP_VERSION_ID<70200
    zend_update_property_ex(lsentity_exception_ce_ptr, &ex, CG(known_strings)[ZEND_STR_CODE], &tmpc);
    #else
    zend_update_property_ex(lsentity_exception_ce_ptr, &ex, ZSTR_KNOWN(ZEND_STR_CODE), &tmpc);
    #endif

    zval_ptr_dtor(&tmpc);
    zval errdata;
    zend_call_method_with_0_params(valid_ok,Z_OBJCE_P(valid_ok),NULL,"errors",&errdata);

    if(Z_TYPE(errdata)==IS_ARRAY){
        zend_call_method_with_1_params(&ex,Z_OBJCE(ex),NULL,"setvalidationerror",NULL,&errdata);
    }
    zend_throw_exception_internal(&ex);
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
        zval tmp;
        ZVAL_DUP(&tmp,data);
        php_array_merge(Z_ARR(arr),Z_ARR(tmp));
        zval_ptr_dtor(&tmp);
        zval *src_entry;
        zend_string *string_key;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR(defarr), string_key, src_entry) {
            if (UNEXPECTED(Z_ISREF_P(src_entry) &&
                           Z_REFCOUNT_P(src_entry) == 1)) {
                ZVAL_UNREF(src_entry);
            }
            Z_TRY_ADDREF_P(src_entry);
            if (string_key) {
                if(!zend_hash_find(Z_ARR(arr),string_key))
                zend_hash_update(Z_ARR(arr), string_key, src_entry);
            } else {
                zend_hash_next_index_insert_new(Z_ARR(arr), src_entry);
            }
        } ZEND_HASH_FOREACH_END();
        zval_ptr_dtor(&defarr);
        zval_ptr_dtor(&param1);
        zval_ptr_dtor(&columns);
        RETURN_ZVAL(&arr,1,1);
    }
    RETURN_ZVAL(data,1,0);
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
    ZEND_ME(lsentity_entity_class,updateData, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(lsentity_entity_class,createData, NULL, ZEND_ACC_PUBLIC)

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
    #ifdef HAVE_JSON
        zend_class_implements(lsentity_entity_ce_ptr,1, php_json_serializable_ce);
    #endif

    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_table"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_query_column_set"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_columns"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_patch_columns"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_change"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_data"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_validation"), ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_filter"), ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_entity_ce_ptr,ZEND_STRL("_loaded"),0,ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_entity_ce_ptr,ZEND_STRL("_valid"),0, ZEND_ACC_PROTECTED );
    zend_declare_property_bool(lsentity_entity_ce_ptr,ZEND_STRL("_saved"),0,ZEND_ACC_PROTECTED );
    zend_declare_property_null(lsentity_entity_ce_ptr,ZEND_STRL("_change_pk"),ZEND_ACC_PROTECTED);
}

