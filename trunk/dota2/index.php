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

.container { width: 1030px;}
.bottom { float:left; height:200px; width:1000px; text-align:center; font-size:12px;}

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
<li class="active"><a href="http://115.29.150.73/">Home</a></li>
<li><a href="http://115.29.150.73/living.php">Live</a></li>
<li><a href="http://115.29.150.73/history.php">History</a></li>
<li><a href="http://115.29.150.73/heroes.php">Heroes</a></li>
<li><a href="http://115.29.150.73/about.php">About</a></li>
</ul>
</div><!--/.nav-collapse -->
</div>
</div>

<DIV id=m>
<DIV id=fm>
</DIV>

<?php
    include "hot.php";

    echo "<br><BR><BR><div class=\"left\">";

    $json = file_get_contents("/tmp/schedule.json");
    $array = json_decode($json, true); 
    $lastday = "";
	foreach($array[0]['list'] as $arr)
    {
        $title = $arr['title'];
        $aside = $arr['aside'];
        $bside = $arr['bside'];
        $bo = $arr['bonum'];
        $result = $arr['pointresult'];
        $endtime = $arr['gameendtime'];

        $day = split(' ',$endtime);
        $day = $day[0];
        if($result == "-") $result = "VS.";

        if($day != $lastday)
        {
            if($lastday != "")
            {
                echo "</table>";
                echo "</li></ul></div>\n";

            }
            echo "<div class=\"panel panel-primary\">";
            echo "<div class=\"panel-heading\">$day</div>\n";
            echo "<ul class=\"list-group\">\n";
            echo "<li class=\"list-group-item\">\n";
            echo "<table class=\"table\">";
            $lastday = $day;
        }
        echo "<tr><td width=20%>$endtime</td><td width=30%>$title</td><td width=10%>BO$bo</td>";
        echo "<td width=40%>$aside <font color=green><b>$result</b></font> $bside</td></tr>";
    }
    echo "</table>";
    echo "</li></ul></div>\n";

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