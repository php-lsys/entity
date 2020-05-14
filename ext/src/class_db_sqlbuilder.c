


#include "zend_smart_str.h"
#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "zend_types.h"
#include "zend_interfaces.h"
#include "zend_inheritance.h"
#include "zend_exceptions.h"
#include "ext/standard/php_string.h"

#include "entity.h"
#include "class_db.h"
#include "class_table.h"
#include "class_db_sqlbuilder.h"
#include "class_db_sqlruner.h"
#include "utils.h"
#include "class_entity.h"
#include "class_entity_set.h"
#include "class_exception.h"
#include "class_column_set.h"

ZEND_BEGIN_ARG_INFO_EX(lsentity_db_sqlbuilder_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO_ENTITYNS(0, table, Table, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lsentity_db_sqlbuilder_update_arginfo, 0, 0, 2)
    ZEND_ARG_ARRAY_INFO(0,records,0)
    ZEND_ARG_INFO(0,where)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(lsentity_db_sqlbuilder_delete_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0,where)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(lsentity_db_sqlbuilder_insert_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0,records,0)
    ZEND_ARG_INFO(0,unique_replace)
ZEND_END_ARG_INFO()


void create_null_sqlruner(zval *db,zval *return_value){
    zval nullval;
    ZVAL_NULL(&nullval);
    zval param[]={
        *db,
        nullval
    };
    lsentity_new_class(lsentity_db_sqlruner_ce_ptr,return_value,param,2);
    zval_ptr_dtor(&nullval);
}


static int build_where(zval *table,zval* where,zval* where_string){
    int check=0;
    if(lsentity_obj_check(lsentity_entity_ce_ptr,where,0,0))check|=1;
    if(lsentity_obj_check(lsentity_entity_set_ce_ptr,where,0,0))check|=2;
    if(!check){
        ZVAL_DUP(where_string,where);
        convert_to_string(where_string);
        return 1;
    }
    zval columns;
    if(!lsentity_get_table_columns(table,&columns))return 0;
    zval pk;
    if(!lsentity_get_table_pk(table,&pk)){
        zval_ptr_dtor(&columns);
        return 0;
    }
    zval db;
    if(!lsentity_get_db(table,&db)){
        zval_ptr_dtor(&pk);
        zval_ptr_dtor(&columns);
        return 0;
    }
    int status=1;
    if(check&1){
        if(Z_TYPE(pk)==IS_ARRAY){
            zval *col,wheres;
            array_init(&wheres);
            ZEND_HASH_FOREACH_VAL(Z_ARR(pk),col) {

                zval pkval;
                zend_call_method_with_1_params(where,Z_OBJCE_P(where),NULL,"__get",&pkval,col);

                zval pkcol;
                zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,col);
                convert_to_string(&pkcol);


                zval val,type;
                ZVAL_NULL(&type);
                zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,col);
                zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&val,&pkval,&type);
                zval_ptr_dtor(&type);


                smart_str _where = {0};
                smart_str_append(&_where, Z_STR(pkcol));
                smart_str_appends(&_where, " = ");
                smart_str_append(&_where, Z_STR(val));
                smart_str_0(&_where);
                zval str;
                ZVAL_STR_COPY(&str,_where.s);
                smart_str_free(&_where);


                Z_REFCOUNTED(str)&&Z_ADDREF(str);
                zend_hash_next_index_insert(Z_ARR(wheres),&str);



                zval_ptr_dtor(&val);
                zval_ptr_dtor(&pkval);
                zval_ptr_dtor(&pkcol);
                zval_ptr_dtor(&str);

            } ZEND_HASH_FOREACH_END();


            if(zend_array_count(Z_ARR(wheres))==0){
                zval_ptr_dtor(&wheres);
                status=0;
                goto iterator_done;
            }

            zend_string *glue = zend_string_init(ZEND_STRL(" and "), 0);
            php_implode(glue, &wheres, where_string);
            zend_string_release(glue);
            zval_ptr_dtor(&wheres);

        }else{


            zval pkval;
            zend_call_method_with_1_params(where,Z_OBJCE_P(where),NULL,"__get",&pkval,&pk);
            zval pkcol;
            zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,&pk);

            if(!zend_is_true(&pkcol)){
                zval_ptr_dtor(&pkcol);
                zval_ptr_dtor(&pkval);
                status=0;
                goto iterator_done;
            }

            convert_to_string(&pkcol);

            zval val,type;
            ZVAL_NULL(&type);
            zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&pk);
            zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&val,&pkval,&type);
            zval_ptr_dtor(&type);
            zval_ptr_dtor(&pkval);

            smart_str _where = {0};
            smart_str_append(&_where, Z_STR(pkcol));
            smart_str_appends(&_where, " = ");
            smart_str_append(&_where, Z_STR(val));
            smart_str_0(&_where);


            zval_ptr_dtor(&pkcol);
            zval_ptr_dtor(&val);


            ZVAL_STR_COPY(where_string,_where.s);
            smart_str_free(&_where);
        }
    }else{
        if(Z_TYPE(pk)==IS_ARRAY){

            zend_object_iterator   *iter;
            zend_class_entry       *ce = Z_OBJCE_P(where);
            zval                   *wval;
            iter = ce->get_iterator(ce, where, 0);
            if (EG(exception)) {
                goto iterator_done;
            }
            if (iter->funcs->rewind) {
                iter->funcs->rewind(iter);
                if (EG(exception)) {
                    status=0;
                    goto iterator_done;
                }
            }
            zval *col,wheres;
            array_init(&wheres);
            while (iter->funcs->valid(iter) == SUCCESS) {
                if (EG(exception)) {
                    status=0;
                    zval_ptr_dtor(&wheres);
                    goto iterator_done;
                }
                wval = iter->funcs->get_current_data(iter);
                if (EG(exception)) {
                    status=0;
                    zval_ptr_dtor(&wheres);
                    goto iterator_done;
                }


                zval wdata;
                ZEND_HASH_FOREACH_VAL(Z_ARR(pk),col) {

                            zval pkval;
                            zend_call_method_with_1_params(wval,Z_OBJCE_P(wval),NULL,"__get",&pkval,col);

                            zval pkcol;
                            zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,col);
                            convert_to_string(&pkcol);


                            zval val,type;
                            ZVAL_NULL(&type);
                            zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,col);
                            zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&val,&pkval,&type);
                            zval_ptr_dtor(&type);


                            smart_str _where = {0};
                            smart_str_append(&_where, Z_STR(pkcol));
                            smart_str_appends(&_where, " = ");
                            smart_str_append(&_where, Z_STR(val));
                            smart_str_0(&_where);
                            zval str;
                            ZVAL_STR_COPY(&str,_where.s);
                            smart_str_free(&_where);


                            Z_REFCOUNTED(str)&&Z_ADDREF(str);
                            zend_hash_next_index_insert(Z_ARR(wdata),&str);



                            zval_ptr_dtor(&val);
                            zval_ptr_dtor(&pkval);
                            zval_ptr_dtor(&pkcol);
                            zval_ptr_dtor(&str);

                        } ZEND_HASH_FOREACH_END();
                    zval wtmp;
                    zend_string *glue = zend_string_init(ZEND_STRL(" and "), 0);
                    php_implode(glue, &wdata, &wtmp);
                    zend_string_release(glue);
                    Z_REFCOUNTED(wtmp)&&Z_ADDREF(wtmp);
                    zend_hash_next_index_insert(Z_ARR(wheres),&wtmp);

                    iter->funcs->move_forward(iter);
                    if (EG(exception)) {
                        status=0;
                        zval_ptr_dtor(&wheres);
                        goto iterator_done;
                    }
                }
                zend_iterator_dtor(iter);


                if(zend_array_count(Z_ARR(wheres))==0){
                    zval_ptr_dtor(&wheres);
                    status=0;
                    goto iterator_done;
                }

                zval watmp;
                zend_string *glue = zend_string_init(ZEND_STRL(" ) or ( "), 0);
                php_implode(glue, &wheres, &watmp);
                zend_string_release(glue);
                zval_ptr_dtor(&watmp);
                zval_ptr_dtor(&wheres);

                smart_str _where = {0};
                smart_str_appends(&_where, " (( ");
                smart_str_appends(&_where, Z_STRVAL(watmp));
                smart_str_appends(&_where, " )) ");
                smart_str_0(&_where);
                ZVAL_STR_COPY(where_string,_where.s);
                smart_str_free(&_where);

            }else{

                zend_object_iterator   *iter;
                zend_class_entry       *ce = Z_OBJCE_P(where);
                zval                   *wval;
                iter = ce->get_iterator(ce, where, 0);
                if (EG(exception)) {
                    goto iterator_done;
                }
                if (iter->funcs->rewind) {
                    iter->funcs->rewind(iter);
                    if (EG(exception)) {
                        status=0;
                        goto iterator_done;
                    }
                }
                zval *col,wheres;
                array_init(&wheres);
                while (iter->funcs->valid(iter) == SUCCESS) {
                    if (EG(exception)) {
                        status=0;
                        zval_ptr_dtor(&wheres);
                        goto iterator_done;
                    }
                    wval = iter->funcs->get_current_data(iter);
                    if (EG(exception)) {
                        status=0;
                        zval_ptr_dtor(&wheres);
                        goto iterator_done;
                    }


                    zval pkval;
                    zend_call_method_with_1_params(wval,Z_OBJCE_P(wval),NULL,"__get",&pkval,&pk);

                    zval pkcol;
                    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&pkcol,&pk);
                    convert_to_string(&pkcol);


                    zval val,type;
                    ZVAL_NULL(&type);
                    zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&type,&pk);
                    zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&val,&pkval,&type);
                    zval_ptr_dtor(&type);


                    smart_str _where = {0};
                    smart_str_append(&_where, Z_STR(pkcol));
                    smart_str_appends(&_where, " = ");
                    smart_str_append(&_where, Z_STR(val));
                    smart_str_0(&_where);
                    zval str;
                    ZVAL_STR_COPY(&str,_where.s);
                    smart_str_free(&_where);


                    Z_REFCOUNTED(str)&&Z_ADDREF(str);
                    zend_hash_next_index_insert(Z_ARR(wheres),&str);

                    zval_ptr_dtor(&val);
                    zval_ptr_dtor(&pkval);
                    zval_ptr_dtor(&pkcol);
                    zval_ptr_dtor(&str);

                    iter->funcs->move_forward(iter);
                    if (EG(exception)) {
                        status=0;
                        zval_ptr_dtor(&wheres);
                        goto iterator_done;
                    }
                }
                zend_iterator_dtor(iter);


                if(zend_array_count(Z_ARR(wheres))==0){
                    zval_ptr_dtor(&wheres);
                    status=0;
                    goto iterator_done;
                }

                zval watmp;
                zend_string *glue = zend_string_init(ZEND_STRL(" or "), 0);
                php_implode(glue, &wheres, &watmp);
                zend_string_release(glue);
                zval_ptr_dtor(&watmp);
                zval_ptr_dtor(&wheres);



                smart_str _where = {0};
                smart_str_appends(&_where, " ( ");
                smart_str_appends(&_where, Z_STRVAL(watmp));
                smart_str_appends(&_where, " ) ");
                smart_str_0(&_where);
                ZVAL_STR_COPY(where_string,_where.s);
                smart_str_free(&_where);

            }
    }

    iterator_done:
    zval_ptr_dtor(&columns);
    zval_ptr_dtor(&pk);
    zval_ptr_dtor(&db);
    return status;
}



