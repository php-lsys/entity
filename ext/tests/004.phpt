<?php
use LSYS\Entity;
use LSYS\Entity\Database;
use LSYS\Entity\ColumnSave;
use LSYS\Entity\Validation;
use LSYS\Entity\Database\Result;
use LSYS\Entity\FilterRule;
use LSYS\Entity\ValidRule;
use LSYS\Entity\Table;
?>
--TEST--
check entity
--SKIPIF--
<?php if (!extension_loaded("entity")) print "skip"; ?>
--FILE--
<?php 


class mytestdb implements Database{
    public function quoteColumn($column)
    {}

    public function exec($sql, array $data = [])
    {}

    public function quoteTable($table)
    {}

    public function quoteValue($value, $column_type = null)
    {}

    public function insertId()
    {}
}

class mytestColumnSaveobj implements ColumnSave{
    public function update(Entity $entity, $column)
    {}

    public function create(Entity $entity, $column)
    {}
}

class mytestdbres implements Result{
    public function next()
    {}

    public function valid()
    {}

    public function current()
    {}

    public function rewind()
    {}

    public function fetchCount($iterator = false)
    {}

    public function get($name, $default = NULL)
    {}

    public function setFetchFree()
    {}

    public function key()
    {}

    
}


class mytestfilterrule implements FilterRule{
    public function filter(Entity $entity, $value, $field)
    {}

    
}

class myvalidrule implements ValidRule{
    public function allowEmpty()
    {}

    public function check(Validation $validation, $field, $value, $label, Entity $entity, array $check_data)
    {}

    
}

class mytesttable implements Table{
    public function tableColumns()
    {}

    public function db()
    {}

    public function tableName()
    {}

    public function primaryKey()
    {}
}

?>
--EXPECT--


