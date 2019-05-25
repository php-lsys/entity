--TEST--

--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

$column= new LSYS\Entity\Column("id");
$column->setAllowNull(true);

var_dump($column->isAllowNull());

var_dump($column->name());

var_dump($column->setDefault("ddd"));
var_dump($column->getDefault());

var_dump($column);



$column= new LSYS\Entity\Column("ppp");

?>
--EXPECT--
