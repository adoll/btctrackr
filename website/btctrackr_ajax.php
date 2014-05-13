<?php

error_reporting(E_ALL);
ini_set('display_errors', '1');

require_once("db_functions.php");
require_once("btctrackr_functions.php");

// This is an AJAX handler for various btctrackr core functions
if((strcmp($_GET['function_name'], 'get_cluster_from_address')==0) && isset($_GET["address"]))
{
	$response = get_cluster_from_address($_GET["address"]);
	if($response["success"] == true)
	{
	    return_ajax_success("success", $response);
	    exit;
	}
	else
	{
		return_ajax_error($response["error_message"]);
		exit;
	}
}
// return error
else 
{
	return_ajax_error("There was error connecting to the server.");
}


function return_ajax_success($message='Your request processed successfully.', $response_data=array()) {

	echo(json_encode(array(
		'status' => 'success',
		'message' => $message,
		'response_data' => $response_data
	)));
	exit;

}

function return_ajax_error($message='There was error during your request.') {

	echo(json_encode(array(
		'status' => 'error',
		'message' => $message,
	)));
	exit;

}

?>