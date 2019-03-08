<?php
namespace LSYS\Entity;
/**
 * @method 
 */
class ColumnSet implements \Countable,\ArrayAccess,\Iterator{
    /**
     * 只返回字段
     * @var integer
     */
    const TYPE_FIELD=1;
    /**
     * 返回字段和默认值
     * @var integer
     */
    const TYPE_ARRAY=2;
    /**
     * 返回字段和默认值
     * @var integer
     */
    const TYPE_DEFAULT=3;
    protected $_columns=[];
    public function __construct(array $column){
        foreach ($column as $v){
            assert($v instanceof Column);
            $this->_columns[$v->name()]=$v;
        }
    }
    /**
     * 根据字段名取得字段对象的类型
     * @param string $name
     * @return number|NULL
     */
    public function getType($name){
        if ($this->offsetExists($name)) {
            return $this->offsetGet($name)->getType();
        }
        return null;
    }
    /**
     * 转换为数组
     * @param int $type
     * @return array
     */
    public function asArray($type){
        switch ($type) {
            case self::TYPE_FIELD:
                return array_keys($this->_columns);
            case self::TYPE_DEFAULT:
                $out=array();
                foreach ($this as $k=>$v){
                    $out[$k]=$v->getDefault();
                }
                return $out;
            case self::TYPE_ARRAY:
                $out=array();
                foreach ($this as $k=>$v){
                    $out[$k]=$v->asArray();
                }
                return $out;
        }
    }
    public function offsetExists($offset)
    {
        return array_key_exists($offset, $this->_columns);
    }
    public function offsetUnset($offset)
    {
        unset($this->_columns[$offset]);
    }
    /**
     * 根据字段名得到字段对象
     * @param string $column
     * @return Column
     */
    public function offsetGet($column)
    {
        return isset($this->_columns[$column])?$this->_columns[$column]:null;
    }
    public function offsetSet($offset, $value)
    {
        assert($value instanceof Column);
        $this->_columns[$offset]=$value;
    }
    public function next()
    {
        return next($this->_columns);
    }
    public function valid()
    {
        return $this->current()!=null;
    }
    public function current()
    {
        return current($this->_columns);
    }
    public function rewind()
    {
        return reset($this->_columns);
    }
    public function key()
    {
        return key($this->_columns);
    }
    public function count() {
        return count($this->_columns);
    }
}