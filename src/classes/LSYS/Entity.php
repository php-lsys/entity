<?php
namespace LSYS;
use LSYS\Entity\Table;
use LSYS\Entity\EntityColumnSet;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Filter;
use LSYS\Entity\Exception;
use LSYS\Entity\Validation;
use LSYS\Entity\ColumnSave;
abstract class Entity implements \JsonSerializable{
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
     * @var array
     */
    protected $_data= array ();
    /**
     * 有更改时保留首次加载数据
     * @var array
     */
    protected $_change= array ();
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
    public function __construct(Table $table=null) {
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
        if ($this->_table==null) $this->_table=(new \ReflectionClass($this->tableClass()))->newInstance();
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
        $this->_data=array_merge($data,$this->_data);
        $primaryKey=$this->table()->primaryKey();
        if(is_array($primaryKey)){
            foreach ($primaryKey as $v){
                $loaded=$loaded&&array_key_exists($v, $data);
                if(!$loaded)break;
            }
        }else $loaded=$loaded&&array_key_exists($primaryKey, $data);
        $this->_loaded=$loaded;
        $this->_saved=$this->_valid=$this->_loaded;
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
        $this->_saved =false;
        $this->_valid =false;
        $this->_data =array();
        $this->_change =array();
        $this->_loaded =false;
        $this->_columns =null;
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
    public function __unset($column){
        $pkey=$this->table()->primaryKey();
        if((is_array($pkey)&&in_array($column,$pkey,true))
          ||$pkey==$column){
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
        if (!$this->_columns) {
            $table_columns=$this->table()->tableColumns();
            if ($this->_query_column_set instanceof EntityColumnSet) {
                $this->_columns=$this->_query_column_set->asColumnSet($table_columns,$patch);
            }else{
                $this->_columns=$table_columns;
            }
        }
        return $this->_columns;
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
                "column"=>$column,"entity"=>get_class($this)
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
        
        if($this->loaded()){
            $primaryKey=$this->table()->primaryKey();
            if((is_array($primaryKey)&&in_array($column, $primaryKey,true))
                ||$primaryKey==$column){
                //更改主键值
                if (is_array($primaryKey)){
                    $change=!$columns->offsetGet($column)->compare($this->_change[$column],$value);
                    unset($primaryKey[$column]);
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
                if($change){//更改主键
                    foreach ($this->_data as $k=>$v){//保存所有原数据更改值数组
                        if (!array_key_exists($k, $this->_change))$this->_change[$k]=$v;
                    }
                    $this->_loaded=false;
                }else{
                    //还原主键值，将更改值与原值相同的删除掉
                    foreach ($this->_change as $k=>$v){
                        $nv=isset($this->_data[$k])?$this->_data[$k]:null;
                        $colobj=$columns->offsetGet($column);
                        if(is_object($colobj)&&$colobj->compare($v,$nv))unset($this->_change[$k]);
                    }
                    $this->_loaded=true;
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
            "column"=>$column,"entity"=>get_class($this)
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
        $table=$this->table();
        if ( ! $this->loaded()){
            $msg=strtr('Cannot update :object model because it is not loaded.',array("object"=>__CLASS__));
            throw new Exception($msg);
        }
        $columns=$this->columns(false);
        $save_data=array();
        foreach ($columns as $v){
            $name=$v->name();
            if ($v instanceof ColumnSave){
                $v->update($this,$name);
            }
            if (!$v->isAllowNull()
                &&array_key_exists($name, $this->_data)
                &&is_null($this->_data[$name])) {
                    $this->_data[$name]='';
            }
            if(array_key_exists($name, $this->_change)){
                $save_data[$name]=$this->_data[$name];
            }
        }
        if (count($save_data)==0){
            $this->_change=array();
            $this->_saved=true;
            return $this;
        }
        if ( ! $this->_valid OR $validation)
        {
            $this->check($validation);
        }
        $db=$table->db();
        $where=$this->_modify_where();
        $table_name = $db->quoteTable( $table->tableName() );
        $sets = array ();
        foreach ( $save_data as $key => $value ) {
            $set = $db->quoteColumn($key) . " = " . $db->quoteValue( $value,$columns->getType($key));
            array_push ( $sets, $set );
        }
        $str_set = implode ( ",", $sets );
        $sql = " UPDATE " . $table_name . " SET " . $str_set." WHERE " . $where;
        $db->exec($sql);
        $this->_change=array();
        $this->_saved=true;
        return $this;
    }
    private function _modify_where(){
        $table=$this->table();
        $columns=$table->tableColumns();
        $primary_key=$table->primaryKey();
        $db=$table->db();
        if (is_array($primary_key)){
            $where = [];
            foreach ($primary_key as $v){
                $pk=isset($this->_data[$v])?$this->_data[$v]:null;
                $where[] = $db->quoteColumn($v) . '=' . $db->quoteValue($pk,$columns->getType($v));
            }
            $where = implode(" and ", $where);
        }else{
            $pk=isset($this->_data[$primary_key])?$this->_data[$primary_key]:null;
            $where = $db->quoteColumn($primary_key) . '=' . $db->quoteValue($pk,$columns->getType($primary_key));
        }
        return $where;
    }
    /**
     * 新增数据
     * @param Validation $validation
     * @throws Exception
     * @return static
     */
    public function create(Validation $validation=null){
        $table=$this->table();
        if ($this->loaded()){
            $msg=strtr('Cannot create :object model because it is already loaded.',array("object"=>__CLASS__));
            throw new Exception ( $msg);
        }
        
        $table_column=$table->tableColumns();
        $save_data=array();
        foreach ($table_column as $v){
            $name=$v->name();
            if ($v instanceof ColumnSave) $v->create($this, $name);
            if(!array_key_exists($name, $this->_data)&&$v->useDefault()){
                $this->_data[$name]=$v->getDefault();
            }
            if (!$v->isAllowNull()
                &&array_key_exists($name, $this->_data)
                &&is_null($this->_data[$name])) {
                $this->_data[$name]='';
            }
            if(array_key_exists($name, $this->_data)){
                $save_data[$name]=$this->_data[$name];
            }
        }
        
        if ( ! $this->_valid OR $validation)
        {
            $this->check($validation);
        }
        
        $field=array();
        $data=array();
        $db=$table->db();
        foreach ($save_data as $key=>$value)
        {
            array_push($field, $db->quoteColumn($key));
            array_push($data, $db->quoteValue($value,$table_column->getType($key)));
        }
        $str_field=implode(",",$field);
        $str_data='('.implode(",", $data).')';
        
        $table_name=$db->quoteTable($table->tableName());
        $sql=" INSERT INTO ".$table_name." (".$str_field.")VALUES {$str_data}";
        $db->exec( $sql );
        $this->_change=array();
        $this->_saved=$this->_loaded=true;
        $primary_key=$table->primaryKey();
        if (!is_array($primary_key)&&! array_key_exists ( $primary_key, $data )) {
            if(!is_null($db->insertId())) $this->_data[$primary_key]=$db->insertId();
        }
        return $this;
    }
    /**
     * 删除数据并清空当前对象
     * @throws Exception
     * @return static
     */
    public function delete() {
        if (! $this->loaded ()){
            $msg=strtr('Cannot delete :object model because it is not loaded.',array("object"=>__CLASS__));
            throw new Exception ($msg);
        }
        $table=$this->table();
        $db=$table->db();
        $table_name=$db->quoteTable($table->tableName());
        $id = $this->pk ();
        $table_column=$table->tableColumns();
        $where=$this->_modify_where();
        $sql=" DELETE FROM ".$table_name." where ".$where;
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
        return strval($this->pk());
    }
    /**
     * 检查当前已设置数据是否合规
     * @param Validation $validation
     * @return static
     */
    public function check(Validation $validation=null) {
        //调用时候创建 或者在外部创建传入 不传参数每次调用check都创建一个新校验对象
        if (!$validation)$validation=$this->validation();
        if (!$validation)return $this;
        if (($this->_valid = $validation->valid($this->_data+$this->columns(true)->asArray(ColumnSet::TYPE_DEFAULT))) === FALSE)
        {
            throw (new Exception(strtr("validation data fail")))->setValidationError($validation->errors());
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
    /**
     * 返回ORM名
     * @return string
     */
    abstract public function tableClass();
}
