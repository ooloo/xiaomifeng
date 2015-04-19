<?php

// http://www.dota2.com/jsfeed/itemdata

$content = file_get_contents("http://www.dota2.com/jsfeed/itemdata");
$obj = json_decode($content);

$items_cost = array();
$items_img = array(0=>"");
foreach($obj->itemdata as $item)
{
    print_r($item);
    $id = "$item->id";
    $pr = "$item->cost";
    $items_cost["$id"] = "$pr";
    $img = "$item->img";
    $items_img["$id"] = "$img";
}

$handle = fopen("./items_cost.php", "w+");
fwrite($handle, '<?php'.chr(10).'$items_cost='.var_export (array_unique($items_cost),true).';'.chr(10).'?>');
fclose($handle);
$handle = fopen("./items_img.php", "w+");
fwrite($handle, '<?php'.chr(10).'$items_img='.var_export (array_unique($items_img),true).';'.chr(10).'?>');
fclose($handle);
?>
