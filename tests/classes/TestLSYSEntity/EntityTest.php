<?php
namespace TestLSYSEntity;
use PHPUnit\Framework\TestCase;
use TestLSYSEntityLib\DomeEntityTest;
use TestLSYSEntityLib\DomeModelTest;
use LSYS\Entity;
use LSYS\Entity\EntityColumnSet;
use TestLSYSEntityLib\DomeModelTestPkArr;
use LSYS\Entity\Column;
class LSYSEntity extends TestCase
{
    //     public function testColumn(){
    //         $c=new Column("a");
    //         $c->setAllowNull(true);
    //         $this->assertTrue($c->isAllowNull());
    //         $c->setComment("aa");
    //         $this->assertEquals("aa", $c->comment());
    //         $c->setAllowNull(false);
    //         $this->assertEquals($c->getDefault(), '');
    //         $c->setDefault(1);
    //         $this->assertEquals($c->getDefault(), 1);
    //         $c->setType("int");
    //         $this->assertEquals($c->getType(), 'int');
    //         $this->assertEquals(strval($c),"a");
    //         $b=new Column("b");
    //         $b->copy($c);
    //         $this->assertEquals($b->getType(),"int");
    //         $this->assertEquals($b->getDefault(), 1);
    //     }
    //     public function testEntity()
    //     {
    //         $model1=new DomeModelTest();
    //         $e=new Entity($model1);
    // //         $this->assertEquals("id", $e->columns(true)->offsetGet("id")->asArray()['name']);
    // //         $this->assertEquals("name", $e->columns(true)->offsetGet("name")->asArray()['name']);
    // //         $this->assertEquals("code", $e->columns(true)->offsetGet("code")->asArray()['name']);
    // //         $this->assertTrue(is_array($e->labels()));
    // //         $this->assertNull($e->filter());
    // //         $this->assertNull($e->validation());
    // //         $e->id=1;
    // //         $this->assertEquals($e->pk(), 1);
    //       //  $data=["id"=>10,"name"=>"ttt","unkown"=>"temp"];
    // //         $e->loadData($data,null,true);
    // //         $this->assertEquals($e->name, "ttt");
    // //         $this->assertTrue($e->loaded());
    // //         unset($data['unkown']);
    // //         $this->assertEquals($data, $e->exportData());
    // //         $e->name="ttt";
    // //         $this->assertArrayNotHasKey("name", $e->changed());
    // //         $e->name="bbb";
    // //         $this->assertArrayHasKey("name", $e->changed());
    // //         $this->assertTrue(isset($e->id));
    // //         $this->assertFalse($e->saved());
    // //         $e->clear();
    //         //$this->assertFalse($e->loaded());
    //         $e->name="fff";
    //         //$this->assertTrue((new EntityColumnSet())->notCustom());
    //        // $ecs=new EntityColumnSet(["id","name"]);
    //        // $this->assertFalse($ecs->notCustom());
    //       //  $e->loadData($data,$ecs,true);
    //         //$this->assertNotEmpty(json_encode($e));
    //         //unset($e->id);
    //        // $this->assertFalse($e->loaded());
    //         $e->values(array(
    //             "id"=>"11",
    //             "name"=>"temp"
    //         ));
    //         $this->assertArrayHasKey("name", $e->asArray());
    // //         $this->assertFalse($e->loaded());
    //       //  $this->assertEquals($e,$e->check());
    // //         $this->assertEquals($e->pk(),strval($e));
    // //         $e->clear();
    // //         $e->loadData($data,null,true);
    // //         $e->id=11;
    // //         $this->assertFalse($e->loaded());
    // //         $e->id=10;
    // //         $this->assertTrue($e->loaded());
    //     }
    public function testEx()
    {
        $model1=new DomeModelTest();
        $e=new Entity($model1);
       // $e->name="fff";
        $e->values(array(
            // "id"=>"11",
            "name"=>"temp"
        ));
        $e->asArray();
        
//           $this->expectException(\LSYS\Entity\Exception::class);
//          $sql="select sleep(1)";
//          $err=array("error1","error2");
//         try{
//                 throw (new \LSYS\Entity\Exception(""))
//         ->setErrorSql($sql)
//          ->setValidationError($err)
//         ;
//          }catch(\LSYS\Entity\Exception $e){
//         $this->assertEquals($sql, $e->getErrorSql());
//         $this->assertEquals($err, $e->getValidationError());
//          $this->assertNotEmpty($e->getMessage());
//            throw $e;
//          }
    }
    //     public function testEntitySetEx1()
    //     {
    //         $model1=new DomeModelTest();
    //         $e=new Entity($model1);
    //         $this->expectException(\LSYS\Entity\Exception::class);
    //         $e->id_bad_columns=1;
    //     }
    //     public function testEntityGetEx1()
    //     {
    //         $model1=new DomeModelTest();
    //         $e=new Entity($model1);
    //         $this->assertEquals($e->id, null);
    //         $this->expectException(\LSYS\Entity\Exception::class);
    //         $e->id_bad_columns;
    //     }
    //     public function testEntityPkArrChange()
    //     {
    //         $model1=new DomeModelTestPkArr();
    //         $e=new Entity($model1);
    //         $e->id=rand(300,5000);
    //         $e->code=uniqid("code_");
    //         $e->name="ddddddddddddddd";
    //         $e->enname="ddddddddddddddd";
    //         $e->save();
    //         $this->assertTrue($e->saved());
    //         $e->enname="dddddddddd";
    //         $e->save();
    //         $this->assertTrue($e->saved());
    //     }
    //     public function testEntityPkArr()
    //     {
    //         $model1=new DomeModelTestPkArr();
    //         $e=new Entity($model1);
    //         $this->assertTrue(is_array($e->pk()));
    //         $e->loadData(["id"=>1,"code"=>"code"],null,true);
    //         $pk=$e->pk();
    //         $this->assertArrayHasKey("id", $pk);
    //         $this->assertArrayHasKey("code", $pk);
    //         $this->assertTrue($e->loaded());
    //         $e->id=2;
    //         $this->assertFalse($e->loaded());
    //         $e->id=1;
    //         $this->assertTrue($e->loaded());
    //         $e->clear();
    //         $e->id=2;
    //         $e->code='code2';
    //         $this->assertEquals(["id"=>2,"code"=>"code2"], $e->pk());
    //         $this->expectException(\LSYS\Entity\Exception::class);
    //         $e->id_unkown=11;
    //     }
    //     public function testModel()
    //     {
    //         $model1=new DomeModelTest();
    //         $entity=new DomeEntityTest($model1);
    //         $entity->values(array(
    //             'name'=>"fasdfadsf",
    //             "acronym"=>"11",
    //             "code"=>11
    //         ));
    //         $entity->enname="name afasdfa <strip>ss</sss> ";
    //         $entity->save();
    //         $this->assertTrue($entity->loaded());
    //         $this->assertTrue($entity->saved());
    //         $this->assertEmpty($entity->changed());
    //         $entity->update();
    //         sleep(1);//自动更新时间
    //         $entity->name="bbbbb";
    //         $this->assertArrayHasKey("name", $entity->changed());
    //         $entity->save();
    //         $this->assertArrayHasKey("id", $entity->asArray());
    //         $pk=$entity->pk();
    
