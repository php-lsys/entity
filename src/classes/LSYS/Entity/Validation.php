<?php
namespace LSYS\Entity;
use LSYS\Entity;
class Validation {
    /**
     * @var Entity
     */
    protected $_entity;
	protected $_rules = array();
	protected $_global_rules = array();
	protected $_labels = array();
	protected $_errors = array();
	protected $_allow_cache;
	/**
	 * 数据校验对象
	 * @param Entity $entity
	 * @param array $rule_group
	 * @param boolean $allow_cache
	 */
	public function __construct(Entity $entity,array $rule_group=array(),$allow_cache=true)
	{
	    $this->_entity=$entity;
	    $this->_allow_cache=$allow_cache;
	    $columns=$entity->columns(false)->asArray(ColumnSet::TYPE_FIELD);
	    $this->_labels=$entity->labels ();
	    $this->_labels = array_merge ( array_combine ($columns , $columns), is_array($this->_labels)?$this->_labels:[] );
	    foreach ($rule_group as $field=>$rules){
	        $this->rules($rules,$field);
	    }
	}
	/**
	 * 是否可以缓存校验对象
	 * @return boolean
	 */
	public function allowCache() {
	    return $this->_allow_cache;
	}
	/**
	 * 校验字段翻译名称
	 * @param string $field
	 * @param string $label
	 * @return \LSYS\Entity\Validation
	 */
	public function label($field, $label)
	{
		$this->_labels[$field] = $label;
		return $this;
	}
	/**
	 * 批量设置校验字段名称
	 * @param array $labels
	 * @return \LSYS\Entity\Validation
	 */
	public function labels(array $labels)
	{
		$this->_labels = $labels + $this->_labels;
		return $this;
	}
	/**
	 * 校验规则设置
	 * @param ValidRule $rule
	 * @param string $field 设置为null为全局校验
	 * @return \LSYS\Entity\Validation
	 */
	public function rule(ValidRule $rule,$field=null)
	 {
	     if ($field===null) {
	        $this->_global_rules[]=$rule;
	        return $this;
	    }
	 	if (!isset($this->_labels[$field]))
	 	{
	 		$this->_labels[$field] = $field;
	 	}
	 	$this->_rules[$field][] = $rule;
	 	return $this;
	 }
	 /**
	  * 批量设置校验规则
	  * @param array $rules
	  * @param string $field
	  * @return \LSYS\Entity\Validation
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
	  * 进行校验
	  * @param array $check_data
	  * @return bool
	  */
	 public function valid(array $check_data)
	  {
	  	$this->_errors = array();
	  	$expected=array_keys($check_data);
	  	$rules = $this->_rules;
	  	if(count($this->_global_rules))foreach ($expected as $field)
	  	{
	  	        if ( ! isset($rules[$field]))
	  	        {
	  	            $rules[$field] = array();
	  	        }
	  	        $rules[$field] = array_merge($rules[$field], $this->_global_rules);
	  	}
	  	foreach ($rules as $field => $set)
	  	{
	  		foreach ($set as $rule)
	  		{
	  		    $value=isset($check_data[$field])?$check_data[$field]:null;
	  		    if($rule->allowEmpty()&&in_array($value, array(null,false,'',array()), TRUE)){//可以为空
	  		        continue;
	  		    }
	  		    $rule->check($this,$field,$value,$this->_labels[$field],$this->_entity,$check_data);
	  		    if (isset($this->_errors[$field])) {
	  		        break;
	  		    }
	  		}
	  	}
	  	return empty($this->_errors);
	  }
	  /**
	   * 设置校验错误(校验方法里调用)
	   * @param string $field
	   * @param string $error
	   * @return \LSYS\Entity\Validation
	   */
	  public function error($field, $error)
	  {
	  	$this->_errors[$field] = $error;
	  	return $this;
	  }
	  /**
	   * 得到校验错误
	   * @return array
	   */
	  public function errors()
	  {
  		 return $this->_errors;
	  }
}
