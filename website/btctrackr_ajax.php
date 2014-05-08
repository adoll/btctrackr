<?php

require_once("db_functions.php");

define("MAIN_TABLE_NAME", "test");

// This is an AJAX handler for various btctrackr core functions
if((strcmp($_POST['function_name'], 'get_cluster_from_address')==0) && isset($_POST["address"]))
{
	$mysqli = db_connect();
	$address = $mysqli->real_escape_string($_POST["address"]);
	$query = "SELECT cluster FROM " . MAIN_TABLE_NAME . " WHERE address = $address";
	return_ajax_success("succes", $query);
	$result = $mysqli->query($query);

	if($row = $result->fetch_assoc())
	{
	    return_ajax_success("success", $row["cluster"]);
	}

	return_ajax_error();
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