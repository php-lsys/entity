<?php
namespace LSYS\Entity\Database;
interface Result extends \Countable,\SeekableIterator{
    public function get($name, $default = NULL);
}