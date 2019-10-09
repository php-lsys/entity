<?php
namespace LSYS;
use LSYS\Entity\Table;
use LSYS\Entity\EntityColumnSet;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Filter;
use LSYS\Entity\Exception;
use LSYS\Entity\Validation;
use LSYS\Entity\ColumnSave;
if (!class_exists(Entity::class)){
    class Entity implements \JsonSerializable{
        /**
         * @var Table
         */
        protected $_table;
        /**
         * @var EntityColumnSet
         */
        protected $_query_column_set;
        /**
         * @var ColumnSet
         */
        protected $_columns;
        /**
         * @var ColumnSet
         */
        protected $_patch_columns;
        /**
         * @var array
         */
        protected $_data= array ();
        /**
         * 有更改时保留首次加载数据
         * @var array
         */
        protected $_change= array ();
        /**
         * 更新时的上一主键值
         * @var array|string|int
         */
        protected $_change_pk;
        /**
         * @var Validation
         */
        protected $_validation;
        /**
         * @var Filter
         */
        protected $_filter;
        /**
         * @var bool
         */
        protected $_loaded=false;
        /**
         * @var bool
         */
        protected $_valid = false;
        /**
         * @var bool
         */
        protected $_saved = false;
        /**
         * 实体类
         * @param Table $table
         * @param EntityColumnSet $columns
         */
        public function __construct(Table $table) {
            $this->_table = $table;
        }
        /**
         * 字段注释
         * @return array
         */
        public function labels() {
            return [];
        }
        /**
         * 返回当前实体用的过滤对象
         * @return Filter|NULL
         */
        public function filter() {
            $filter = $this->_filter;
            if (!$filter){
                $filter = $this->filterFactory();
                if($filter&&$filter->allowCache())$this->_filter=$filter;
            }
            return $filter;
        }
        /**
         * 返回当前实体用的校验对象
         * @return \LSYS\Entity\Validation
         */
        public function validation()
        {
            $validation=$this->_validation;
            if (!$validation){
                $validation=$this->validationFactory();
                if ($validation&&$validation->allowCache())$this->_validation=$validation;
            }
            return $validation;
        }
        /**
         * 返回当前实体关联的ORM对象
         * @return Table
         */
        public function table(){
            return $this->_table;
        }
        /**
         * 装载数据指定数据,会通过字段规则进行转换,已存在模型加载数据请先调用clear清理
         * @param array $data
         * @param boolean $loaded
         * @param EntityColumnSet $query_column_set
         * @return static
         */
        public function loadData(array $data,EntityColumnSet $query_column_set=null,$loaded=true){
            $this->_query_column_set=$query_column_set;
            $columns=$this->columns(true);//因为设置时候保持一致,所以这里需要解析
            foreach ($data as $column=>$value){
                if ($columns->offsetExists($column)){
                    $data[$column]=$columns->offsetGet($column)->read($value);
                }else unset($data[$column]);
            }
            $this->_data=array_merge($this->_data,$data);
            $primaryKey=$this->table()->primaryKey();
            if(is_array($primaryKey)){
                foreach ($primaryKey as $v){
                    $loaded=$loaded&&array_key_exists($v, $data);
                    if(!$loaded)break;
                }
            }else $loaded=$loaded&&array_key_exists($primaryKey, $data);
            $this->_loaded=$loaded;
            $this->_saved=$this->_valid=$this->_loaded;
            $this->_change_pk=null;
            return $this;
        }
        /**
         * 导出当前已装载数据
         * @return array
         */
        public function exportData() {
            return $this->_data;
        }
        /**
         * 返回已修改数据
         * @return array
         */
        public function changed() {
            return array_intersect_key($this->_data,$this->_change);
        }
        /**
         * 清空当前实体
         * @return static
         */
        public function clear(){
            $this->_change_pk=null;
            $this->_saved =false;
            $this->_valid =false;
            $this->_data =array();
            $this->_change =array();
            $this->_loaded =false;
            $this->_columns =null;
            $this->_patch_columns =null;
            $this->_query_column_set =null;
            return $this;
        }
        /**
         * 判断指定字段是否设置过值
         * @param string $column
         * @return boolean
         */
        public function __isset($column){
            return array_key_exists($column,$this->_data);
        }
        /**
         * 删除已赋值的字段
         * @param string $column
         */
        public function __unset($column){
            $pkey=$this->table()->primaryKey();
            if((is_array($pkey)&&in_array($column,$pkey,true))
                ||$pkey==$column){
                    $this->_change_pk=null;
                    $this->_loaded=false;
            }
            unset($this->_data[$column]);
        }
        /**
         * 返回当前实体使用字段集合
         * @param boolean $patch
         * @return \LSYS\Entity\ColumnSet
         */
        public function columns($patch=false){
            if ($patch?(!$this->_patch_columns):(!$this->_columns)) {
                $table_columns=$this->table()->tableColumns();
                if ($this->_query_column_set instanceof EntityColumnSet) {
                    $columns=$this->_query_column_set->asColumnSet($table_columns,$patch);
                }else{
                    $columns=$table_columns;
                }
                $patch?($this->_patch_columns=$columns):($this->_columns=$columns);
            }
            return $patch?$this->_patch_columns:$this->_columns;
        }
        /**
         * 设置指定字段数据
         * 通过过滤规则进行过滤
         * 更改主键会变为未加载状态
         * @param string $column
         * @param mixed $value
         * @throws Exception
         */
        public function __set($column,$value){
            $columns=$this->columns(true);
            if (!$columns->offsetExists($column)){
                $msg=strtr("The :column property does not exist in the :entity entity",array(
                    ":column"=>$column,":entity"=>get_class($this)
                ));
                throw new Exception($msg);
            }
            $filter = $this->filter();
            if($filter)$value=$filter->runFilter($column,$value);
            
            if($this->loaded()) {
                if(array_key_exists($column, $this->_data)&&$columns->offsetGet($column)->compare($this->_data[$column],$value)){
                    $this->_data[$column]=$value;
                    return;
                }
            }
            if(!array_key_exists($column, $this->_change)){
                $this->_change[$column]=isset($this->_data[$column])?$this->_data[$column]:null;
            }
            
            $primaryKey=$this->table()->primaryKey();
            if((is_array($primaryKey)&&in_array($column, $primaryKey,true))||$primaryKey==$column){
                //更改主键值
                if (is_array($primaryKey)){
                    $change=!$columns->offsetGet($column)->compare($this->_change[$column],$value);
                    $_key_=array_search($column, $primaryKey);
                    if($_key_!==false)unset($primaryKey[$_key_],$_key_);
                    if(!$change)foreach ($primaryKey as $v){
                        if(isset($this->_change[$v])){
                            $data=isset($this->_data[$v])?$this->_data[$v]:null;
                            $change=$change||!$columns->offsetGet($v)->compare($this->_change[$v],$data);
                            if(!$change)break;
                        }
                    }
                }else{
                    $change=!$columns->offsetGet($column)->compare($this->_change[$column],$value);
                }
                if($change){//主键是否更改
                    if($this->loaded()){
                        foreach ($this->_data as $k=>$v){//保存所有原数据更改值数组
                            if (!array_key_exists($k, $this->_change))$this->_change[$k]=$v;
                        }
                        $this->_change_pk=$value;
                    }
                    $this->_loaded=false;
                }else{
                    //还原主键值，将更改值与原值相同的删除掉
                    if(!$this->loaded()&&$this->_change_pk){
                        foreach ($this->_change as $k=>$v){
                            $nv=isset($this->_data[$k])?$this->_data[$k]:null;
                            $colobj=$columns->offsetGet($k);
                            if(is_object($colobj)&&$colobj->compare($v,$nv))unset($this->_change[$k]);
                        }
                        $this->_loaded=true;
                        $this->_change_pk=null;
                    }
                }
            }
            $this->_saved = $this->_valid = FALSE;
            $this->_data[$column]=$value;
        }
        /**
         * 获取指定字段数据
         * @param string $column
         * @throws Exception
         * @return mixed
         */
        public function __get($column){
            if (array_key_exists($column, $this->_data)){
                return $this->_data[$column];
            }
            $columns=$this->columns(true);
            if ($columns->offsetExists($column)){
                $columnobj=$columns->offsetGet($column);
                return $columnobj->getDefault();
            }
            $msg=strtr("The :column property does not exist in the :entity entity",array(
                ":column"=>$column,":entity"=>get_class($this)
            ));
            throw new Exception($msg);
        }
        /**
         * 返回当前实体主键
         * @return mixed
         */
        public function pk(){
            if(!is_array($this->_data))return NULL;
            $primary_key=$this->table()->primaryKey();
            if(is_array($primary_key)){
                return array_intersect_key($this->_data, array_flip($primary_key));
            }else {
                if (!isset($this->_data[$primary_key])) return null;
                return $this->_data[$primary_key];
            }
        }
        /**
         * 当前实体是否已加载数据
         * @return boolean
         */
        public function loaded() {
            return $this->_loaded;
        }
        /**
         * 当前实体是否已进行数据保存
         * @return boolean
         */
        public function saved() {
            return $this->_saved;
        }
        /**
         * 更新已修改数据
         * @param Validation $validation
         * @throws Exception
         * @return static
         */
        public function update(Validation $validation=null){
            if ( ! $this->_valid OR $validation)
            {
                $this->check($validation);
            }
            $save_data=$this->updateData();
            if (count($save_data)==0){
                $this->_change=array();
                $this->_change_pk=null;
                $this->_saved=true;
                return $this;
            }
            $db=$this->table()->db();
            $sql=$db->builder($this->table())->update($save_data, $this);
            $db->exec($sql);
            $this->_change=array();
            $this->_saved=true;
            $this->_change_pk=null;
            return $this;
        }
        /**
         * 得到需要更新数据
         * @throws Exception
         */
        public function updateData(){
            if ( ! $this->loaded()){
                $msg=strtr('Cannot update :object model because it is not loaded.',array(":object"=>get_called_class()));
                throw new Exception($msg);
            }
            $table_column=$this->table()->tableColumns();
            $save_data=array();
            foreach ($table_column as $v){
                $name=$v->name();
                if(array_key_exists($name, $this->_change)){
                    $save_data[$name]=$this->_data[$name];
                }
            }
            return $save_data;
        }
        /**
         * 新增数据
         * @param Validation $validation
         * @throws Exception
         * @return static
         */
        public function create(Validation $validation=null,$unique_replace=false){
            if ( ! $this->_valid OR $validation)
            {
                $this->check($validation);
            }
            $save_data=$this->createData();
            $table=$this->table();
            $db=$table->db();
            $sql=$db->builder($table)->insert([$save_data],$unique_replace);
            $db->exec($sql);
            $this->_change=array();
            $this->_saved=$this->_loaded=true;
            $this->_change_pk=null;
            $primary_key=$table->primaryKey();
            if (!is_array($primary_key)&&! array_key_exists ( $primary_key, $save_data )) {
                if(!is_null($table->db()->insertId())) $this->_data[$primary_key]=$db->insertId();
            }
            return $this;
        }
        /**
         * 得到需要创建数据
         * @throws Exception
         */
        public function createData() {
            if ($this->loaded()){
                $msg=strtr('Cannot create :object model because it is already loaded.',array(":object"=>get_called_class()));
                throw new Exception ( $msg);
            }
            $table_column=$this->table()->tableColumns();
            $save_data=array();
            foreach ($table_column as $v){
                $name=$v->name();
                if(array_key_exists($name, $this->_data)){
                    $save_data[$name]=$this->_data[$name];
                }
            }
            return $save_data;
        }
        /**
         * 删除数据并清空当前对象
         * @throws Exception
         * @return static
         */
        public function delete() {
            if (! $this->loaded ()){
                $msg=strtr('Cannot delete :object model because it is not loaded.',array(":object"=>get_called_class()));
                throw new Exception ($msg);
            }
            $table=$this->table();
            $db=$table->db();
            $sql=$db->builder($table)->delete($this);
            $db->exec($sql);
            return $this->clear();
        }
        /**
         * 保存数据
         * @param Validation $validation
         * @return static
         */
        public function save(Validation $validation=null) {
            if ($this->loaded())$this->update($validation);
            else $this->create($validation);
            return $this;
        }
        /**
         * entity to string
         * @return int|NULL
         */
        public function __toString(){
            $pk=$this->pk();
            $pk=is_array($pk)?implode(",",$pk):$pk;
            return strval($pk);
        }
        /**
         * 检查当前已设置数据是否合规
         * @param Validation $validation
         * @return static
         */
        public function check(Validation $validation=null) {
            $table=$this->table();
            $table_column=$table->tableColumns();
            $pk=$this->table()->primaryKey();
            if(!$this->loaded()){
                foreach ($table_column as $v){
                    $name=$v->name();
                    if ($v instanceof ColumnSave){
                        $v->create($this, $name);
                    }
                    
                    if((is_array($pk)&&in_array($name, $pk))||$pk==$name)continue;
                    
                    if(!array_key_exists($name, $this->_data)&&$v->useDefault()){
                        $this->_data[$name]=$v->getDefault();
                    }
                    if (!$v->isAllowNull()
                        &&array_key_exists($name, $this->_data)
                        &&is_null($this->_data[$name])) {
                            $this->_data[$name]='';
                        }
                }
            }else{
                foreach ($table_column as $v){
                    $name=$v->name();
                    if ($v instanceof ColumnSave){
                        $v->update($this,$name);
                    }
                    if((is_array($pk)&&in_array($name, $pk))||$pk==$name)continue;
                    if (!$v->isAllowNull()
                        &&array_key_exists($name, $this->_data)
                        &&is_null($this->_data[$name])) {
                            $this->_data[$name]='';
                        }
                }
            }
            if (!$validation)$validation=$this->validation();
            if (!$validation)return $this;
            if (($this->_valid = $validation->valid($this->_data+$this->columns(true)->asArray(ColumnSet::TYPE_DEFAULT))) === FALSE)
            {
                throw (new Exception(strtr("validation data fail [:object]",array(":object"=>get_called_class()))))->setValidationError($validation->errors());
            }
            return $this;
        }
        /**
         * 批量__set,注意主键不能被设置
         * @param array $values
         * @param array $expected
         * @return static
         */
        public function values(array $values, array $expected = NULL) {
            if ($expected === NULL) {
                $expected = $this->columns(true)->asArray(ColumnSet::TYPE_FIELD);
                $pkey=$this->table()->primaryKey();
                if(is_array($pkey))foreach ($pkey as $v)unset ( $values [$v] );
                else unset ( $values [$pkey] );
            }
            foreach ( $expected as $column ) {
                if (! array_key_exists ( $column, $values )) continue;
                $this->__set($column, $values [$column]);
            }
            return $this;
        }
        /**
         * 装数据转换为数组
         * @return number
         */
        public function asArray() {
            return $this->_data+$this->columns(true)->asArray(ColumnSet::TYPE_DEFAULT);
        }
        /**
         * 返回JSON用数组
         * @return number
         */
        public function jsonSerialize () {
            return $this->asArray();
        }
        /**
         * 过滤器工厂
         * @return Filter|NULL
         */
        public function filterFactory() {
            return NULL;
        }
        /**
         * 校验器工厂
         * @return Validation
         */
        public function validationFactory(){
            return NULL;
        }
    }
}