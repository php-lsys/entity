--TEST--
check column class
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

$column= new LSYS\Entity\Column("id");
echo "is true:\n";
var_dump($column->name()=='id');

var_dump($column->setcomment("fasdfa"));
echo "is true:\n";
var_dump($column->comment()=='fasdfa');

var_dump($column->setDefault("ddddef"));
echo "is true:\n";
var_dump($column->getDefault("ddddef")=='ddddef');

var_dump($column->settype("ddd"));
echo "is true:\n";
var_dump($column->gettype("ddd")=='ddd');

var_dump($column->setAllowNull(true));
echo "is true:\n";
var_dump($column->isAllowNull());


var_dump($column->read("fasd"));

var_dump((string)$column);
var_dump($column->asarray());
echo "is false:\n";
var_dump($column->compare("a",null));
echo "is false:\n";
var_dump($column->compare("",null));
echo "is true:\n";
var_dump($column->compare("a","a"));


$column1= new LSYS\Entity\Column("ppp");
$column1->copy($column);

var_dump($column1);

?>
--EXPECT--


