<?php
namespace LSYS\Entity;
// @codeCoverageIgnoreStart
if (!interface_exists(Table::class)){
    interface Table {
    	/**
    	 * 数据库操作对象
    	 * @return \LSYS\Entity\Database
    	 */
    	public function db();
    	/**
    	 * 得到数据库表字段集合
    	 * @return ColumnSet|Column[]
    	 */
    	public function tableColumns();
    	/**
    	 * 返回表名
    	 * @return string
    	 */
    	public function tableName();
    	/**
    	 * 返回主键字段名,联合主键返回字段名数组
    	 * @return string|array
    	 */
    	public function primaryKey();
    }
}
// @codeCoverageIgnoreEnd