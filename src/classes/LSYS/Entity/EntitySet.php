<?php
namespace LSYS\Entity;
use LSYS\Entity\Database\Result;
use LSYS\Entity;
if (!class_exists(EntitySet::class)){
    class EntitySet implements \Iterator{
        /**
         * @var Result
         */
        protected $_result;
        /**
         * @var EntityColumnSet|NULL
         */
        protected $_columns;
        /**
         * @var Table|NULL
         */
        protected $_table;
        /**
         * @var string
         */
        protected $_entity;
        public function __construct(Result $result,$entity_name,EntityColumnSet $columns=null,Table $table=null)
        {
            $this->_result = $result;
            $this->_table=$table;
            $this->_entity=strval($entity_name);
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
         public function asArray( $key = NULL, $value = NULL)
         {
            $keep_key=$this->key();
            $this->rewind();
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
            while (true) {
                if(!$this->valid()||$this->key()==$keep_key)break;
                $this->next();
            }
            return $results;
        }
        /**
         * @return Entity
         */
        public function current()
        {
            $row=$this->_result->current();
            if (is_null($row))return null;
            /**
             * @var Entity $entity
             */
            $entity=(new \ReflectionClass($this->_entity))->newInstance($this->_table);
            if (is_null($this->_table)) {
                $this->_table=$entity->table();
            }
            return $entity->loadData($row,$this->_columns,true);
        }
    }
}