<?php
use LSYS\Entity;
use LSYS\Entity\Validation;
use function LSYS\Model\__ as __;
use LSYS\Entity\Filter;
class DomeEntityTest extends Entity{
    public function __construct(DomeModelTest $model1){
        parent::__construct($model1);//非必须
    }
    public function labels(){
        return array(
            "enname"=>"名称"
        );
    }
    public function filterFactory() {
        return (new Filter($this))
        ->rule(new DomeFilterCallback("trim"))
        ->rule(new DomeFilterCallback("strip_tags"),"enname")
//         ->rules(array (//批量
//             new DomeFilterCallback("trim"),
//             new DomeFilterCallback("strip_tags"),
//         ),'name')
        ;
    }
    public function validationFactory(){
        return (new Validation($this))
        ->rule(new DomeVaildStrlen(1, 15, 0),"enname")
//         ->rules([//批量
//             new DomeVaildStrlen(1, 15, 0),
//         ],"name")
        ;
    }
}