<?php
use LSYS\Entity\Table;
use LSYS\Entity\ColumnSet;
use LSYS\Entity\Column;
use LSYS\Entity\EntityColumnSet;
use LSYS\Entity\EntitySet;
class DomeModelTest implements Table{
    protected static $_table_columns;
    public function tableColumns()
    {
        if (!self::$_table_columns) {
            self::$_table_columns=new ColumnSet([
                new Column("id"),
                new Column("name"),
                new Column("enname"),
                new Column("acronym"),
                new Column("code"),
            ]);
        }
        return self::$_table_columns;
    }
    /**
     * @return DomeDB
     */
    public function db()
    {
        return DomeDB::instance();
    }
    public function tableName()
    {
        return "address";
    }
    public function primaryKey()
    {
        return 'id';
    }
    //以下非必须，根据你的业务实现方法
    private function columnSetAsStr($columns){
        if (!count($columns)) return "*";
        return implode(",", $columns->asArray(ColumnSet::TYPE_FIELD));
    }
    public function findById($id){
        $id=$this->db()->quoteValue($id, $this->tableColumns()->getType($this->primaryKey()));
        $table=$this->db()->quoteTable($this->tableName());
        $col=new EntityColumnSet(["id","name","add_time"],[new Column("enname")]);
        $cos=$this->columnSetAsStr($col->asColumnSet($this->tableColumns(),true));
        $sql="select {$cos} from {$table} where id={$id}";
        $row=$this->db()->query($sql);
        $entity=new DomeEntityTest($this);
        if($row->count()>0) $entity->loadData($row->current(),$col);
        return $entity;
    }
    public function findByWhere($where='1',$limit=10){
        $table=$this->db()->quoteTable($this->tableName());
        $row=$this->db()->query("select * from {$table} where {$where} limit {$limit}");
        return new EntitySet($row,DomeEntityTest::class,null,$this);
    }
}