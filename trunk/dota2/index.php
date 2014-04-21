<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title> Dota2直播</title>
<STYLE>HTML {
	OVERFLOW-Y: auto
}
P {
	PADDING-BOTTOM: 0px; LIST-STYLE-TYPE: none; MARGIN: 0px; PADDING-LEFT: 0px; PADDING-RIGHT: 0px; LIST-STYLE-IMAGE: none; PADDING-TOP: 0px
}
FORM {
	PADDING-BOTTOM: 0px; LIST-STYLE-TYPE: none; MARGIN: 0px; PADDING-LEFT: 0px; PADDING-RIGHT: 0px; LIST-STYLE-IMAGE: none; PADDING-TOP: 5px
}
UL {
	PADDING-BOTTOM: 0px; LIST-STYLE-TYPE: none; MARGIN: 0px; PADDING-LEFT: 0px; PADDING-RIGHT: 0px; LIST-STYLE-IMAGE: none; PADDING-TOP: 0px
}
LI {
	PADDING-BOTTOM: 0px; LIST-STYLE-TYPE: none; MARGIN: 0px; PADDING-LEFT: 0px; PADDING-RIGHT: 0px; LIST-STYLE-IMAGE: none; PADDING-TOP: 0px
}
FORM {
	POSITION: relative
}
#fm {
	width: 1000px;
	margin: 0 auto;
	margin-bottom: 10px;
	position: relative;
}
TABLE {     
	BORDER-RIGHT-WIDTH: 0px; BORDER-TOP-WIDTH: 0px; BORDER-BOTTOM-WIDTH: 0px; BORDER-LEFT-WIDTH: 0px
}       
IMG {   
	BORDER-RIGHT-WIDTH: 0px; BORDER-TOP-WIDTH: 0px; BORDER-BOTTOM-WIDTH: 0px; BORDER-LEFT-WIDTH: 0px
}           
TD {            
	LINE-HEIGHT: 18px; FONT-SIZE: 9pt
} 
A {
	COLOR: #00c
}
A:active {
	COLOR: #f60
}
#lh {
	MARGIN: 16px 0px 5px; WORD-SPACING: 3px
}
#u {
	TEXT-ALIGN: right; PADDING-BOTTOM: 3px; PADDING-LEFT: 0px; PADDING-RIGHT: 10px; PADDING-TOP: 6px
}
#m {
	MARGIN: 0px auto; WIDTH: 1000px
}
#fm {
	TEXT-ALIGN: left; PADDING-LEFT: 0px
}
#cp {
	COLOR: #77c
}
#cp A {
	COLOR: #77c
}

#navBarBG {
width: 100%;
height: auto;
background-image: url("http://www.fengyunzhibo.com/images/v3/nav-bk.png");
background-repeat: repeat-x;
position: relative;
z-index: 10000;
min-width: 1000px;
min-height: 55px;
}

#navBar{
width: 1000px;
height: 55px;
text-align: center;
margin-right: auto;
margin-left: auto;
}

#body {
width: 1000px;
height: auto;
margin: 0 auto;
position: relative;
overflow: hidden;
}

