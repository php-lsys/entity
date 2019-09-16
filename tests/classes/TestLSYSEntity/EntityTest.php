<?php
namespace TestLSYSEntity;
use PHPUnit\Framework\TestCase;
use TestLSYSEntityLib\DomeEntityTest;
use TestLSYSEntityLib\DomeModelTest;
use LSYS\Entity;
use LSYS\Entity\EntityColumnSet;
use TestLSYSEntityLib\DomeModelTestPkArr;
class LSYSEntity extends TestCase
{
    public function testEntity()
    {
        $model1=new DomeModelTest();
        $e=new Entity($model1);
        $this->assertEquals("id", $e->columns(true)->offsetGet("id")->asArray()['name']);
        $this->assertTrue(is_array($e->labels()));
        $this->assertNull($e->filter());
        $this->assertNull($e->validation());
        $e->id=1;
        $this->assertEquals($e->pk(), 1);
        $data=["id"=>10,"name"=>"ttt","unkown"=>"temp"];
        $e->loadData($data,null,true);
        $this->assertEquals($e->name, "ttt");
        $this->assertTrue($e->loaded());
        unset($data['unkown']);
        $this->assertEquals($data, $e->exportData());
        $e->name="ttt";
        $this->assertArrayNotHasKey("name", $e->changed());
        $e->name="bbb";
        $this->assertArrayHasKey("name", $e->changed());
        $this->assertTrue(isset($e->id));
        $this->assertFalse($e->saved());
        $e->clear();
        $this->assertFalse($e->loaded());
        $this->name="fff";
        $e->loadData($data,new EntityColumnSet(["id","name"]),true);
        $this->assertNotEmpty(json_encode($e));
        unset($e->id);
        $this->assertFalse($e->loaded());
        $e->values(array(
            "id"=>"11",
            "name"=>"temp"
        ));
        $this->assertArrayHasKey("name", $e->asArray());
        $this->assertFalse($e->loaded());
        $this->assertEquals($e,$e->check());
        $this->assertEquals($e->pk(),strval($e));
        $e->clear();
        $e->loadData($data,null,true);
        $e->id=11;
        $this->assertFalse($e->loaded());
        $e->id=10;
        $this->assertTrue($e->loaded());
    }
    public function testEntitySetEx1()
    {
        $model1=new DomeModelTest();
        $e=new Entity($model1);
        $this->expectException(\LSYS\Entity\Exception::class);
        $e->id_bad_columns=1;
    }
    public function testEntityGetEx1()
    {
        $model1=new DomeModelTest();
        $e=new Entity($model1);
        $this->assertEquals($e->id, null);
        $this->expectException(\LSYS\Entity\Exception::class);
        $e->id_bad_columns;
    }
    public function testEntityPkArr()
    {
        $model1=new DomeModelTestPkArr();
        $e=new Entity($model1);
        $this->assertTrue(is_array($e->pk()));
        $e->loadData(["id"=>1,"code"=>"code"],null,true);
        $this->assertTrue($e->loaded());
        $e->id=2;
        $this->assertFalse($e->loaded());
        $e->id=1;
        $this->assertTrue($e->loaded());
        $e->clear();
        $e->id=2;
        $e->code='code2';
        $this->assertEquals(["id"=>2,"code"=>"code2"], $e->pk());
        $this->expectException(\LSYS\Entity\Exception::class);
        $e->id_unkown=11;
    }
    public function testModel()
    {
        $model1=new DomeModelTest();
        $entity=new DomeEntityTest($model1);
        $entity->values(array(
            'name'=>"fasdfadsf",
            "acronym"=>"11",
            "code"=>11
        ));
        $entity->enname="name afasdfa <strip>ss</sss> ";
        $entity->save();
        $this->assertTrue($entity->loaded());
        $this->assertTrue($entity->saved());
        $this->assertEmpty($entity->changed());
        $entity->name="bbbbb";
        $this->assertArrayHasKey("name", $entity->changed());
        $entity->save();
        $this->assertArrayHasKey("id", $entity->asArray());
        $pk=$entity->pk();
        
        $this->assertEquals($pk,strval($entity));
        $entity->clear();
        $this->assertFalse($entity->saved());
        $this->assertFalse($entity->loaded());
        
        $res=$model1->findbywhere("id>0");
        foreach ($res as $value) {
            $this->assertTrue($value instanceof Entity);
            $this->assertTrue($value->loaded());
        }
//         $t=$res->asArray();
//         $t=array_shift($t);
//         $this->assertArrayHasKey("id", $t);
//         $t=$res->asArray("id");
//         $key=array_keys($t);
//         $this->assertEquals($key[0], $t[$key[0]]['id']);
//         $tt=$res->asArray(null,"id");
//         $this->assertEquals($tt[0],$key[0]);
//         $ttt=$res->asArray("id","name");
//         $this->assertEquals($ttt[$key[0]], $t[$key[0]]['name']);
        
        $this->assertTrue($res->fetchCount(0)>0);
        
        $entity=$model1->findById($pk);
        $this->assertTrue($entity->loaded());
        $entity->delete();
        $this->assertFalse($entity->loaded());
        $entity=$model1->findById($pk);
        $this->assertFalse($entity->loaded());
    }
    public function testModelValid()
    {
        $this->expectException(\LSYS\Entity\Exception::class);
        $model1=new DomeModelTest();
        $entity=new DomeEntityTest($model1);
        $entity->enname="1";
        $entity->name="bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
        $entity->save();
    }
}