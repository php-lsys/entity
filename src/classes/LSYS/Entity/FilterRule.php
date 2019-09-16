<?php
namespace LSYS\Entity;
use LSYS\Entity;
if (!interface_exists(FilterRule::class)){
    interface FilterRule{
        /**
         * 过滤器实现
         * @param Entity $entity
         * @param mixed $value
         * @param string $field
         */
        public function filter(Entity $entity,$value,$field);
    }
}