<?php
namespace LSYS\Entity;
use LSYS\Entity;
class Filter{
    /**
     * @var Entity
     */
    protected $_entity;
    /**
     * @var FilterRule[][]
     */
    protected $_rules=[];
    /**
     * @var FilterRule[]
     */
    protected $_global_rules=[];
    /**
     * @var bool
     */
    protected $_allow_cache;
    public function __construct(Entity $entity,array $rule_gruop=[],$allow_cache=true){
        $this->_entity=$entity;
        $this->_allow_cache=$allow_cache;
        foreach ($rule_gruop as $field=>$rules){
            $this->rules($rules,$field);
        }
    }
    /**
     * 添加规则,$field为空为全局规则
     * @param FilterRule $rule
     * @param string $field
     * @return \LSYS\Entity\Filter
     */
    public function rule(FilterRule $rule,$field=null){
        if ($field===null){
            $this->_global_rules[]=$rule;
            return $this;
        }
        $this->_rules[$field][]=$rule;
        return $this;
    }
    /**
     * 批量添加规则,$field为空为全局规则
     * @param array $rules
     * @param string $field
     * @return \LSYS\Entity\Filter
     */
    public function rules(array $rules,$field=null)
    {
        foreach ($rules as $rule)
        {
            $this->rule($field, $rule);
        }
        return $this;
    }
    /**
     * 执行过滤
     * @param string $field
     * @param mixed $value
     * @return mixed
     */
    public function runFilter($field,$value) {
        $rule=isset($this->_rules[$field])?$this->_rules[$field]:[];
        $rule=array_merge($rule,$this->_global_rules);
        foreach ($rule as $v){
            $value=$v->filter($this->_entity,$value,$field);
        }
        return $value;
    }
    /**
     * 是否可以缓存,当过滤规则不变时设置为真
     * @return boolean
     */
    public function allowCache() {
        return $this->_allow_cache;
    }
}