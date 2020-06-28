<?php
namespace LSYS\Entity;
// @codeCoverageIgnoreStart
if (!interface_exists(Database::class)){
    interface Database{
    	/**
    	 * 包裹表名
    	 * @param string $table
    	 * @return string
    	 */
    	public function quoteTable($table):string;
    	/**
    	 * 包裹字段
    	 * @param string $column
    	 * @return string
    	 */
    	public function quoteColumn($column):string;
    	/**
    	 * 包裹值
         * $column_type　为NULL　根据$value类型推断
    	 * @param string $value
    	 * @param mixed $column_type
    	 * @return string
    	 */
    	public function quoteValue($value,$column_type=null):string;
    	/**
    	 * 执行语句
    	 * @param string
    	 * @throws Exception
    	 * @return bool
    	 */
    	public function exec(string $sql,array $data=[]):bool;
    	/**
    	 * 返回最后插入的ID
    	 * @return int|null
    	 */
    	public function insertId():?int;
    	/**
    	 * DB操作构造器
    	 * @return \LSYS\Entity\Database\SQLBuilder
    	 */
    	public function SQLBuilder(Table $table);
    }
}
// @codeCoverageIgnoreEnd