<?php
$content = file_get_contents("itemdata");
$obj = json_decode($content);

$cost = array();
foreach($obj->itemdata as $item)
{
    print_r($item);
    $id = "$item->id";
    $pr = "$item->cost";
    $cost["$id"] = "$pr";
}

$handle = fopen("./cost.php", "w+");
fwrite($handle, '<?php'.chr(10).'$cost='.var_export (array_unique($cost),true).';'.chr(10).'?>');
fclose($handle);
?>
