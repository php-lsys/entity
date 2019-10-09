<?php
use LSYS\Entity\Table;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Column;
use LSYS\Entity\Database;
use LSYS\Entity;
use LSYS\Entity\EntitySet;
use LSYS\Entity\Database\Builder;
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
    public function builder(Table $table)
    {
        return new Builder($table);
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
class DomeDBResult implements \LSYS\Entity\Database\Result {
    public function __construct(array $ps){

    }
    public function get($name, $default = NULL)
    {

    }
    public function next()
    {

    }
    public function seek($position)
    {

    }
    public function valid()
    {

    }
    public function current()
    {

    }
    public function rewind()
    {

    }
    public function key()
    {

    }
    public function fetchCount($iterator = false)
    {

    }
    public function setFetchFree()
    {

    }
}
class DomeEntityTest extends Entity{
    public function __construct(mddtest $model1){
        parent::__construct($model1);//非必须
    }
    public function labels(){
        return array(
            "enname"=>"名称"
        );
    }
}
$d= new EntitySet(new DomeDBResult([]),DomeEntityTest::class);
var_dump($d);
?>

--EXPECT--



