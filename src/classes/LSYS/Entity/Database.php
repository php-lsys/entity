<?php
namespace LSYS\Entity;
interface Database{
	/**
	 * 包裹表名
	 * @param string $table
	 * @return string
	 */
	public function quoteTable($table);
	/**
	 * 包裹字段
	 * @param string $column
	 * @return string
	 */
	public function quoteColumn($column);
	/**
	 * 包裹值
	 * @param string $value
	 * @return string
	 */
	public function quoteValue($value,$column_type);
	/**
	 * 执行语句
	 * @param string
	 * @throws Exception
	 * @return bool
	 */
	public function exec($sql,array $data=[]);
	/**
	 * 返回最后插入的ID
	 * @return int
	 */
	public function insertId();
}