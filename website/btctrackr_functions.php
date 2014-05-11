<?php

error_reporting(E_ALL);
ini_set('display_errors', '1');

// This file contains core btctrackr functions
require_once("db_functions.php");

define("MAIN_TABLE_NAME", "test");

$result_array = array();

// This function queries the database to get all of the addresses associated with the cluster_id of the given address
// Parameters:
//	address: the Bitcoin address that we want to query
// Returns:
//	success: true/false depending on whether this method completed successfully
//	cluster_id: the id of the cluster this address belongs to (internal, meaningless)
//	cluster_addresses_balances: an associative array of all of the addresses in the cluster (including parameter address)
//					   this maps addresses (keys) to address balances (values)
//	cluster_btc: the total bitcoin balance of the cluster
function get_cluster_from_address($address)
{
	$return_array = array();

	$mysqli = db_connect();
	$address = $mysqli->real_escape_string($address);
	$query = "SELECT cluster FROM " . MAIN_TABLE_NAME . " WHERE address = '$address'";
	$result = $mysqli->query($query);

	if($row = $result->fetch_assoc())
	{
		$cluster_id = $row["cluster"];
		$query2 = "SELECT address FROM " . MAIN_TABLE_NAME . " WHERE cluster = $cluster_id";
		$result2 = $mysqli->query($query2);

		$cluster_addresses = array();
		while($row = $result2->fetch_assoc())
		{
			$cluster_addresses[] = $row["address"];
		}

		$cluster_addresses_balances = get_balances_from_addresses($cluster_addresses);

		$return_array["success"] = true;
		$return_array["address"] = $address;
		$return_array["cluster_id"] = $cluster_id;
		$return_array["cluster_addresses_balances"] = $cluster_addresses_balances;
		
		// calculate total value of this cluster
		$cluster_btc = 0.0;
		foreach($cluster_addresses_balances as $address => $balance)
		{	
			$cluster_btc += floatval($balance);
		}
		$return_array["cluster_btc"] = $cluster_btc;

		return $return_array;
	}
}

// This function queries the blockr.io API for Bitcoin addresses
// It does so by using curl_multi_init to concurrently send off multiple requests, to speed up the whole process for massive clusters
// Parameters:
//	addresses: an array of Bitcoin addresses we want to retrieve balances for
// Returns:
//	addresses_balances: an associative array mapping every address to a balance
function get_balances_from_addresses($addresses)
{
	require_once('parallelcurl.php');

	$base_url = "http://btc.blockr.io/api/v1/address/balance/";

	$parallel_curl = new ParallelCurl(100);

	$num_processed = 0;
	for($i = 0; $i < count($addresses); $i = $i + 30)
	{
	    $url = $base_url . $addresses[$i];
	    $num_processed += 1;
	    for ($j = $i + 1; $j < $i + 30; $j++) 
	    {
	    	if($j == count($addresses))
	    		break;
	        $url .= "," . $addresses[$j];
	        $num_processed += 1;
	    }
	    $parallel_curl->startRequest($url, 'on_request_done');
	}

	if($num_processed < count($addresses))
	{
		// do stuff here
	}

	// This should be called when you need to wait for the requests to finish.
	// This will automatically run on destruct of the ParallelCurl object, so the next line is optional.
	$parallel_curl->finishAllRequests();

	global $result_array;
	return $result_array;
}

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

    if(isset($responseobject["data"][0]))
    {
	    foreach($responseobject["data"] as $address)
	    {
	        $address_name = $address["address"];
	        $address_balance = floatval($address["balance"]);
	        $result_array[$address_name] = $address_balance;
	    }
	}
	else
	{
		$address_name = $responseobject["data"]["address"];
		$address_balance = floatval($responseobject["data"]["balance"]);
		$result_array[$address_name] = $address_balance;
	}
}


?>