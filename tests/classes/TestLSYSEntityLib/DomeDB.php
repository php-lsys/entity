<?php
namespace TestLSYSEntityLib;
use LSYS\Entity\Database;
//一般框架都带自己的数据库实现,可基于框架自带的数据库实现以下Database接口,以下是基于PDO简易实现
class DomeDB implements Database{
    protected static $obj;
    public static function instance() {
        if(!self::$obj)self::$obj=new static();
        return self::$obj;
    }
    protected $pdo;
    public function __construct() {
        $this->pdo=new \PDO('mysql:host=127.0.0.1;dbname=test','root','');
        $this->pdo->exec('SET NAMES "utf8"');
    }
    /**
     * @param string $sql
     * @return DomeDBResult
     */
    public function query($sql,array $data=[]){
        $prepare=$this->pdo->prepare($sql);
        if(!$prepare->execute($data)){
            $msg=$prepare->errorInfo();
            throw new \Exception("sql exec error:".array_pop($msg)."-".$sql);
        }
        return new DomeDBResult($prepare);
    }
    public function exec($sql,array $data=[])
    {
        $prepare=$this->pdo->prepare($sql);
        if($prepare->execute($data)===false){
            $msg=$prepare->errorInfo();
            throw new \Exception("sql exec error:".array_pop($msg)."-".$sql);
        }
        return true;
    }
    public function insertId()
    {
        return $this->pdo->lastInsertId();
    }
    public function quoteColumn($column)
    {
        if(empty($column)) return '';
        $identifier='';
        // Identifiers are escaped by repeating them
        $escaped_identifier = $identifier . $identifier;
        
        if (is_array ( $column )) {
            list ( $column, $alias ) = $column;
            $alias = str_replace ( $identifier, $escaped_identifier, $alias );
        }
        // Convert to a string
        $column = ( string ) $column;
        $column = str_replace ( $identifier, $escaped_identifier, $column );
        if ($column === '*') {
            return $column;
        } elseif (strpos ( $column, '.' ) !== FALSE) {
            $parts = explode ( '.', $column );
            foreach ( $parts as & $part ) {
                if ($part !== '*') {
                    // Quote each of the parts
                    $part = $identifier . $part . $identifier;
                }
            }
            $column = implode ( '.', $parts );
        } else {
            $column = $identifier . $column . $identifier;
        }
        if (isset ( $alias )) {
            $column .= ' AS ' . $identifier . $alias . $identifier;
        }
        return $column;
    }
    public function quoteTable($table)
    {
        $identifier='`';
        $escaped_identifier = $identifier . $identifier;
        if (is_array ( $table )) {
            list ( $table, $alias ) = $table;
            $alias = str_replace ( $identifier, $escaped_identifier, $alias );
        }
        $table = ( string ) $table;
        $table = str_replace ( $identifier, $escaped_identifier, $table );
        
        if (strpos ( $table, '.' ) !== FALSE) {
            $parts = explode ( '.', $table );
            
            foreach ( $parts as & $part ) {
                // Quote each of the parts
                $part = $identifier . $part . $identifier;
            }
            
            $table = implode ( '.', $parts );
        } else {
            // Add the table prefix
            $table = $identifier . $table . $identifier;
        }
        
        if (isset ( $alias )) {
            // Attach table prefix to alias
            $table .= ' AS ' . $identifier. $alias . $identifier;
        }
        return $table;
    }
    public function quoteValue($value, $column_type=null)
    {
        try{
            if ($value === NULL) {
                return $this->pdo->quote($value,\PDO::PARAM_NULL);
            } elseif ($value === TRUE||$value === FALSE) {
                return $this->pdo->quote($value,\PDO::PARAM_BOOL);
            } elseif (is_object ( $value )) {
                return $this->quoteValue ( ( string ) $value ,$column_type);
            } elseif (is_array ( $value )) {
                return '(' . implode ( ', ', array_map ( array (
                    $this,
                    __FUNCTION__
                ), $value ) ) . ')';
            } elseif (is_int ( $value )) {
                return $this->pdo->quote($value,\PDO::PARAM_INT);
            } elseif (is_float ( $value )) {
                return $this->quote(sprintf ( '%F', $value ),\PDO::PARAM_STR);
            }
            return $this->pdo->quote($value);
        }catch (\Exception $e){//callback can't throw exception...
            return "'".addslashes($value)."'";
        }
    }
}
