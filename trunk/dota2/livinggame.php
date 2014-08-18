<?php
include "hot.php";

$content = file_get_contents("/tmp/GetLiveLeagueGames.xml");
if(empty($content)) exit;

$arr = $hot;

$xml = simplexml_load_string($content);

foreach($xml->games->game as $game)
{
    $l = $game->league_id;
    $arr["$l"] = 1;
    foreach($game->players->player as $player)
    {
        $dbh = dba_open("/tmp/account.db", "c", "db4");
        dba_replace("$player->account_id", "$player->name", $dbh);
        dba_close($dbh);
    }
}

$key = "V001/?key=B1426000A46BD10C3FE0EAB36501A9E3&format=xml&language=zh";
$head = "https://api.steampowered.com/IDOTA2Match_570";

if(!empty($arr))
{
    foreach($arr as $id => $num)
    {
        $l_url = "\"$head/GetMatchHistory/$key&league_id=$id\"";
        if(file_exists("/tmp/$id.xml"))
        {
            $st = filemtime("/tmp/$id.xml");
            $ct = time() - 600;
            if($ct > $st)
            {
                file_put_contents("/tmp/wget.ready", "wget -t3 -T30 -O /tmp/$id.xml $l_url\n", FILE_APPEND);
            }
        }
        else
        {
            file_put_contents("/tmp/wget.ready", "wget -t3 -T30 -O /tmp/$id.xml $l_url\n", FILE_APPEND);
            continue;
        }
        $content = file_get_contents("/tmp/$id.xml");
        $xml = simplexml_load_string($content);

        $show_num = 0;
        foreach($xml->matches->match as $match)
        {
            if(++$show_num >= 10)
                break;

            $m_url = "\"$head/GetMatchDetails/$key&match_id=$match->match_id\"";
            if(!file_exists("/tmp/$match->match_id.xml"))
            {
                file_put_contents("/tmp/wget.ready", "wget -t3 -T30 -O /tmp/$match->match_id.xml $m_url\n", FILE_APPEND);
            }
        }
    }
}
?>
