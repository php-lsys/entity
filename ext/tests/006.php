--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

$c=new LSYS\Entity\EntityColumnSet(["id","name"],[
    (new LSYS\Entity\Column("sss"))->setComment("dddd"),
    (new LSYS\Entity\Column("bbb"))->setDefault("iiii"),
]);
var_dump($c);
var_dump($c->notCustom());
var_dump($c->asColumnSet(new LSYS\Entity\ColumnSet([
    (new LSYS\Entity\Column("id"))->setComment("dddd"),
]),true));

var_dump($c->asColumnSet(new LSYS\Entity\ColumnSet([
    (new LSYS\Entity\Column("id"))->setComment("dddd"),
    (new LSYS\Entity\Column("name"))->setDefault("iiii"),
]),false));



?>
--EXPECT--



