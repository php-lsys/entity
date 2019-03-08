<?php
use LSYS\Entity\EntitySet;
use LSYS\Entity\Database;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\I18n;
use LSYS\Entity\Table;
use LSYS\Entity\Column;
use LSYS\Entity\Exception;
use LSYS\Entity\EntityColumnSet;
use LSYS\Entity\Filter;
use LSYS\Entity\FilterRule\Callback;
use LSYS\Entity\Validation;
use LSYS\Entity\ValidRule\StrlenRange;
use LSYS\Entity\ValidRule\NumRange;
use LSYS\Entity;
include_once __DIR__."/boot.php";
//数据库结果集实现
class pdores implements \LSYS\Entity\Database\Result {
    protected $_data;
    protected $_index=0;
    protected $_total=0;
    public function __construct(PDOStatement $ps){
        $this->_data=$ps->fetchAll();
        $this->_total=$ps->rowCount();
    }
    public function get($name, $default = NULL)
    {
        $row=$this->current();
        if (is_array($row)&&array_key_exists($name, $row))return $row[$name];
        return $default;
    }
    public function next()
    {
        if ($this->_index>=$this->_total) {
            return false;
        }
        $this->_index++;
        return true;
    }
    public function seek($position)
    {
        return array_key_exists($position,$this->_data);
    }
    public function valid()
    {
        return $this->_index<$this->_total&&$this->_index>=0;
    }
    public function current()
    {
        return isset($this->_data[$this->_index])?$this->_data[$this->_index]:null;
    }
    public function rewind()
    {
        $this->_index=0;
        return true;
    }
    public function key()
    {
        return $this->_index;
    }
    public function count()
    {
        return $this->_total;
    }
}
//数据库实现,一般框架都带有,增加实现即可
/*
//示例表结构
CREATE TABLE `address` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL,
  `enname` varchar(64) NOT NULL,
  `acronym` varchar(12) NOT NULL,
  `code` varchar(21) NOT NULL,
  `add_time` int(11) NOT NULL,
  PRIMARY KEY (`id`)
)
*/
class dbobj extends PDO implements Database{
    protected static $obj;
    public static function instance() {
        if(!self::$obj){
            self::$obj=new self('mysql:host=127.0.0.1;dbname=test','root','110');
            self::$obj->exec('SET NAMES "utf8"');
        }
        return self::$obj;
    }
    /**
     * @param string $sql
     * @return pdores
     */
    public function query($sql){
        $res=parent::query($sql);
        if($res===false){
            $msg=$this->errorInfo();
            throw new Exception(array_pop($msg));
        }
        return new pdores($res);
    }
    public function exec($sql)
    {
        $res=parent::exec($sql);
        if($res===false){
            $msg=$this->errorInfo();
            throw new Exception(array_pop($msg));
        }
        return $res;
    }
    public function quoteColumn($column)
    {
        if(empty($column)) return '';
        $identifier='';
        // Identifiers are escaped by repeating them
        $escaped_identifier = $identifier . $identifier;
        
        if (is_array ( $column )) {
            list ( $column, $alias ) = $column;
            $alias = str_replace ( $identifier, $escaped_identifier, $alias );
        }
        // Convert to a string
        $column = ( string ) $column;
        
        $column = str_replace ( $identifier, $escaped_identifier, $column );
        if ($column === '*') {
            return $column;
        } elseif (strpos ( $column, '.' ) !== FALSE) {
            $parts = explode ( '.', $column );
            foreach ( $parts as & $part ) {
                if ($part !== '*') {
                    // Quote each of the parts
                    $part = $identifier . $part . $identifier;
                }
            }
            $column = implode ( '.', $parts );
        } else {
            $column = $identifier . $column . $identifier;
        }
        if (isset ( $alias )) {
            $column .= ' AS ' . $identifier . $alias . $identifier;
        }
        return $column;
    }
    public function quoteTable($table)
    {
        $identifier='`';
        $escaped_identifier = $identifier . $identifier;
        if (is_array ( $table )) {
            list ( $table, $alias ) = $table;
            $alias = str_replace ( $identifier, $escaped_identifier, $alias );
        }
        $table = ( string ) $table;
        $table = str_replace ( $identifier, $escaped_identifier, $table );
        
        if (strpos ( $table, '.' ) !== FALSE) {
            $parts = explode ( '.', $table );
            
            foreach ( $parts as & $part ) {
                // Quote each of the parts
                $part = $identifier . $part . $identifier;
            }
            
            $table = implode ( '.', $parts );
        } else {
            // Add the table prefix
            $table = $identifier . $table . $identifier;
        }
        
        if (isset ( $alias )) {
            // Attach table prefix to alias
            $table .= ' AS ' . $identifier. $alias . $identifier;
        }
        return $table;
    }
    public function quoteValue($value, $column_type)
    {
        try{
            if ($value === NULL) {
                return $this->quote($value,PDO::PARAM_NULL);
            } elseif ($value === TRUE||$value === FALSE) {
                return $this->quote($value,PDO::PARAM_BOOL);
            } elseif (is_object ( $value )) {
                return $this->quoteValue ( ( string ) $value );
            } elseif (is_array ( $value )) {
                return '(' . implode ( ', ', array_map ( array (
                    $this,
                    __FUNCTION__
                ), $value ) ) . ')';
            } elseif (is_int ( $value )) {
                return $this->quote($value,PDO::PARAM_INT);
            } elseif (is_float ( $value )) {
                return $this->quote(sprintf ( '%F', $value ),PDO::PARAM_STR);
            }
            return $this->quote($value);
        }catch (\Exception $e){//callback can't throw exception...
            return "'".addslashes($value)."'";
        }
    }
    public function insertId()
    {
        return $this->lastInsertId();
    }
}
//多语言实现
class i18nobj implements I18n{
    protected static $obj;
    public static function instance() {
        if(!self::$obj)self::$obj=new self("./i18n","default");
        return self::$obj;
    }
    protected $_domain;
    protected $_dir;
    public function __construct($dir,$domain){
        $this->_dir=$dir;
        $this->_domain=$domain;
    }
    public function __($string, array $values = NULL)
    {
        bind_textdomain_codeset($this->_domain, 'UTF-8' );
        bindtextdomain($this->_domain, $this->_dir);
        $string=dgettext($this->_domain,$string);
        if(is_array($values)){
            foreach ($values as $k=>$v){
                $values[$k]=(string)$v;
            }
            $string=strtr($string, $values);
        }
        return $string;
    }
}
//实体
class entity1 extends Entity{
    public function tableClass()
    {
        return model1::class;
    }
    public function labels(){
        return array(
          "enname"=>"名称"  
        );
    }
    public function filterFactory() {
        return (new Filter($this))
            ->rule(new Callback("trim"))
            ->rule(new Callback("strip_tags"),"enname")
        ;
    }
    public function validationFactory(){
        return (new Validation($this))
        ->rule(new StrlenRange(1, 15, 0),"enname")
        ->rule(new NumRange(1, 1000, 0),"code")
        ;
    }
}
//ORM 或者MODEL
class model1 implements Table{
    protected $_table_columns;
    public function tableColumns()
    {
        if (!$this->_table_columns) {
            $this->_table_columns=new ColumnSet([
                new Column("id"),
                new Column("name"),
                new Column("enname"),
                new Column("acronym"),
                new Column("code"),
            ]);
        }
        return $this->_table_columns;
    }
    /**
     * @return dbobj
     */
    public function db()
    {
        return dbobj::instance();
    }
    public function i18n()
    {
        return i18nobj::instance();
    }
    public function tableName()
    {
        return "address";
    }
    public function primaryKey()
    {
        return 'id';
    }
    protected function columnSetAsStr($columns){
        if (!count($columns)) return "*";
        return implode(",", $columns->asArray(ColumnSet::TYPE_FIELD));
    }
    public function findById($id){
        $id=$this->db()->quoteValue($id, $this->tableColumns()->getType($this->primaryKey()));
        $table=$this->db()->quoteTable($this->tableName());
        $col=new EntityColumnSet(["id","name","add_time"],[new Column("enname")]);
        $cos=$this->columnSetAsStr($col->asColumnSet($this->tableColumns(),true));
        $sql="select {$cos} from {$table} where id={$id}";
        $row=$this->db()->query($sql);
        $entity=new entity1($this);
        if($row->count()>0) $entity->loadData($row->current(),$col);
        return $entity;
    }
    public function findByWhere($where='1',$limit=10){
        $table=$this->db()->quoteTable($this->tableName());
        $row=$this->db()->query("select * from {$table} where {$where} limit {$limit}");
        return new EntitySet($row,entity1::class,null,$this);
    }
}

// $model1=new model1();
//  $entit=$model1->findById(3597);
//  print_r($entit->asArray());
// $entit->name="sss";
// $entit->save();

$entity=new entity1();
$entity->values(array(
    'name'=>"fasdfadsf",
    "acronym"=>"11",
    "code"=>11
));
$entity->enname="name afasdfa <strip>ss</sss> ";
//$entity->validation()->rule(new \LSYS\Entity\ValidRule\Callback("ctype_upper",0),"enname");
var_dump($entity->saved());
$entity->save();
var_dump($entity->saved());
$entity->name="bbbbb";
var_dump($entity->saved());
print_r($entity->changed());
$entity->save();
print_r($entity->asArray());
$entity->clear();
print_r($entity->pk());
print_r($entity->asArray());

// $entity=new entity1();
// //$entity->id="1";
// $entity->name="ddddddddd";
// $entity->save();
// $id=$entity->pk();
// print_r($entity->asArray());
// print_r($id);
// $entity->delete();
// $entit=$model1->findById($id);
// var_dump($entit->loaded());

//$res=$model1->findbywhere("id<100");
//foreach ($res as $value) {
//    print_r($value->asarray());
//}


