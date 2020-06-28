<?php
namespace TestLSYSEntityLib;
//数据库结果集实现
class DomeDBResult implements \LSYS\Entity\Database\Result {
    /**
     * @var \PDOStatement
     */
    protected $result;
    protected $total_rows=0;
    protected $init=false;
    protected $index=0;
    //not all
    protected $row;
    //all
    protected $cache_rows=array();
    protected $fetch_free;
    
    /**
     * @param mixed $result
     * @param string $sql
     */
    public function __construct($result)
    {
        $result ->setFetchMode(\PDO::FETCH_ASSOC);
        $this->result=$result;
        $this->total_rows = $this->result->rowCount();
    }
    public function setFetchFree()
    {
        $this->fetch_free=true;
        return $this;
    }
    public function __destruct()
    {
        if ($this->result instanceof \PDOStatement){
            empty($this->cache_rows)&&@$this->result->fetchAll(\PDO::FETCH_ASSOC);
            $this->result->closeCursor();
        }
        $this->result=null;
        unset($this->cache_rows);
        unset($this->row);
    }
    protected function init(){
        if($this->init)return;
        $this->init=true;
        if(!$this->fetch_free){
            $this->cache_rows=$this->result->fetchAll();
            $this->result->closeCursor();
            $this->total_rows=count($this->cache_rows);
        }else{
            $this->fetch();
            if(is_null($this->row))$this->total_rows=0;//SQLLITE PDO 存在BUG
        }
    }
    /**
     * @param int $offset
     * @return boolean
     */
    private function fetch(){
        $this->row=$this->result->fetch();
        if(is_bool($this->row))$this->row=null;
        if($this->total_rows==0&&!is_null($this->row))$this->total_rows++;
    }
    /**
     * {@inheritDoc}
     * @see \Iterator::current()
     */
    public function current()
    {
        $this->init();
        if($this->fetch_free){
            return $this->row;
        }else{
            return $this->cache_rows[$this->index]??null;
        }
    }
    public function next()
    {
        $this->init();
        $this->index++;
        if ($this->fetch_free) {
            $this->fetch();
        }
    }
    public function valid()
    {
        $this->init();
        if ($this->fetch_free) {
            return !is_null($this->row);
        }else{
            return array_key_exists($this->index, $this->cache_rows);
        }
    }
    public function rewind()
    {
        $this->index=0;
    }
    public function key()
    {
        return $this->index;
    }
    public function fetchCount(bool $iterator = false)
    {
        $this->init();
        if(!$iterator||!$this->fetch_free){
            return $this->total_rows;
        }else{
            $this->total_rows+=count($this->result->fetchAll());
        }
    }
    public function get(string $name, $default = NULL)
    {
        return $this->current()[$name]??$default;
    }
}