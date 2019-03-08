<?php
namespace LSYS\Entity;
interface Table {
    /**
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
	 */
	public function tableName();
	/**
	 * 返回主键字段名
	 */
	public function primaryKey();
}