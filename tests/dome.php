<?php
use LSYS\Entity;
use TestLSYSEntityLib\DomeModelTestPkArr;
include __DIR__.'/autoload.php';
$model1=new DomeModelTestPkArr();
$e=new Entity($model1);
$e->id=rand(300,5000);
$e->code=uniqid("code_");
$e->name="ddddddddddddddd";
$e->enname="ddddddddddddddd";
$e->save();
$e->enname="dddddddddd";
$e->save();