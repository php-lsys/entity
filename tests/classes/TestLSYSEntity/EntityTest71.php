<?php
namespace TestLSYSEntity;
use PHPUnit\Framework\TestCase;
use TestLSYSEntityLib\DomeModelTest;
use LSYS\Entity;
class EntityTest71 extends TestCase
{
     public function testEntityPkArrChange()
        {
            $model1=new DomeModelTestPkArr();
            $e=new Entity($model1);
            $e->id=rand(300,5000);
            $e->code=uniqid("code_");
            $e->name="ddddddddddddddd";
            $e->enname="ddddddddddddddd";
            $e->save();
            $this->assertTrue($e->saved());
            $e->enname="dddddddddd";
            $e->save();
            $this->assertTrue($e->saved());
        }
}