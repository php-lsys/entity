<?php
namespace LSYS\Entity;
if (!class_exists(Exception::class)){
    class Exception extends \Exception{
        public function __construct($message, $code = 0, \Exception $previous = NULL)
        {
            $message=strval($message);
            if (DIRECTORY_SEPARATOR === '\\'&&$this->_isGb2312($message)){
                if(PHP_SAPI!=='cli'||PHP_SAPI==='cli'&&version_compare(PHP_VERSION,'7.0.0',">=")){
                    $message=iconv("gb2312", "utf-8",$message);//GB2312字符转UTF-8
                }
            }
            parent::__construct($message,$code,$previous);
        }
        private function _isGb2312($str)
        {
            for($i=0; $i<strlen($str); $i++) {
                $v = ord( $str[$i] );
                if( $v > 127) {
                    if( ($v >= 228) && ($v <= 233) )
                    {
                        if(($i+2) >= (strlen($str)- 1)) return true;
                        $v1 = ord( $str[$i+1] );
                        $v2 = ord( $str[$i+2] );
                        if( ($v1 >= 128) && ($v1 <=191) && ($v2 >=128) && ($v2 <= 191) ) // utf编码
                            return false;
                            else
                                return true;
                    }
                }
            }
            return true;
        }
        /**
         * @var string
         */
        private $_error_sql;
        /**
         * set error sql
         * @param string $sql
         * @return static
         */
        public function setErrorSql($sql){
            $this->_error_sql=$sql;
            return $this;
        }
        /**
         * get error sql
         * @return string
         */
        public function getErrorSql(){
            return $this->_error_sql;
        }
        /**
         * @var array
         */
        private $_validation_error=[];
        /**
         * set validation error
         * @param array $error
         * @return $this
         */
        public function setValidationError(array $error){
            $msg=[];
            foreach ($error as $v){
                if (is_string($v))$msg[]=$v;
            }
            if (count($msg)>0)$this->message.=":".implode(";",$msg);
            $this->_validation_error=$error;
            return $this;
        }
        /**
         * get validateion message
         * @return array
         */
        public function getValidationError(){
            return $this->_validation_error;
        }
    }
}