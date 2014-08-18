<?php

$file = file("/tmp/officialname") or exit("Unable to open file!");
foreach($file as $line)
{
    list($accid, $url) = explode("\t", $line);
    $content = file_get_contents("$url");

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
