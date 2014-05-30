<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

<meta charset="utf-8">
<link rel="stylesheet" href="http://cdn.bootcss.com/twitter-bootstrap/3.0.3/css/bootstrap.min.css">

<title>Dota2 直播</title>

<STYLE>
#m {
	MARGIN: 0px auto; WIDTH: 1000px
}
.left {
width: 778px;
height: auto;
position: relative;
float: left;
}

.right {
float: right;
overflow: hidden;
width: 198px;
height: auto;
padding-right: 10px;
}

.bottom { float:left; height:200px; width:1000px; text-align:center; font-size:12px;}
.container { width: 1030px;}

</STYLE>
</head>

<body>
<?php include_once("analyticstracking.php") ?>

<div class="navbar navbar-inverse navbar-fixed-top" role="navigation">
<div class="container">
<div class="navbar-header">
<button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
<span class="sr-only">Toggle navigation</span>
<span class="icon-bar"></span>
<span class="icon-bar"></span>
<span class="icon-bar"></span>
</button>
<a class="navbar-brand" href="">DOAT2 直播</a>
</div>
<div class="collapse navbar-collapse">
<ul class="nav navbar-nav">
<li><a href="http://115.29.150.73/">Home</a></li>
<li><a href="http://115.29.150.73/living.php">Live</a></li>
<li class="active"><a href="http://115.29.150.73/history.php">History</a></li>
<li><a href="http://115.29.150.73/heroes.php">Heroes</a></li>
<li><a href="http://115.29.150.73/about.php">About</a></li>
</ul>
</div><!--/.nav-collapse -->
</div>
</div>

<DIV id=m>

<?php
    include "team.php";
    include "items.php";
    include "lea.php";
    include "hot.php";

    echo "<br><BR><BR><div class=\"left\">";

    $content = file_get_contents("/tmp/heroes.xml");
    $xml = simplexml_load_string($content);
    $show_lastmatch_num = 0;
    $heroes_arr = array();
    foreach($xml->heroes->hero as $hero)
    {
        $heroes_arr["$hero->id"] = $hero->localized_name;
    }

    function show_match($matchXmlContent)
    {
        global $lea;
        global $team;
        global $heroes_arr;
        global $show_lastmatch_num;
        $xml = simplexml_load_string($matchXmlContent);

        if(!array_key_exists("$xml->radiant_team_id", $team) && !array_key_exists("$xml->dire_team_id", $team))
            return;

        if($xml->first_blood_time == "0" || empty($xml->first_blood_time))
            return;

        if(empty($xml->radiant_name) || empty($xml->dire_name))
            return;

        $ln = $lea["$xml->leagueid"];
        $d2 = date('Y-m-d H:i:s', (int)($xml->start_time));
        echo "<div class=\"panel panel-primary\">";
        echo "<div class=\"panel-heading\">{$xml->radiant_name} <font color=red>$xml->radiant_win</font> {$xml->dire_name}";
        echo "&nbsp;&nbsp;&nbsp;(联赛id:$xml->leagueid,比赛id:$xml->match_id)&nbsp;&nbsp;[$d2]</div>\n";
        echo "<ul class=\"list-group\">\n";
        echo "<li class=\"list-group-item\">\n";
        echo "<table class=\"table\">";
        echo "<tr><th width=35%><font color=green>$ln</font></th>";
        echo "<th width=10%>等级</th>";
        echo "<th width=20%>击杀/死亡/助攻</th>";
        echo "<th width=15%>花费金钱</th><th width=20%>每分钟金钱/经验</th></tr>";
        $dbh = dba_open("/tmp/account.db", "r", "db4");
        foreach($xml->players->player as $player)
        {
            echo "<tr>";
            $name = $heroes_arr["$player->hero_id"];
            $account_name = dba_fetch("$player->account_id", $dbh);
            if($player->player_slot < 5)
                echo "<td>$name<font color=blue size=2>($account_name)</font></td>";
            else
                echo "<td>$name<font color=red size=2>($account_name)</font></td>";
            echo "<td>$player->level</td>";
            echo "<td>$player->kills/$player->deaths/$player->assists</td>";
            echo "<td>$player->gold_spent</td>";
            echo "<td>$player->gold_per_min/$player->xp_per_min</td></tr>";
        }
        dba_close($dbh);
        echo "</table></li></ul></div>\n";
        $show_lastmatch_num++;
    }

    $file = file("/tmp/matches_filelist") or exit("Unable to open file!");
    foreach($file as $line)
    {
        if($show_lastmatch_num >= 10)
            break;

        $filename = str_replace("\n", "", $line);
        $content = file_get_contents("/tmp/$filename");
        show_match($content);
    }

    // -------------left end--------------
    echo "</div>\n";

    $content = file_get_contents("/tmp/GetLeagueListing.xml");
	$xml = simplexml_load_string($content);
	$leagues = $xml->leagues[0];

    echo "<div class=\"right\">";
    echo "<div class=\"panel panel-primary\">";
    echo "<div class=\"panel-heading\">热门菠菜</div>\n";
    echo "<ul class=\"list-group\">\n";
    echo "<li class=\"list-group-item\"><a target='_blank' href='http://bet.sgamer.com/game/4'>sgamer竞猜中心</a></li>";
    echo "<li class=\"list-group-item\"><a target='_blank' href='http://www.dota2lounge.com/'>d2l菠菜中心</a></li>";
    echo "<li class=\"list-group-item\"><a target='_blank' href='http://www.dota2sp.com/gmatchs'>dota2sp竞猜中心</a></li>";
    echo "<li class=\"list-group-item\"><a target='_blank' href='http://bet.replays.net/'>replays竞猜中心</a></li>";
    echo "<li class=\"list-group-item\"><a target='_blank' href='http://moba.uuu9.com/myz_jcsg-game.html'>moba菠菜中心</a></li>";
	echo "</ul></div>";

    echo "<div class=\"panel panel-primary\">";
    echo "<div class=\"panel-heading\">热门官方联赛</div>\n";
    echo "<ul class=\"list-group\">\n";

	foreach($leagues as $league)
	{
        $l = "$league->leagueid";
        if($hot["$l"] >= 1)
		    $name = $league->name;
        else
            continue;

		echo "<li class=\"list-group-item\">";
		echo "<p><font color='green'>联赛id:$l</font></p>\n";
        //echo "<a target='_blank' href='$league->tournament_url' title='$league->description'>$name</a>\n";
        echo "<a target='_blank' href='$league->tournament_url'>$name</a>\n";
		echo "</li>";
	}
	echo "</ul></div></div>";
?>

<BR><BR>
<DIV class="bottom">
<p id="lh"><a href="http://www.dota2zhibo.com/">加入dota2zhibo</a> | <a href="http://www.dota2zhibo.com">dota2风云榜</a> | <a href="http://www.dota2zhibo.com">关于dota2zhibo</a> | <a href="http://www.dota2zhibo.com">About dota2zhibo</a></p><p id="cp">&copy;2014 dota2zhibo.com <a href="http://www.dota2zhibo.com">使用搜索前必读</a> <a href="http://www.miibeian.gov.cn" target="_blank">京ICP证960173号</a> <img src="http://gimg.baidu.com/img/gs.gif"></p><br>
</DIV>
</DIV>

<script src="http://cdn.bootcss.com/jquery/1.10.2/jquery.min.js"></script>
<script src="../../dist/js/bootstrap.min.js"></script>

</body>
</html>
