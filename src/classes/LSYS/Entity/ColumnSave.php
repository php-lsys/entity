<?php
namespace LSYS\Entity;
/**
 * 保存时回调字段
 */
interface ColumnSave{
    /**
     * 更新时回调
     * @param Entity $entity
     * @param string $column
     */
   public function update(Entity $entity,$column);
   /**
    * 创建时回调
    * @param Entity $entity
    * @param string $column
    */
   public function create(Entity $entity,$column);
}