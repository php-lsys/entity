--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

try{
$ss= new LSYS\Entity\Exception("ssss");
$ss->setErrorSql("select sss");
$ss->setValidationError(["aaa"=>"ddd","bb"=>["ddd"]]);//可能有bug
throw $ss;
}catch(\LSYS\Entity\Exception $e){
   var_dump($e->getErrorSql());
    var_dump($e->getmessage());
    var_dump($e->getValidationError());
    print_r($e);


}
?>
--EXPECT--


