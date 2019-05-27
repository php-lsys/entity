--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

$c=new LSYS\Entity\ColumnSet([
    (new LSYS\Entity\Column("id"))->setComment("dddd"),
    (new LSYS\Entity\Column("name"))->setDefault("iiii"),
]);



$c->add((new LSYS\Entity\Column("id"))->setType("type")->setComment("mmmm"),true);

//print_r($c->asArray(LSYS\Entity\ColumnSet::TYPE_DEFAULT));
//print_r($c->asArray(LSYS\Entity\ColumnSet::TYPE_FIELD));
//print_r($c->asArray(LSYS\Entity\ColumnSet::TYPE_ARRAY));

var_dump($c->count());
var_dump($c->current());
var_dump($c->offsetGet("id"));
var_dump($c->getType("id"));


//print_r($c);

?>
--EXPECT--



