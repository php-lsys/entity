<?php
namespace TestLSYSEntity;
use PHPUnit\Framework\TestCase;
use LSYS\Entity;
use TestLSYSEntityLib\DomeModelTestPkArr;
class EntityTest71 extends TestCase
{
     public function testEntityPkArrChange()
     {//travis-ci php 7.1 存在问题
            $model1=new DomeModelTestPkArr();
            $e=new Entity($model1);
            $e->name="ddddddddddddddd";
            $e->enname="ddddddddddddddd";
            $e->save();
        }
}