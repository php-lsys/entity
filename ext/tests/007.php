<?php
use LSYS\Entity;
use LSYS\Entity\Table;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Column;
use LSYS\Entity\Database;
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
        return "id";
    }
}
$e=new Entity(new mddtest());

$e->loadData(["id"=>"bbb",'ids'=>"bbb","b"=>"adfasd","m"=>"sss","dd"=>"fasdf"]);
var_dump($e->loaded());
$e->b="fasdfa";
$e->ids="fasdfa";
var_dump($e->changed());
var_dump($e->pk());
var_dump($e->loaded());
var_dump($e->saved());
var_dump($e->check());

$e->save();

$e->clear();


var_dump($e->loaded());
$e->b="fasdfa";
$e->ids="fasdfa";

var_dump($e->values(array("b"=>"ddd")));

$e->create();



var_dump($e->filter());
var_dump($e->labels());
var_dump($e->validation());
var_dump($e->table());
var_dump($e->exportData());
var_dump($e->columns(true));
var_dump($e->columns(0));
var_dump($e->columns(0));
var_dump($e->asArray(0));

//var_dump($e->delete());

?>

--EXPECT--



