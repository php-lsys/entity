<?php
namespace LSYS\Entity\Database;
use LSYS\Entity;
use LSYS\Entity\Table;
use LSYS\Entity\EntitySet;
if (!class_exists(Builder::class)){
    class Builder{
        private $_table;
        public function __construct(Table $table) {
            $this->_table=$table;
        }
        /**
         * 得到TABLE对象
         * @return \LSYS\Entity\Table
         */
        public function table() {
            return $this->_table;
        }
        /**
         * 按条件更新
         * @param array $save_data
         * @param mixed $where
         * @return boolean
         */
        public function update(array $records,$where){
            if(count($records)==0)return true;
            $table=$this->table();
            $where=$this->buildWhere($where);
            if($where===false)return false;
            $db=$table->db();
            $table_name = $db->quoteTable( $table->tableName() );
            $table_column=$table->tableColumns();
            $sets = array ();
            foreach ( $records as $key => $value ) {
                $set = $db->quoteColumn($key) . " = " . $db->quoteValue( $value,$table_column->getType($key));
                array_push ( $sets, $set );
            }
            $str_set = implode ( ",", $sets );
            $sql = " UPDATE " . $table_name . " SET " . $str_set." WHERE " . $where;
            return $db->exec($sql);
        }
        /**
         * 按条件删除
         * @param mixed $where
         * @return boolean
         */
        public function delete($where){
            $table=$this->table();
            $where=$this->buildWhere($where);
            if($where===false)return false;
            $db=$table->db();
            $table_name=$db->quoteTable($table->tableName());
            $sql=" DELETE FROM ".$table_name." where ".$where;
            return $db->exec($sql);
        }
        /**
         * 插入结果集
         * @param array $records
         * @return boolean
         */
        public function insert(array $records,$unique_replace=false){
            $table=$this->table();
            $db=$table->db();
            $table_name=$db->quoteTable($table->tableName());
            $keys=array();
            $field=array();
            $table_column=$table->tableColumns();
            foreach ($records as $value)
            {
                if(!is_array($value))continue;
                foreach (array_keys($value) as $key){
                    if(isset($table_column[$key])){
                        if(!in_array($key, $keys)){
                            $keys[]=$key;
                            array_push($field, $db->quoteColumn($key));
                        }
                    }else{
                        trigger_error(
                            strtr("The :column property does not exist in the :table table:",[":column"=>$key,":table"=>$table_name]),
                            E_USER_NOTICE
                            );
                    }
                }
            }
            $values=array();
            foreach ($records as $value)
            {
                $_values=array();
                foreach ($keys as $key){
                    
                    if(array_key_exists($key, $value)){
                        $_values[]= $db->quoteValue($value[$key],$table_column->getType($key));
                    }else{
                        $_values[]=$db->quoteValue($table_column->offsetGet()->getDefault());
                    }
                }
                $values[]='('.implode(",", $_values).')';
            }
            $str_field=implode(",",$field);
            $str_data=implode(",", $values);
            $sql=" INSERT INTO ".$table_name." (".$str_field.")VALUES {$str_data}";
            return $db->exec( $sql );
        }
        /**
         * 编译where条件
         * @param mixed $where
         * @return string
         */
        private function buildWhere($where){
            $table=$this->table();
            if ($where instanceof Entity) {
                $columns=$table->tableColumns();
                $primary_key=$table->primaryKey();
                $db=$table->db();
                if (is_array($primary_key)){
                    $where_data = [];
                    foreach ($primary_key as $v){
                        $pk=$where->__get($v);
                        $where_data[] = $db->quoteColumn($v) . '=' . $db->quoteValue($pk,$columns->getType($v));
                    }
                    if(count($where_data)==0)return false;
                    $where_data = implode(" and ", $where_data);
                }else{
                    if(empty($primary_key))return false;
                    $pk=$where->__get($primary_key);
                    $where_data = $db->quoteColumn($primary_key) . '=' . $db->quoteValue($pk,$columns->getType($primary_key));
                }
                return $where_data;
            }else if ($where instanceof EntitySet) {
                $columns=$table->tableColumns();
                $primary_key=$table->primaryKey();
                $db=$table->db();
                if (is_array($primary_key)){
                    $wheres=[];
                    foreach ($where as $entity){
                        $where_data = [];
                        foreach ($primary_key as $v){
                            $pk=$entity->__get($v);
                            $where_data[] = $db->quoteColumn($v) . '=' . $db->quoteValue($pk,$columns->getType($v));
                        }
                        $wheres[] = implode(" and ", $where_data);
                    }
                    if(count($wheres)==0)return false;
                    return "((".implode(") or (", $wheres)."))";
                }else{
                    $wheres=[];
                    foreach ($where as $entity){
                        $pk=$entity->__get($primary_key);
                        $wheres[] = $db->quoteColumn($primary_key) . '=' . $db->quoteValue($pk,$columns->getType($primary_key));
                        
                    }
                    if(count($wheres)==0)return false;
                    return "(".implode(" or ", $wheres).")";
                }
                
            }else return strval($where);
        }
    }
}
