<?php

require_once("db_functions.php");

define("MAIN_TABLE_NAME", "test");

$lookup_address = ""


for($i = 0; $i < len($cluster); $i = $i + 5)
{
    $url = $base_url . $cluster[$i];
    for ($j = $i + 1; $j < $j + 5; $j++) 
    {
        $url .= $cluster[$j];
    }
    $parallel_curl->startRequest($url, 'on_request_done');
}


// This should be called when you need to wait for the requests to finish.
// This will automatically run on destruct of the ParallelCurl object, so the next line is optional.
$parallel_curl->finishAllRequests();


// This function gets called back for each request that completes
function on_request_done($content, $url, $ch, $search) 
{    
    global $result_array;

    $httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);    
    if ($httpcode !== 200) {
        print "Fetch error $httpcode for '$url'\n";
        return;
    }

    $responseobject = json_decode($content, true);
    if (empty($responseobject['data'])) {
        print "No results found for '$search'\n";
        return;
    }

    foreach($responseobject["data"] as $address)
    {
        $address_name = $address["address"];
        $address_balance = $address["balance"];
        $result_array[$address_name] = $address_balance;
    }
}



?>