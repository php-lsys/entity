<?php
use TestLSYSEntityLib\DomeModelTest;
include __DIR__.'/autoload.php';
$model1=new DomeModelTest();
$res=$model1->findbywhere("id>0");
print_r($res->asArray());
