<?php
use LSYS\Entity;
use LSYS\Entity\Table;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Column;
?>
--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

class mddtest implements Table{
    public function tableColumns()
    {
        return new ColumnSet([
            new Column("id"),
            new Column("b"),
        ]);
    }
    public function db()
    {}
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

$e->loadData(["id"=>"bbb","b"=>"ccc","m"=>"sss"]);



var_dump($e);

?>
--EXPECT--



