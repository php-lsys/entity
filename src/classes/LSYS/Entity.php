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
     * 装载数据指定数据
     * @param array $data
     * @param boolean $loaded
     * @param EntityColumnSet $query_column_set
     * @return static
     */
    public function loadData(array $data,EntityColumnSet $query_column_set=null,$loaded=true){
        $this->clear();
        $this->_query_column_set=$query_column_set;
        $columns=$this->columns(true);//因为设置时候保持一致,所以这里需要解析
        foreach ($data as $column=>$value){
            if ($columns->offsetExists($column)){
                $data[$column]=$columns->offsetGet($column)->read($value);
            }else unset($data[$column]);
        }
        $this->_data=$data;
        $this->_loaded=$loaded&&array_key_exists($this->table()->primaryKey(), $data);
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
     * @param string $column
     * @param mixed $value
     * @throws Exception
     */
    public function __set($column,$value){
        $columns=$this->columns(true);
        if (!$columns->offsetExists($column)){
            $msg=$this->table()->i18n()->__("The :column property does not exist in the :entity entity",array(
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
        if($this->loaded()&&$column==$this->table()->primaryKey()){
            if($columns->offsetGet($column)->compare($this->_change[$column],$value)){//还原主键值或未修改值
                $this->_loaded=true;
            }else{
                //更改主键值
                foreach ($this->_data as $k=>$v){//保存所有原数据加载数据
                    if (!array_key_exists($k, $this->_change))$this->_change[$k]=$v;
                }
                $this->_loaded=false;
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
        $msg=$this->table()->i18n()->__("The :column property does not exist in the :entity entity",array(
            "column"=>$column,"entity"=>get_class($this)
        ));
        throw new Exception($msg);
    }
    /**
     * 返回当前实体主键
     * @return mixed
     */
    public function pk(){
        $primary_key=$this->table()->primaryKey();
        if (!isset($this->_data[$primary_key])) return null;
        return $this->_data[$primary_key];
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
            $msg=$table->i18n()->__('Cannot update :object model because it is not loaded.',array("object"=>__CLASS__));
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
        $primary_key=$table->primaryKey();
        $where = $db->quoteColumn($primary_key) . '=' . $db->quoteValue($this->pk(),$columns->getType($primary_key));
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
    /**
     * 新增数据
     * @param Validation $validation
     * @throws Exception
     * @return static
     */
    public function create(Validation $validation=null){
        $table=$this->table();
        if ($this->loaded()){
            $msg=$table->i18n()->__('Cannot create :object model because it is already loaded.',array("object"=>__CLASS__));
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
        
        if (! array_key_exists ( $primary_key, $data )) {
            $this->_data[$primary_key]=$db->insertId();
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
            $msg=$this->table()->i18n()->__('Cannot delete :object model because it is not loaded.',array("object"=>__CLASS__));
            throw new Exception ($msg);
        }
        $table=$this->table();
        $db=$table->db();
        $table_name=$db->quoteTable($table->tableName());
        $primary_key=$table->primaryKey();
        $id = $this->pk ();
        $table_column=$table->tableColumns();
        $where = $db->quoteColumn($primary_key) . '=' . $db->quoteValue($id,$table_column->getType($primary_key));
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
            throw (new Exception($this->table()->i18n()->__("validation data fail")))->setValidationError($validation->errors());
        }
        return $this;
    }
    /**
     * 批量__set
     * @param array $values
     * @param array $expected
     * @return static
     */
    public function values(array $values, array $expected = NULL) {
        if ($expected === NULL) {
            $expected = $this->columns(true)->asArray(ColumnSet::TYPE_FIELD);
            unset ( $values [$this->table()->primaryKey()] );
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