    //         $this->assertEquals($pk,strval($entity));
    //         $entity->clear();
    //         $this->assertFalse($entity->saved());
    //         $this->assertFalse($entity->loaded());
    
    //         $res=$model1->findbywhere("id>0");
    //         foreach ($res as $value) {
    //             $this->assertTrue($value instanceof Entity);
    //             $this->assertTrue($value->loaded());
    //         }
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
    
    //         $this->assertTrue($res->fetchCount(0)>0);
    
    //         $entity=$model1->findById($pk);
    //         $this->assertTrue($entity->loaded());
    //         $entity->delete();
    //         $this->assertFalse($entity->loaded());
    //         $entity=$model1->findById($pk);
    //         $this->assertFalse($entity->loaded());
    //     }
    //     public function testEntityUpdateEx(){
    //         $model1=new DomeModelTest();
    //         $this->expectException(\LSYS\Entity\Exception::class);
    //         $e=$model1->findById(-11);
    //         $e->name="dddaaaa";
    //         $e->enname="dddaaaa";
    //         $e->update();
    //     }
    //     public function testEntityDeleteEx(){
    //         $model1=new DomeModelTest();
    //         $this->expectException(\LSYS\Entity\Exception::class);
    //         $e=$model1->findById(-11);
    //         $e->delete();
    //     }
    //     public function testEntityCreateEx(){
    //         $model1=new DomeModelTest();
    //         $entity=new DomeEntityTest($model1);
    //         $entity->values(array(
    //             'name'=>"fasdfadsf",
    //             "acronym"=>"11",
    //             "code"=>11
    //         ));
    //         $entity->enname="name afasdfa <strip>ss</sss> ";
    //         $entity->save();
    //         $this->expectException(\LSYS\Entity\Exception::class);
    //         $entity->create();
    //     }
    //     public function testModelValid()
    //     {
    //         $this->expectException(\LSYS\Entity\Exception::class);
    //         $model1=new DomeModelTest();
    //         $entity=new DomeEntityTest($model1);
    //         $entity->enname="11111111111111111111111111111111111111111111";
    //         $entity->name="bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    //         $entity->save();
    //     }
}