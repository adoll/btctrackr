<?php

error_reporting(E_ALL);
ini_set('display_errors', '1');


//$addr1 = "12cbQLTFMXRnSzktFkuoG3eHoMeFtpTu3S";
//$addr2 = "1DUDsfc23Dv9sPMEk5RsrtfzCw5ofi5sVW";

$addr1 = $_GET["addr1"];
$addr2 = $_GET["addr2"];

chdir("/home/ubuntu/btctrackr/parser");
exec("sudo ./path $addr1 $addr2", $output);

var_dump($output);

?>