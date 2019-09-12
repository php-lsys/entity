<?php
namespace TestLSYSEntityLib;
//数据库结果集实现
class DomeDBResult implements \LSYS\Entity\Database\Result {
    protected $_data;
    protected $_index=0;
    protected $_total=0;
    public function __construct(\PDOStatement $ps){
        $this->_data=$ps->fetchAll();
        $this->_total=$ps->rowCount();
    }
    public function get($name, $default = NULL)
    {
        $row=$this->current();
        if (is_array($row)&&array_key_exists($name, $row))return $row[$name];
        return $default;
    }
    public function next()
    {
        if ($this->_index>=$this->_total) {
            return false;
        }
        $this->_index++;
        return true;
    }
    public function seek($position)
    {
        return array_key_exists($position,$this->_data);
    }
    public function valid()
    {
        return $this->_index<$this->_total&&$this->_index>=0;
    }
    public function current()
    {
        return isset($this->_data[$this->_index])?$this->_data[$this->_index]:null;
    }
    public function rewind()
    {
        $this->_index=0;
        return true;
    }
    public function key()
    {
        return $this->_index;
    }
    public function fetchCount($iterator = false)
    {
        return count($this->_total);
    }
    public function setFetchFree()
    {
        
    }
}