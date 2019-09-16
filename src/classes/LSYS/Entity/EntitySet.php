<?php
namespace LSYS\Entity;
use LSYS\Entity\Database\Result;
use LSYS\Entity;
if (!class_exists(EntitySet::class)){
    class EntitySet implements \Iterator{
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
        public function setFetchFree(){
            $this->_result->setFetchFree();
            return $this;
        }
        public function fetchCount($iterator=false){
            return $this->_result->fetchCount($iterator);
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
                foreach ($this as $row)
                {
                    $results[$row->__get($key)] = $row->__get($value);
                }
            }
            $this->rewind();
            return $results;
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
    }
}