ZEND_API zend_class_entry *lsentity_db_sqlbuilder_ce_ptr;


ZEND_METHOD(lsentity_db_sqlbuilder_class, __construct){
    zval *table_object = NULL, *object;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_OBJECT_OF_CLASS_EX(table_object, lsentity_table_ce_ptr, 0, 0)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    if (table_object){
        zend_update_property(Z_OBJCE_P(object), object, ZEND_STRL("_table"), table_object);
    }
}
ZEND_METHOD(lsentity_db_sqlbuilder_class, update){
    zval *records = NULL;
    zval *where = NULL;
    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_ARRAY(records)
            Z_PARAM_ZVAL(where)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *object=getThis();
    zval *table = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),1,NULL);

    zval db;
    if(!lsentity_get_db(table,&db)){
        RETURN_FALSE;
    }

    if(zend_array_count(Z_ARR_P(records))==0){
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }

    zval strwhere;
    if(!build_where(table,where,&strwhere)){
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }

    if (EG(exception)) {
        zval_ptr_dtor(&db);
        return ;
    }

    zval _table_name;
    zend_call_method_with_0_params(table,Z_OBJCE_P(table),NULL,"tablename",&_table_name);

    if(Z_TYPE(_table_name)!=IS_STRING){
        zval_ptr_dtor(&strwhere);
        zval_ptr_dtor(&_table_name);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }

    zval table_name;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotetable",&table_name,&_table_name);
    zval_ptr_dtor(&_table_name);
    if(Z_TYPE(table_name)!=IS_STRING){
        zval_ptr_dtor(&strwhere);
        zval_ptr_dtor(&table_name);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }


    zval columns;
    if(!lsentity_get_table_columns(table,&columns)){
        zval_ptr_dtor(&strwhere);
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&columns);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }


    zval sets;
    array_init(&sets);
    zval *dval;
    zend_string *dkey;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(records),dkey,dval) {
                zval _field;
                zval key;
                ZVAL_STR_COPY(&key,dkey);
                zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&_field,&key);
                zval _type;
                zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&_type,&key);
                zval _sdata;
                zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&_sdata,dval,&_type);
                smart_str set = {0};
                smart_str_append(&set, Z_STR(key));
                smart_str_appends(&set, " = ");
                smart_str_append(&set, Z_STR(_sdata));
                smart_str_0(&set);
                zval str;
                ZVAL_STR_COPY(&str,set.s);
                smart_str_free(&set);
                Z_REFCOUNTED(str)&&Z_ADDREF(str);
                add_next_index_str(&sets,Z_STR(str));
                zval_ptr_dtor(&str);
                zval_ptr_dtor(&_type);
                zval_ptr_dtor(&_field);
                zval_ptr_dtor(&key);
                zval_ptr_dtor(&_sdata);
            } ZEND_HASH_FOREACH_END();
    zval str_set;
    zend_string *glue = zend_string_init(ZEND_STRL(","), 0);
    php_implode(glue, &sets, &str_set);
    zval_ptr_dtor(&sets);
    zend_string_release(glue);




    smart_str sql = {0};
    smart_str_appends(&sql, " UPDATE ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_appends(&sql, " SET ");
    smart_str_append(&sql, Z_STR(str_set));
    smart_str_appends(&sql, " WHERE ");
    smart_str_append(&sql, Z_STR(strwhere));


    smart_str_0(&sql);

    zval zsql;
    ZVAL_STR_COPY(&zsql,sql.s);
    //zval status;
    //zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);

    zval_ptr_dtor(&str_set);
    //zval_ptr_dtor(&zsql);

    zval_ptr_dtor(&strwhere);
    zval_ptr_dtor(&table_name);

    zval_ptr_dtor(&columns);

    zval param[]={
        db,
        zsql
    };
    lsentity_new_class(lsentity_db_sqlruner_ce_ptr,return_value,param,2);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&zsql);

   // RETURN_ZVAL(&zsql,1,1);

}
ZEND_METHOD(lsentity_db_sqlbuilder_class, delete){
    zval *where = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(where)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);



    zval *object=getThis();
    zval *table = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),1,NULL);

    zval db;
    if(!lsentity_get_db(table,&db)){
        RETURN_FALSE;
    }

    zval strwhere;

    if(!build_where(table,where,&strwhere)){
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }
    if (EG(exception)) {
        zval_ptr_dtor(&db);
        return ;
    }


    zval _table_name;
    zend_call_method_with_0_params(table,Z_OBJCE_P(table),NULL,"tablename",&_table_name);

    if(Z_TYPE(_table_name)!=IS_STRING){
        zval_ptr_dtor(&strwhere);
        zval_ptr_dtor(&_table_name);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }

    zval table_name;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotetable",&table_name,&_table_name);
    zval_ptr_dtor(&_table_name);
    if(Z_TYPE(table_name)!=IS_STRING){
        zval_ptr_dtor(&strwhere);
        zval_ptr_dtor(&table_name);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }


    smart_str sql = {0};

    smart_str_appends(&sql, " DELETE FROM ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_appends(&sql, " WHERE ");

    smart_str_append(&sql, Z_STR(strwhere));


    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR_COPY(&zsql,sql.s);
    smart_str_free(&sql);
    //zval status;
    //zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);


    zval_ptr_dtor(&table_name);
    //zval_ptr_dtor(&zsql);


    zval param[]={
        db,
        zsql
    };
    lsentity_new_class(lsentity_db_sqlruner_ce_ptr,return_value,param,2);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&zsql);


  //  RETURN_ZVAL(&zsql,1,1);

}
ZEND_METHOD(lsentity_db_sqlbuilder_class, insert){
    zval *records = NULL;
    zend_bool unique_replace=0;
    ZEND_PARSE_PARAMETERS_START(1, 2)
            Z_PARAM_ARRAY(records)
            Z_PARAM_OPTIONAL
            Z_PARAM_BOOL(unique_replace)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);



    zval *object=getThis();
    zval *table = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),1,NULL);

    zval db;
    if(!lsentity_get_db(table,&db)){
        RETURN_FALSE;
    }

    zval _table_name;
    zend_call_method_with_0_params(table,Z_OBJCE_P(table),NULL,"tablename",&_table_name);

    if(Z_TYPE(_table_name)!=IS_STRING){
        zval_ptr_dtor(&_table_name);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }

    zval table_name;
    zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotetable",&table_name,&_table_name);

    if(Z_TYPE(table_name)!=IS_STRING){
        zval_ptr_dtor(&table_name);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }

    zval columns;
    if(!lsentity_get_table_columns(table,&columns)){
        zval_ptr_dtor(&table_name);
        zval_ptr_dtor(&_table_name);
        zval_ptr_dtor(&columns);
        create_null_sqlruner(&db,return_value);
        zval_ptr_dtor(&db);
        return ;
    }



    zval field,keys;
    array_init(&keys);
    array_init(&field);
    zval *dval;



    ZEND_HASH_FOREACH_VAL(Z_ARR_P(records),dval) {
        if(Z_TYPE_P(dval)!=IS_ARRAY)continue;
        zend_string *kval;
        ZEND_HASH_FOREACH_STR_KEY(Z_ARR_P(dval),kval){
            zval find;
            zval ktmp;
            ZVAL_STR_COPY(&ktmp,kval);
            zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"offsetget",&find,&ktmp);

            if(zend_is_true(&find)&&zend_object_is_true(&find)&&!lsentity_in_array(&keys,&find)){

                zend_hash_next_index_insert(Z_ARR(keys),&ktmp);
                zval qtmp;

                zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"quotecolumn",&qtmp,&find);

                if(Z_TYPE(qtmp)!=IS_STRING||EG(exception)){
                    zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "obj %s quoteColumn method return not a string,param[%s]",ZSTR_VAL(Z_OBJCE(db)->name),ZSTR_VAL(Z_STR(qtmp)));
                    zval_ptr_dtor(&find);
                    zval_ptr_dtor(&qtmp);
                    zval_ptr_dtor(&table_name);
                    zval_ptr_dtor(&_table_name);
                    zval_ptr_dtor(&columns);

                    zval_ptr_dtor(&db);
                    zval_ptr_dtor(&field);
                    zval_ptr_dtor(&keys);
                    RETURN_FALSE;
                }

                Z_REFCOUNTED(qtmp)&&Z_ADDREF(qtmp);
                zend_hash_next_index_insert(Z_ARR(field),&qtmp);
                zval_ptr_dtor(&find);
                zval_ptr_dtor(&qtmp);
                zval_ptr_dtor(&ktmp);

            }else{
                zval_ptr_dtor(&find);
                zval_ptr_dtor(&ktmp);
                zend_error(E_USER_NOTICE, "The %s property does not exist in the %s table:",ZSTR_VAL(kval),Z_STRVAL(_table_name));
            }
        }ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FOREACH_END();


    zval values;
    array_init(&values);
    zval *kval;
    ZEND_HASH_FOREACH_VAL(Z_ARR_P(records),dval) {
        zval _values;
        array_init(&_values);
        ZEND_HASH_FOREACH_VAL(Z_ARR(keys),kval) {
            zval *find=zend_hash_find(Z_ARR_P(dval),Z_STR_P(kval));
            zval *val;
            if(!find){
                zval tmp;
                zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"offsetget",&tmp,kval);
                if(!lsentity_obj_check(lsentity_column_set_ce_ptr,&tmp,1,1)){
                    goto error_end;
                }
                zval tmp1;
                zend_call_method_with_0_params(&tmp,Z_OBJCE(tmp),NULL,"getdefault",&tmp1);
                convert_to_string(&tmp1);
                val=&tmp1;
            }else val=find;
            zval _type;
            zend_call_method_with_1_params(&columns,Z_OBJCE(columns),NULL,"gettype",&_type,kval);
            zval _sdata;
            zend_call_method_with_2_params(&db,Z_OBJCE(db),NULL,"quotevalue",&_sdata,find,&_type);
            Z_REFCOUNTED(_sdata)&&Z_ADDREF(_sdata);
            zend_hash_next_index_insert(Z_ARR(_values),&_sdata);
            zval_ptr_dtor(&_sdata);
            zval_ptr_dtor(&_type);
            if(!find)zval_ptr_dtor(val);
            error_end:
            if(EG(exception)){
                zend_throw_exception_ex(lsentity_exception_ce_ptr, 1, "obj %s quoteValie is fail",ZSTR_VAL(Z_OBJCE(db)->name));
                zval_ptr_dtor(&values);
                zval_ptr_dtor(&_values);
                zval_ptr_dtor(&table_name);
                zval_ptr_dtor(&_table_name);
                zval_ptr_dtor(&columns);

                zval_ptr_dtor(&db);
                zval_ptr_dtor(&field);
                zval_ptr_dtor(&keys);
                RETURN_FALSE;
            }
        } ZEND_HASH_FOREACH_END();

        zval str_val;
        zend_string *glue = zend_string_init(ZEND_STRL(","), 0);
        php_implode(glue, &_values, &str_val);

        smart_str valstr = {0};
        smart_str_appends(&valstr, "(");
        smart_str_appends(&valstr, Z_STRVAL(str_val));
        smart_str_appends(&valstr, ")");
        smart_str_0(&valstr);

        zval vtmp;
        ZVAL_STR_COPY(&vtmp,valstr.s);
        Z_REFCOUNTED(vtmp)&&Z_ADDREF(vtmp);
        zend_hash_next_index_insert(Z_ARR(values),&vtmp);


        smart_str_free(&valstr);
        zend_string_release(glue);
        zval_ptr_dtor(&str_val);
        zval_ptr_dtor(&_values);
        zval_ptr_dtor(&vtmp);
    } ZEND_HASH_FOREACH_END();

    zval_ptr_dtor(&keys);


    zval str_field;
    zend_string *glue = zend_string_init(ZEND_STRL(","), 0);
    php_implode(glue, &field, &str_field);
    zval str_data;
    php_implode(glue, &values, &str_data);

    zend_string_release(glue);

    zval_ptr_dtor(&values);
    zval_ptr_dtor(&field);



    smart_str sql = {0};
    smart_str_appends(&sql, " INSERT INTO ");
    smart_str_append(&sql, Z_STR(table_name));
    smart_str_appends(&sql, " ( ");
    smart_str_append(&sql, Z_STR(str_field));
    smart_str_appends(&sql, " ) VALUES ");
    smart_str_append(&sql, Z_STR(str_data));
    smart_str_0(&sql);
    zval zsql;
    ZVAL_STR_COPY(&zsql,sql.s);
    //zval status;
    //zend_call_method_with_1_params(&db,Z_OBJCE(db),NULL,"exec",&status,&zsql);
    smart_str_free(&sql);

    zval_ptr_dtor(&table_name);
    zval_ptr_dtor(&_table_name);
    zval_ptr_dtor(&columns);




    zval param[]={
            db,
            zsql
    };
    lsentity_new_class(lsentity_db_sqlruner_ce_ptr,return_value,param,2);
    zval_ptr_dtor(&db);
    zval_ptr_dtor(&zsql);

    //zval_ptr_dtor(&db);
    //RETURN_ZVAL(&zsql,1,1);


}

ZEND_METHOD(lsentity_db_sqlbuilder_class, table){
    zval *object=getThis();
    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_table"),1,NULL);
    RETURN_ZVAL(valid,1,0);
}

static zend_function_entry lsentity_db_sqlbuilder_class_method[] = {
        ZEND_ME(lsentity_db_sqlbuilder_class,__construct, lsentity_db_sqlbuilder_construct_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_db_sqlbuilder_class,update, lsentity_db_sqlbuilder_update_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_db_sqlbuilder_class,table, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_db_sqlbuilder_class,delete, lsentity_db_sqlbuilder_delete_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_db_sqlbuilder_class,insert, lsentity_db_sqlbuilder_insert_arginfo, ZEND_ACC_PUBLIC)
        ZEND_FE_END
};




void lsentity_db_sqlbuilder_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_DB_NS,"SQLBuilder",lsentity_db_sqlbuilder_class_method);
    lsentity_db_sqlbuilder_ce_ptr = zend_register_internal_class(&ce);
    zend_declare_property_null(lsentity_db_sqlbuilder_ce_ptr,ZEND_STRL("_table"), ZEND_ACC_PRIVATE );
}