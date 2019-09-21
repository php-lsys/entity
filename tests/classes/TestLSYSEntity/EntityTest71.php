<?php
namespace TestLSYSEntity;
use PHPUnit\Framework\TestCase;
use TestLSYSEntityLib\DomeModelTest;
use LSYS\Entity;
class EntityTest71 extends TestCase
{
    public function testColumn(){
        $model1=new DomeModelTest();
        $e=new Entity($model1);
        $e->name="fff";
        $e->values(array(
            "name"=>"temp"
        ));
        $e->asArray();
    }
}