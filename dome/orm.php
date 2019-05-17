<?php
include_once __DIR__."/boot.php";
/*
//示例表结构
CREATE TABLE `address` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(32) ,
  `enname` varchar(64) ,
  `acronym` varchar(12),
  `code` varchar(21) ,
  `add_time` int(11) ,
  PRIMARY KEY (`id`)
)
*/
$model1=new DomeModelTest();
//  $entit=$model1->findById(3597);
//  print_r($entit->asArray());
// $entit->name="sss";
// $entit->save();
//$e=new Entity($model1);//可以直接用Entity类
$entity=new DomeEntityTest($model1);
$entity->values(array(
    'name'=>"fasdfadsf",
    "acronym"=>"11",
    "code"=>11
));
$entity->enname="name afasdfa <strip>ss</sss> ";
$entity->validation()->rule(new DomeVaildCallback("ctype_upper",0),"enname");
var_dump($entity->saved());
$entity->save();
var_dump($entity->saved());
$entity->name="bbbbb";
var_dump($entity->saved());
print_r($entity->changed());
$entity->save();
print_r($entity->asArray());
$entity->clear();
print_r($entity->pk());
print_r($entity->asArray());

// $entity=new DomeEntityTest($model1);
// //$entity->id="1";
// $entity->name="ddddddddd";
// $entity->save();
// $id=$entity->pk();
// print_r($entity->asArray());
// print_r($id);
// $entity->delete();
// $entit=$model1->findById($id);
// var_dump($entit->loaded());

// $res=$model1->findbywhere("id<100");
// foreach ($res as $value) {
//    print_r($value->asarray());
// }


