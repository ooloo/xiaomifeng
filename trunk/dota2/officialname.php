<?php

$official_account = array();

$key = "v1/?key=B1426000A46BD10C3FE0EAB36501A9E3&format=xml&language=zh";
$head = "https://api.steampowered.com/IDOTA2Fantasy_570/GetPlayerOfficialInfo";

$file = file("/tmp/officialname_filelist") or exit("Unable to open file!");
foreach($file as $line)
{
    $filename = str_replace("\n", "", $line);
    $content = file_get_contents("$filename");
    $xml = simplexml_load_string($content);

    if(empty($xml->players))
    {
        continue;
    }

    foreach($xml->players->player as $player) 
    {
        $playerurl = "$head/$key&AccountID=$player->account_id";
        $official_account["$player->account_id"] = "$playerurl";
    }
}

foreach($official_account as $accid => $playerurl)
{
    $content = file_get_contents("$playerurl");

    $xml = simplexml_load_string($content);

    if(!empty($xml->Name) && !empty($xml->TeamTag))
    {
        $dbh = dba_open("/tmp/official_account.db", "c", "db4");
        dba_replace("$accid", "$xml->TeamTag.$xml->Name", $dbh);
        dba_close($dbh);
        echo "$accid $xml->TeamTag.$xml->Name\n";
    }
}

?>
