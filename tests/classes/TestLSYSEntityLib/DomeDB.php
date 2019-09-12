<?php
namespace TestLSYSEntityLib;
use LSYS\Entity\Database;
//数据库实现,一般框架都带有,增加实现即可
class DomeDB  extends \PDO implements Database{
    protected static $obj;
    public static function instance() {
        if(!self::$obj){
            self::$obj=new self('mysql:host=127.0.0.1;dbname=test','root','110');
            self::$obj->exec('SET NAMES "utf8"');
        }
        return self::$obj;
    }
    /**
     * @param string $sql
     * @return DomeDBResult
     */
    public function query($sql){
        $res=parent::query($sql);
        if($res===false){
            $msg=$this->errorInfo();
            throw new \Exception("sql exec error:".array_shift($msg)."-".$sql);
        }
        return new DomeDBResult($res);
    }
    public function exec($sql,array $data=[])
    {
        $res=$this->prepare($sql)->execute($data);
        if($res===false){
            $msg=$this->errorInfo();
            throw new \Exception("sql exec error:".array_shift($msg)."-".$sql);
        }
        return $res;
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
                return $this->quote($value,\PDO::PARAM_NULL);
            } elseif ($value === TRUE||$value === FALSE) {
                return $this->quote($value,\PDO::PARAM_BOOL);
            } elseif (is_object ( $value )) {
                return $this->quoteValue ( ( string ) $value ,$column_type);
            } elseif (is_array ( $value )) {
                return '(' . implode ( ', ', array_map ( array (
                    $this,
                    __FUNCTION__
                ), $value ) ) . ')';
            } elseif (is_int ( $value )) {
                return $this->quote($value,\PDO::PARAM_INT);
            } elseif (is_float ( $value )) {
                return $this->quote(sprintf ( '%F', $value ),\PDO::PARAM_STR);
            }
            return $this->quote($value);
        }catch (\Exception $e){//callback can't throw exception...
            return "'".addslashes($value)."'";
        }
    }
    public function insertId()
    {
        return $this->lastInsertId();
    }
}
