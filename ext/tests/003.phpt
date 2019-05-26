--TEST--
check db interface
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 

try{
$ss= new LSYS\Entity\Exception("ssss");
$ss->setErrorSql("select sss");
$ss->setVaildationError(["aaa"=>"ddd","bb"=>["ddd"]]);
throw $ss;
}catch(\Exception $e){
   var_dump($e->getErrorSql());
    var_dump($e->getmessage());
    var_dump($e->getVaildationError());
    print_r($e);


}
?>
--EXPECT--


