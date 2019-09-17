<?php
use LSYS\Entity;
include __DIR__.'/autoload.php';
$e=new Entity(new \TestLSYSEntityLib\DomeModelTestPkArr());
$e->id=mt_rand(5000,100000000);
$e->code="fasd";
$e->name="ddddddddddddddd";
$e->enname="ddddddddddddddd";
$e->save();