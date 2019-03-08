<?php
namespace LSYS\Entity;
interface I18n{
    /**
     * 多语言翻译
     * @param string $string
     * @param array $values
     */
    public function __($string, array $values = NULL);
}