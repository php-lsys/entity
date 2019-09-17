<?php
namespace LSYS\Entity;
use LSYS\Entity;
// @codeCoverageIgnoreStart
if (!interface_exists(ValidRule::class)){
    interface ValidRule{
        /**
         * 是否可以为空
         * @return bool
         */
        public function allowEmpty();
        /**
         * 检测 有问题使用 $validation->error();设置错误
         * @param Validation $validation
         * @param mixed $value
         * @param string $label
         * @param Entity $entity
         * @param array $check_data
         */
        public function check(Validation $validation,$field,$value,$label,Entity $entity,array $check_data);
    }
}
// @codeCoverageIgnoreEnd