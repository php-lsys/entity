<?php
use LSYS\Entity;
?>
--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

$e=new Entity();
var_dump($e);

?>
--EXPECT--



