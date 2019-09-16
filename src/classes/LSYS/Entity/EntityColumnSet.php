<?php
namespace LSYS\Entity;
if (!class_exists(EntityColumnSet::class)){
    class EntityColumnSet{
        /**
         * @var array
         */
        protected $_columns;
        /**
         * @var array
         */
        protected $_patch_columns;
        /**
         * 查询请求字段
         * @param string[] $column 在表中的字段
         * @param Column[] $patch_columns 不在指定表中的附加字段
         */
        public function __construct(array $column=null,array $patch_columns=[]){//a_id,b_name
            foreach ($column as $v){
                assert(is_scalar($v));
            }
            foreach ($patch_columns as $v){
                assert($v instanceof Column);
            }
            $this->_columns=$column;
            $this->_patch_columns=$patch_columns;
        }
        /**
         * 是否是自定义请求字段
         * 当返回false时,当前对象中的字段列表可能跟表字段存在不同
         * @return boolean
         */
        public function isCustom() {
            return !is_array($this->_columns)&&empty($this->_patch_columns);
        }
        /**
         * 转换为ColumnSet对象
         * @param ColumnSet $table_columns
         * @param boolean $patch
         * @return \LSYS\Entity\ColumnSet
         */
        public function asColumnSet(ColumnSet $table_columns,$patch=false) {
            if (!is_array($this->_columns)) return $table_columns;
            $columntype=[];
            foreach ($this->_columns as $v){
                if($table_columns->offsetExists($v))$columntype[$v]=$table_columns->offsetGet($v);
            }
            if($patch)foreach ($this->_patch_columns as $v){
                $columntype[$v->name()]=$v;
            }
            return new ColumnSet($columntype);
        }
    }
}