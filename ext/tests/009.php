<?php
use LSYS\Entity\Table;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Column;
use LSYS\Entity\Database;
use LSYS\Entity;
use LSYS\Entity\FilterRule;
use LSYS\Entity\Filter;
use LSYS\Entity\Database\SQLBuilder;
?>
--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php
class mytestdb implements Database{
    public function quoteTable($table){
        return $table;
    }
    public function quoteColumn($column){
        return $column;
    }
    public function quoteValue($value,$column_type=null){
        return "'".$value."'";
    }
    public function exec($sql,array $data=[]){
        print_r($sql);
    }
    public function insertId(){
        return 1;
    }
    public function SQLBuilder(Table $table)
    {
        return new SQLBuilder($table);
    }

}

class mddtest implements Table{
    public function tableColumns()
    {
        return new ColumnSet([
            new Column("id"),
            new Column("ids"),
            new Column("b"),
        ]);
    }
    public function db()
    {return new mytestdb();}
    public function tableName()
    {
        return "bbb";
    }
    public function primaryKey()
    {
        return ["id","ids"];
    }
}
class DomeFilterCallback implements FilterRule{
    protected $_fun;
    protected $_param;
    /**
     * 回调方式实现过滤器
     * @param string $fun 函数名
     * @param array $param 参数格式
     */
    public function __construct($fun,$param=array(":value")){
        $this->_fun=$fun;
        $this->_param=$param;
    }
    public function filter(Entity $entity, $value, $field)
    {
        $_bound = array (
            ':field' => $field,
            ':entity' => $entity,
            ':value' => $value
        );
        $filter = $this->_fun;
        $params = $this->_param;
        foreach ( $params as $key => $param ) {
            if (is_string ( $param ) and array_key_exists ( $param, $_bound )) {
                $params [$key] = $_bound [$param];
            }
        }
        if (is_array ( $filter ) or ! is_string ( $filter )) {
            $value = call_user_func_array ( $filter, $params );
        } elseif (strpos ( $filter, '::' ) === FALSE) {
            $function = new \ReflectionFunction ( $filter );
            $value = $function->invokeArgs ( $params );
        } else {
            list ( $class, $method ) = explode ( '::', $filter, 2 );
            $method = new \ReflectionMethod ( $class, $method );
            $value = $method->invokeArgs ( NULL, $params );
        }
        return $value;
    }
}
$e=new Entity(new mddtest());
$new =new Filter($e,["id"=>[new DomeFilterCallback(function($value){
                               return strip_tags($value);
                           })]],true);
//$new->rules([new DomeFilterCallback(function($value){
//    return strip_tags($value);
//})]);
$new->rule(new DomeFilterCallback(function($value){
    return strip_tags($value);
}),"id");
var_dump($new->runFilter("id","aa<aa>sdfasbb"));
var_dump($new->allowCache());

?>

--EXPECT--



