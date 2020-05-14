<?php
namespace LSYS\Entity\Database;
use LSYS\Entity\Database;
if (!class_exists(SQLRuner::class)){
    class SQLRuner{
        private $_db;
        private $_sql;
        public function __construct(Database $db,$sql) {
            $this->_db=$db;
            $this->_sql=$sql;
        }
        /**
         * 返回SQL
         * @return string
         */
        public function toSql() {
            return $this->_sql;
        }
        /**
         * 返回SQL
         * @return string
         */
        public function __toString() {
            return $this->_sql;
        }
        /**
         * 执行SQL
         * @return bool
         */
        public function exec() {
            if (empty($this->_sql))return true;
            return $this->_db->exec($this->_sql);
        }
    }
}
