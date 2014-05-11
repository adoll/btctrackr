<?php

require_once('parallelcurl.php');

$base_url = "http://btc.blockr.io/api/v1/address/balance/";
$result_array = array();

$cluster = array();
$cluster[] = "1Shremdh9tVop1gxMzJ7baHxp6XX2WWRW";
$cluster[] = "12KKNwUSevCtLzoc2vku9V3KQVYis34SXn";
$cluster[] = "18QYtHD4YWen6WfcpLCr3mffsWTEUevgJL";
$cluster[] = "18HFpwckniwq6NWr87AvGaBv5aSogo9qnp";
$cluster[] = "1HiyugiweAkjnusau2eYSdrHrqfnaxxhVP";
$cluster[] = "1Fb8S9bM5KmjbzXBNtzXSqpDcPAnZCvYPN";
$cluster[] = "1EouuXgoNYYgEWX1erVarCWZYayF8wWhuR";
$cluster[] = "1ChgTrAN8oyzdCUzWZCq5rf8tsRALzbje9";
$cluster[] = "1Abwi5PC9TLLE93iLHoVr3SNYknoRXW81m";
$cluster[] = "1HP7n77z9eTko2Rejmp4qD62m2dezai5iB";
$cluster[] = "12noPFqbvXwrGbUbS9TU6dUTGRygu9vgXa";


$parallel_curl = new ParallelCurl(10);

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

var_dump($result_array);

?>

?>