<?php
namespace LSYS\Entity\Database;
interface Result extends \Iterator{
    /**
     * 根据名称得到对应的当前值
     * @param string $name
     * @param mixed $default
     * @return mixed
     */
    public function get($name, $default = NULL);
    /**
     * 迭代时,是否对已迭代结果进行释放
     * 调用次函数后,迭代只需要保证一次正常执行
     * 一般当结果集非常大时使用，防止内存不足
     */
    public function fetchFree();
    /**
     * 已从资源中获取的结果数量
     * 完成一次完整迭代后保证该值等于结果中的总数
     * 迭代过程中不确定，可能是迭代次数，也可能时结果总数
     * @return int
     */
    public function fetchCount();
}