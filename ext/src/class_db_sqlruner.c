

#include "zend.h"
#include "php.h"
#include "zend_API.h"
#include "zend_types.h"
#include "zend_interfaces.h"
#include "zend_inheritance.h"
#include "ext/standard/php_string.h"

#include "entity.h"
#include "class_db.h"

ZEND_BEGIN_ARG_INFO_EX(lsentity_db_sqlruner_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO_ENTITYDBNS(0, db, Database, 0)
    ZEND_ARG_INFO(0, sql)
ZEND_END_ARG_INFO()

ZEND_API zend_class_entry *lsentity_db_runer_ce_ptr;

ZEND_METHOD(lsentity_db_sqlruner_class, __construct){
    zval *db;
    zend_string *sql;
    ZEND_PARSE_PARAMETERS_START(2,2)
    Z_PARAM_OBJECT_OF_CLASS(db, lsentity_db_ce_ptr)
    Z_PARAM_STR(sql)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    object = getThis();
    zend_update_property(lsentity_entity_set_ce_ptr,object,ZEND_STRL("_db"),db);
    zend_update_property_str(lsentity_entity_set_ce_ptr,object,ZEND_STRL("_sql"),sql);
}
ZEND_METHOD(lsentity_db_sqlruner_class, exec){
    zval *object=getThis();
    zval *sql = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_sql"),1,NULL);
    zval *db = zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_db"),1,NULL);
    if(!zend_is_true(sql)){
        RETURN_NULL();
    }
    zend_call_method_with_1_params(db,Z_OBJCE_P(db),NULL,"exec",NULL,sql);
}
ZEND_METHOD(lsentity_db_sqlruner_class, __toString){
    zval *object=getThis();
    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_sql"),1,NULL);
    RETURN_ZVAL(valid,1,0);
}
ZEND_METHOD(lsentity_db_sqlruner_class, toSql){
    zval *object=getThis();
    zval *valid=zend_read_property(Z_OBJCE_P(object),object,ZEND_STRL("_sql"),1,NULL);
    RETURN_ZVAL(valid,1,0);
}

static zend_function_entry lsentity_db_sqlruner_class_method[] = {
        ZEND_ME(lsentity_db_sqlruner_class,__construct, lsentity_db_sqlruner_construct_arginfo, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_db_sqlruner_class,toSql, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_db_sqlruner_class,__toString, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(lsentity_db_sqlruner_class,exec, NULL, ZEND_ACC_PUBLIC)
        ZEND_FE_END
};

void lsentity_db_sqlruner_class_init(){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,LSENTITY_DB_NS,"SQLRuner",lsentity_db_sqlruner_class_method);
    lsentity_db_sqlruner_ce_ptr = zend_register_internal_class(&ce);
    zend_declare_property_null(lsentity_db_sqlruner_ce_ptr,ZEND_STRL("_db"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(lsentity_db_sqlruner_ce_ptr,ZEND_STRL("_sql"), ZEND_ACC_PRIVATE);
}