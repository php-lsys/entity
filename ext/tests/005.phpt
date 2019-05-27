--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

$b=[
    new LSYS\Entity\Column("id"),
    new LSYS\Entity\Column("name"),
];

$c=new LSYS\Entity\ColumnSet($b);
print_r($c);

?>
--EXPECT--


