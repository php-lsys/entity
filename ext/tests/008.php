<?php
use LSYS\Entity\Validation;
use LSYS\Entity\Table;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Column;
use LSYS\Entity\Database;
use LSYS\Entity;
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
$e=new Entity(new mddtest());
$new =new Validation($e);
$new->valid(array("id"=>"111","ids"=>"ddd"));
?>

--EXPECT--



