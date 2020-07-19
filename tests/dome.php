<?php
use LSYS\Entity;
use TestLSYSEntityLib\DomeModelTest;
include __DIR__.'/autoload.php';
$model1=new DomeModelTest();
$e=new Entity($model1);
$e->id_bad_columns=1;
