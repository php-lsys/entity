<?php
namespace LSYS\Entity;
use LSYS\Entity;
// @codeCoverageIgnoreStart
if (!interface_exists(FilterRule::class)){
    interface FilterRule{
        /**
         * 过滤器实现
         * @param Entity $entity
         * @param mixed $value
         * @param string $field
         * @return mixed $value
         */
        public function filter(Entity $entity,$value,string $field);
    }
}
// @codeCoverageIgnoreEnd