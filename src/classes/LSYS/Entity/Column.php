<?php
namespace LSYS\Entity;
if (!class_exists(Column::class)){
    class Column{
        /**
         * @var string
         */
        protected $_name;
        /**
         * @var string
         */
        protected $_type;
        /**
         * @var bool
         */
        protected $_is_default;
        /**
         * @var mixed
         */
        protected $_default;
        /**
         * @var bool
         */
        protected $_is_nullable;
        /**
         * @var string
         */
        protected $_comment;
        /**
         * 字段
         * @param string $name
         */
        public function __construct($name){
            $this->_name=$name;
            $this->_default=NULL;
            $this->_type=NULL;
            $this->_comment=null;
            $this->_is_nullable=TRUE;
        }
        /**
         * 获取名
         * @return string
         */
        public function name(){
            return $this->_name;
        }
        public function __toString() {
            return $this->_name;
        }
        /**
         * 设置注释
         * @param string $comment
         * @return $this
         */
        public function setComment($comment){
            $this->_comment=$comment;
            return $this;
        }
        /**
         * 获取字段注释
         * @return string
         */
        public function comment(){
            return $this->_comment;
        }
        /**
         * 是否使用默认值(创建时)
         * @return mixed
         */
        public function useDefault(){
            return $this->_is_default;
        }
        /**
         * 设置默认值
         * @param mixed $default
         * @return $this
         */
        public function setDefault($default){
            $this->_is_default=true;
            $this->_default=$default;
            return $this;
        }
        /**
         * 得到默认值
         * @return mixed
         */
        public function getDefault(){
            return $this->_default;
        }
        /**
         * 设置字段类型
         * @param mixed $set_type
         * @return $this
         */
        public function setType($set_type){
            $this->_type=$set_type;
            return $this;
        }
        /**
         * 得到字段类型
         * @return mixed
         */
        public function getType(){
            return $this->_type;
        }
        /**
         * 设置是否可为空
         * @return $this
         */
        public function setAllowNull($set_allow_nullable){
            $this->_is_nullable=$set_allow_nullable;
            if ($set_allow_nullable) {
                if($this->_default==='')$this->_default=null;
            }else{
                if($this->_default===null)$this->_default='';
            }
            return $this;
        }
        /**
         * 得到是否允许为空
         * @return boolean
         */
        public function isAllowNull(){
            return $this->_is_nullable;
        }
        /**
         * 比较两个值,用在是否更新
         * @param string $old_val
         * @param string $new_val
         * @return bool
         */
        public function compare($old_val,$new_val){
            if ((!is_null($old_val)&&is_null($new_val))
                ||(is_null($old_val)&&!is_null($new_val)))return false;
            return $old_val==$new_val;
        }
        /**
         * 读取值时解析
         * @param string $val
         * @return mixed
         */
        public function read($val){
            return $val;
        }
        /**
         * 转换为数组
         * @return array
         */
        public function asArray(){
            return array(
              'name'=>$this->_name,
              'type'=>$this->_type,
              'default'=>$this->_default,
              'allowNull'=>$this->_is_nullable,
            );
        }
        /**
         * 拷贝除名字外的所有属性
         * @param Column $column
         * @return \LSYS\Entity\Column
         */
        public function copy(Column $column) {
            $this->_type=$column->_type;
            $this->_is_default=$column->_is_default;
            $this->_default=$column->_default;
            $this->_is_nullable=$column->_is_nullable;
            $this->_comment=$column->_comment;
            return $this;
        }
    }
}
