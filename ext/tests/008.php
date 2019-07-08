<?php
use LSYS\Entity\Validation;
use LSYS\Entity\Table;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Column;
use LSYS\Entity\Database;
use LSYS\Entity;
use LSYS\Entity\ValidRule;
?>
--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php
class mytestdb implements Database{
    public function quoteTable($table){
        return $table;
    }
    public function quoteColumn($column){
        return $column;
    }
    public function quoteValue($value,$column_type=null){
        return "'".$value."'";
    }
    public function exec($sql,array $data=[]){
        print_r($sql);
    }
    public function insertId(){
        return 1;
    }
}

class mddtest implements Table{
    public function tableColumns()
    {
        return new ColumnSet([
            new Column("id"),
            new Column("ids"),
            new Column("b"),
        ]);
    }
    public function db()
    {return new mytestdb();}
    public function tableName()
    {
        return "bbb";
    }
    public function primaryKey()
    {
        return ["id","ids"];
    }
}

class DomeVaildNum implements ValidRule{
    protected $_min;
    protected $_max;
    protected $_allow_empty;
    /**
     * @param int $min 存在时表示不小于此值
     * @param int $max 存在时表示不大于此值
     * @param bool $allow_empty 是否不能为空
     */
    public function __construct($min,$max,$allow_empty) {
        $this->_min=intval($min);
        $this->_max=intval($max);
        $this->_allow_empty=boolval($allow_empty);
    }
    /**
     * @return bool
     */
    public function check(Validation $validation,$field,$value,$label,Entity $entity,array $check_data) {
        $param=array(
            ":label"=>$label,
            ":min"=>$this->_min,
            ":max"=>$this->_max,
            ":field"=>$field,
        );
        
        if(!is_numeric($value)){
            $validation->error($field, strtr(":label [:field] must be number",$param));
        }
        if ($this->_min>0&&$value<$this->_min) {
            $validation->error($field, strtr(":label [:field] can't be < :min",$param));
        }
        if ($this->_max>0&&$value>$this->_max) {
            $validation->error($field, strtr(":label [:field] can't be > :max",$param));
        }
    }
    public function allowEmpty()
    {
        return $this->_allow_empty;
    }
}
$e=new Entity(new mddtest());
$new =new Validation($e);
$new->label("id","主键");
$new->rule(new DomeVaildNum(1, 10, 0),"id");
$new->rule(new DomeVaildNum(1, 20, 0),"ids");
var_dump($new->valid(array("id"=>"111","ids"=>"2222")));
var_dump($new->errors());
//var_dump($new->error("ids", "dddd"));
//var_dump($new->error("idsw", "dddd"));
//var_dump($new->errors());
?>

--EXPECT--