.highlightit img{ border:1px solid #fff; }
.highlightit:hover img{ border:1px solid #FFCC99; }
.highlightit:hover { color: #FFCC99; }

.head1 { float:left; height:200px; width:1000px; }
.head2 { border-left:0 none; float:left; height:230px; width:500px; }

.bottom { float:left; height:200px; width:1000px; text-align:center; font-size:12px;}

.left {
width: 780px;
height: auto;
position: relative;
float: left;
}

.item {
	float:left; height:30px; width:778px;
	border:solid 1px lightGrey;
	font-size:14px;
	color: #0054AF;
	text-align:left;
	padding-left: 10px;
	line-height: 30px;
	font-family: '微软雅黑', 'Microsoft YaHei', '黑体', Arial;
	font-weight: normal;
	background: url("http://www.zhibo8.cc/css/images/menu_bg2.gif") repeat-x;
}

.content{
border: solid 1px lightGrey;
width:788px;
height: auto;
}

.content ul {
width: 768px;
height: auto;
overflow: hidden;
padding-top: 10px;
padding-right: 10px;
padding-bottom: 10px;
padding-left: 10px;
}

.content ul li {
width:768px;
height:auto;
line-height:20px;
font-size:12px; 
padding:0 0 8px 0; 
margin:8px 0; 
border-bottom:dotted 1px #CCCCCC;
font-family:Arial,"宋体";
}

.right {
float: right;
overflow: hidden;
width: 190px;
height: auto;
padding-right: 10px;
}

.leaguebox {
	float:left; height:30px; width:188px;
	border:solid 1px lightGrey;
	position: relative;
	font-size:14px;
	color: #0054AF;
	text-align:left;
	padding-left: 10px;
	line-height: 30px;
	font-family: '微软雅黑', 'Microsoft YaHei', '黑体', Arial;
	font-weight: normal;
	background: url("http://www.zhibo8.cc/css/images/menu_bg2.gif") repeat-x;
}

.info{
border: solid 1px lightGrey;
width:198px;
height: auto;
}

.info ul {
width: 178px;
height: auto;
overflow: hidden;
padding-top: 10px;
padding-right: 10px;
padding-bottom: 10px;
padding-left: 10px;
}

.info ul li {
width:178px;
height:auto;
line-height:20px;
font-size:12px; 
padding:0 0 8px 0; 
margin:8px 0; 
border-bottom:dotted 1px #CCCCCC;
font-family:Arial,"宋体";
word-break: break-all;
word-wrap: break-word;
}

</STYLE>
</head>

<body>
<div id="navBarBG" style="display: block;">
<div id="navBar">
<font color='green' size='6'>DOTA2官方联赛直播</font>
</div>
</div>

<DIV id=m>
<DIV id=fm>
</DIV>

<?php
    include "team.php";
    echo "<div class=\"left\">";

    $content = file_get_contents("/tmp/heroes.xml");
    $xml = simplexml_load_string($content);
    $show_lastmatch_num = 0;
    $heroes_arr = array();
    foreach($xml->heroes->hero as $hero)
    {
        $heroes_arr["$hero->id"] = $hero->localized_name;
    }

    $arr = array();
    $haslive = 0;
    $content = file_get_contents("/tmp/GetLiveLeagueGames.xml");
    $xml = simplexml_load_string($content);
        
    foreach($xml->games->game as $game)
    {
        $r = $game->radiant_team->team_name;
        $d = $game->dire_team->team_name;
        $s = $game->spectators;
        $l = $game->league_id;

        $dbh = dba_open("/tmp/living.db", "c", "db4");
        $starttime = dba_fetch("$game->lobby_id", $dbh);
        if(empty($starttime))
        {
            $starttime = time();
            dba_replace($game->lobby_id, $starttime, $dbh);
        }
        dba_close($dbh);
        $d0 = date('Y-m-d H:i:s', (int)($starttime));

        $rt = $game->radiant_team->team_id;
        $dt = $game->dire_team->team_id;
        if(in_array("$rt", $team) || in_array("$dt", $team))
        {
            if(empty($arr))
            {
                echo "<div class=\"item\">Living Game</div>\n";
                echo "<div class=\"content\"><ul>\n";
                $haslive = 1;
            }
            array_push($arr, $l);
            echo "<li><table width=680 border=1>";
            echo "<tr><th width=25%><font color=blue>近卫</font></th><th width=25%><font color=red>天灾</font></th>";
            echo "<th width=30%><font color=purple>开始时间</font></th>";
            echo "<th width=10%><font color=purple>观众数</font></th><th width=10%><font color=purple>联赛id</font></th></tr>";
            echo "<tr>";
            echo "<td>$r</td><td>$d</td>";
            echo "<td>$d0</td>";
            echo "<td>$s</td>";
            echo "<td>$l</td>";
            echo "</tr></table>";
            $t0 = array();
            $t1 = array();
            foreach($game->players->player as $player)
            {
                if($player->team == "0")
                {
                    $hero = $heroes_arr["$player->hero_id"];
                    array_push($t0, "<td>$hero($player->name)</td>");
                }
                elseif($player->team == "1")
                {
                    $hero = $heroes_arr["$player->hero_id"];
                    array_push($t1, "<td>$hero($player->name)</td>");
                }
            }
            echo "<table border=1 width=680>";
            echo "<tr>";
            echo "<th width=50%>近卫</th><th width=50%>天灾</th>";
            for($i=0; $i<5; $i++)
            {
                echo "<tr>";
                echo "$t0[$i]$t1[$i]";
                echo "</tr>";
            }
            echo "</tr>";
            echo "</table></li>";
        }
    }
    if($haslive == 1)
    {
        echo "</ul></div>\n";
    }

    include "items.php";
    include "count.php";
    include "hot.php";
    include "stat.php";
    include "cost.php";

    if($haslive == 0)
    {
        echo "<div class=\"item\">最近一个月职业联赛热门英雄排行</div>\n";
        echo "<div class=\"content\">\n";
        echo "<ul><li>\n";
        echo "<table border=1 width=680>";
        echo "<tr><th width=20%>英雄</th><th width=10%>出场</th><th width=50%>热门装备</th><th width=20%>对应装备次数</th></tr>";

        $show_hero_num = 0;
        foreach($count as $hero => $picknum)
        {
            if($show_hero_num++ >= 15)
                break;
            $show_num = 0;
            $item_num = "";
            $item_arr = $stat["$hero"];
            echo "<tr><td>$hero</td><td>$picknum</td><td>";
            foreach($item_arr as $itemid => $usenum)
            {
                if($show_num > 4)
                    break;
                $t = $items_arr["$itemid"];
                $pr = $cost["$itemid"];
                if(!empty($t) && !empty($pr) && $pr > 875)
                {
                    echo "<img src='http://media.steampowered.com/apps/dota2/images/items/{$t}_lg.png' ";
                    echo "width='45'style='margin-right:2px'/>";
                    if($item_num == "")
                        $item_num = "$usenum";
                    else
                        $item_num = "$item_num/$usenum";
                    $show_num++;
                }
            }
            echo "</td><td>$item_num</td></tr>";
        }
        echo "</table>";
        echo "</li></ul></div>\n";
    }

    function show_match($matchXmlContent)
    {
        global $team;
        global $heroes_arr;
        global $show_lastmatch_num;
        $xml = simplexml_load_string($matchXmlContent);

        if(!in_array($xml->radiant_team_id, $team) && !in_array($xml->dire_team_id, $team))
            return;
        if($xml->first_blood_time == "0" || empty($xml->first_blood_time))
            return;

        $d2 = date('Y-m-d H:i:s', (int)($xml->start_time));
        echo "<div class=\"item\">{$xml->radiant_name} <font color=red>$xml->radiant_win</font> {$xml->dire_name}";
        echo "&nbsp;&nbsp;&nbsp;(联赛id:$xml->leagueid,比赛id:$xml->match_id)&nbsp;&nbsp;[$d2]</div>\n";
        echo "<div class=\"content\">\n";
        echo "<ul><li>\n";
        echo "<table border=1 width=680>";
        echo "<tr><th width=40%>英雄</th><th width=15%>击杀/死亡/助攻</th>";
        echo "<th width=10%>等级</th><th width=15%>花费金钱</th><th width=20%>每分钟金钱/经验</th></tr>";
        $dbh = dba_open("/tmp/account.db", "r", "db4");
        foreach($xml->players->player as $player)
        {
            echo "<tr>";
            $name = $heroes_arr["$player->hero_id"];
            $account_name = dba_fetch("$player->account_id", $dbh);
            if($player->player_slot < 5)
                echo "<td><font color=green size=2>$name($account_name)</font></td>";
            else
                echo "<td><font color=red size=2>$name($account_name)</font></td>";
            echo "<td>$player->kills/$player->deaths/$player->assists</td>";
            echo "<td>$player->level</td>";
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
        if($show_lastmatch_num >= 8)
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
	echo "<div class=\"leaguebox\">热门菠菜</div>";
	echo "<div class=\"info\"><ul>";
    echo "<li><a target='_blank' href='http://bet.sgamer.com/game/4'>sgamer竞猜中心</a></li>";
    echo "<li><a target='_blank' href='http://www.dota2lounge.com/'>d2l菠菜中心</a></li>";
    echo "<li><a target='_blank' href='http://www.dota2sp.com/gmatchs'>dota2sp竞猜中心</a></li>";
    echo "<li><a target='_blank' href='http://bet.replays.net/'>replays竞猜中心</a></li>";
    echo "<li><a target='_blank' href='http://moba.uuu9.com/myz_jcsg-game.html'>moba菠菜中心</a></li>";
	echo "</ul></div>";

	echo "<div class=\"leaguebox\">热门官方联赛</div>";
	echo "<div class=\"info\"><ul>";

	foreach($leagues as $league)
	{
        $l = "$league->leagueid";
        if($hot["$l"] > 20 || in_array($l, $arr))
		    $name = $league->name;
        else
            continue;

		echo "<li>";
		echo "<p><font color='green'>联赛id:$l</font></p>\n";
        echo "<a target='_blank' href='$league->tournament_url' title='$league->description'>$name</a>\n";
		echo "</li>";
	}
	echo "</ul></div></div>";
?>

<BR><BR>
<DIV class="bottom">
<p id="lh"><a href="http://www.dota2zhibo.com/">加入dota2zhibo</a> | <a href="http://www.dota2zhibo.com">dota2风云榜</a> | <a href="http://www.dota2zhibo.com">关于dota2zhibo</a> | <a href="http://www.dota2zhibo.com">About dota2zhibo</a></p><p id="cp">&copy;2013 dota2zhibo.com <a href="http://www.dota2zhibo.com">使用搜索前必读</a> <a href="http://www.miibeian.gov.cn" target="_blank">京ICP证960173号</a> <img src="http://gimg.baidu.com/img/gs.gif"></p><br>
</DIV>
</DIV>

<script>
(function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
 (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
 m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
 })(window,document,'script','//www.google-analytics.com/analytics.js','ga');
ga('create', 'UA-21342336-3', 'dota2zhibo.com');
ga('send', 'pageview');
</script>

</body>
</html>
