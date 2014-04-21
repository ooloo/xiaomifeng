<?php
include "items.php";
$content = file_get_contents("/tmp/heroes.xml");
$xml = simplexml_load_string($content);
$heroes_arr = array();
foreach($xml->heroes->hero as $hero)
{
    $heroes_arr["$hero->id"] = $hero->localized_name;
}
$stat = array();
$count = array();
$hot = array();
$team = array();
$seed = array("5", "7", "15", "39", "46", "111474", "726228", "1333179");

$file = file("/tmp/matches_filelist") or exit("Unable to open file!");
foreach($file as $line)
{
    $filename = str_replace("\n", "", $line);
    $content = file_get_contents("/tmp/$filename");

    $xml = simplexml_load_string($content);

    if(empty($xml->radiant_name) || empty($xml->dire_name))
        continue;
    if($xml->first_blood_time == "0" || empty($xml->first_blood_time))
        continue;

    if(in_array("$xml->radiant_team_id", $seed) || in_array("$xml->dire_team_id", $seed))
    {
        array_push($team,"$xml->radiant_team_id","$xml->dire_team_id");
    }

    array_push($hot, "$xml->leagueid");
    foreach($xml->players->player as $player)
    {
        $name = $heroes_arr["$player->hero_id"];
        if(!isset($stat["$name"]))
            $stat["$name"] = array();
        if(!isset($count["$name"]))
            $count["$name"] = "0";
        $arr = array(
                "$player->item_0",
                "$player->item_1",
                "$player->item_2",
                "$player->item_3",
                "$player->item_4",
                "$player->item_5"
                );
        foreach($arr as $item)
        {
            if($item != "0")
            {
                if(!isset($stat["$name"]["$item"]))
                    $stat["$name"]["$item"] = 1;
                else
                    $stat["$name"]["$item"] = ++$stat["$name"]["$item"];
            }
        }
        $i = $count["$name"]+1;
        $count["$name"] = $i;
        arsort($stat["$name"]);
    }
}
//print_r($stat);
arsort($count);
array_unique($team);

$handle = fopen("./stat.php", "w+");
fwrite($handle, '<?php'.chr(10).'$stat='.var_export ($stat,true).';'.chr(10).'?>');
fclose($handle);

$handle = fopen("./count.php", "w+");
fwrite($handle, '<?php'.chr(10).'$count='.var_export ($count,true).';'.chr(10).'?>');
fclose($handle);

$handle = fopen("./hot.php", "w+");
fwrite($handle, '<?php'.chr(10).'$hot='.var_export (array_count_values($hot),true).';'.chr(10).'?>');
fclose($handle);

$handle = fopen("./team.php", "w+");
fwrite($handle, '<?php'.chr(10).'$team='.var_export ($team,true).';'.chr(10).'?>');
fclose($handle);

?>
