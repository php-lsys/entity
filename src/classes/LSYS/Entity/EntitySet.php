<?php
namespace LSYS\Entity;
use LSYS\Entity\Database\Result;
use LSYS\Entity;
class EntitySet implements \Countable, \Iterator, \SeekableIterator, \ArrayAccess{
    protected $_result;
    protected $_columns;
    protected $_table;
    protected $_entity;
    public function __construct(Result $result,$entity_name,EntityColumnSet $columns=null,Table $table=null)
    {
        $this->_result = $result;
        $this->_table=$table;
        $this->_entity=$entity_name;
        $this->_columns=$columns;
    }
    public function offsetExists($offset)
    {
        return $this->seek($offset);
    }
    public function offsetGet($offset)
    {
         if ( ! $this->seek($offset)) return NULL;
         return $this->current();
    }
    final public function offsetSet($offset, $value)
    {
        throw new Exception('entity results are read-only');
    }
     final public function offsetUnset($offset)
     {
         throw new Exception('entity results are read-only');
     }
     public function key()
     {
         return $this->_result->key();
     }
     public function next()
     {
         return $this->_result->next();
     }
     public function rewind()
     {
         return $this->_result->rewind();
     }
     public function valid()
     {
         return $this->_result->valid();
     }
    public function asArray($key = NULL, $value = NULL)
    {
        $results = array();
        
        if ($key === NULL AND $value === NULL)
        {
            // Indexed rows
            
            foreach ($this as $row)
            {
                $results[] = $row->asArray();
            }
        }
        elseif ($key === NULL)
        {
            // Indexed columns
            foreach ($this as $row)
            {
                $results[] = $row->__get($value);
            }
        }
        elseif ($value === NULL)
        {
            foreach ($this as $row)
            {
                $row=$row->asArray();
                $results[$row[$key]] = $row;
            }
        }
        else
       {
            foreach ($this->_result as $row)
            {
                $results[$row->__get($key)] = $row->__get($value);
            }
        }
        $this->rewind();
        return $results;
    }
    public function seek($position)
    {
        return $this->_result->seek($position);
    }
    /**
     * @return Entity
     */
    public function current()
    {
        $row=$this->_result->current();
        if (is_null($row))return null;
        return (new \ReflectionClass($this->_entity))->newInstance($this->_table)->loadData($row,$this->_columns,true);
    }
    public function count()
    {
        return $this->_result->count();
    }
}