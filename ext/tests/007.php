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

$e->loadData(["a"=>"bbb","b"=>"ccc"]);




var_dump($e);

?>
--EXPECT--



