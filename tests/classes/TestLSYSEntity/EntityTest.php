<?php
namespace TestLSYSEntity;
use PHPUnit\Framework\TestCase;
use TestLSYSEntityLib\DomeEntityTest;
use TestLSYSEntityLib\DomeModelTest;
class LSYSEntity extends TestCase
{
    public function testinsert()
    {
        $model1=new DomeModelTest();
        $entit=$model1->findById(3597);
        print_r($entit->asArray());
        $entit->name="sss";
        $entit->save();
        $e=new \LSYS\Entity($model1);//可以直接用Entity类
        $entity=new DomeEntityTest($model1);
        $entity->values(array(
            'name'=>"fasdfadsf",
            "acronym"=>"11",
            "code"=>11
        ));
        $entity->enname="name afasdfa <strip>ss</sss> ";
        //$entity->validation()->rule(new DomeVaildCallback("ctype_upper",0),"enname");
        var_dump($entity->saved());
        $entity->save();
        var_dump($entity->saved());
        $entity->name="bbbbb";
        var_dump($entity->saved());
        var_dump($entity->changed());
        $entity->save();
        var_dump($entity->asArray());
        $entity->clear();
        var_dump($entity->pk());
        var_dump($entity->asArray());
        
        $entity=new DomeEntityTest($model1);
        $entity->id=1000000;
        $entity->enname="ddddddddd";
        $entity->name="ddddddddd";
        $entity->save();
        $id=$entity->pk();
        var_dump($entity->asArray());
        var_dump($id);
        $entity->delete();
        $entit=$model1->findById(1);
        var_dump($entit->loaded());
        
        $res=$model1->findbywhere("id<100");
        foreach ($res as $value) {
            var_dump($value->asarray());
        }
        
    }
}