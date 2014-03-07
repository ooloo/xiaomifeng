<?php
$content = file_get_contents("/tmp/GetLiveLeagueGames.xml");
$xml = simplexml_load_string($content);

$arr = array();
foreach($xml->games->game as $game)
{
    $l = $game->league_id;
    array_push($arr, $l);
    $dbh = dba_open("/tmp/account.db", "c", "db4");
    foreach($game->players->player as $player)
    {
        dba_replace($player->account_id, $player->name, $dbh);
    }
    dba_close($dbh);
}
array_push($arr, "189");

$key = "V001/?key=B1426000A46BD10C3FE0EAB36501A9E3&format=xml&language=zh";
$head = "https://api.steampowered.com/IDOTA2Match_570";

if(!empty($arr))
{
    foreach($arr as $id)
    {
        $l_url = "\"$head/GetMatchHistory/$key&league_id=$id\"";
        if(file_exists("/tmp/$id.xml"))
        {
            $st = filemtime("/tmp/$id.xml");
            $ct = time()-1800;
            if($ct > $st)
            {
                file_put_contents("/tmp/wget.ready", "wget -O /tmp/$id.xml $l_url\n", FILE_APPEND);
            }
        }
        else
        {
            file_put_contents("/tmp/wget.ready", "wget -O /tmp/$id.xml $l_url\n", FILE_APPEND);
            continue;
        }
        $content = file_get_contents("/tmp/$id.xml");
        $xml = simplexml_load_string($content);

        $show_num = 0;
        foreach($xml->matches->match as $match)
        {
            if(++$show_num > 10)
                break;

            $m_url = "\"$head/GetMatchDetails/$key&match_id=$match->match_id\"";
            if(!file_exists("/tmp/$match->match_id.xml"))
            {
                file_put_contents("/tmp/wget.ready", "wget -O /tmp/$match->match_id.xml $m_url\n", FILE_APPEND);
            }
        }
    }
}
?>
