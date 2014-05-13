<?php

error_reporting(E_ALL);
ini_set('display_errors', '1');


$addr1 = "12cbQLTFMXRnSzktFkuoG3eHoMeFtpTu3S";
$addr2 = "1DUDsfc23Dv9sPMEk5RsrtfzCw5ofi5sVW";

shell_exec("cd /home/ubuntu/btctrackr/parser");
exec("./path $addr1 $addr2", $output);

var_dump($output);

?>