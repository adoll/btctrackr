<?php

require_once("db_functions.php");

define("MAIN_TABLE_NAME", "test");

// This is an AJAX handler for various btctrackr core functions
if((strcmp($_POST['function_name'], 'get_cluster_from_address')==0) && isset($_POST["address"]))
{
	$return_array = array();

	$mysqli = db_connect();
	$address = $mysqli->real_escape_string($_POST["address"]);
	$query = "SELECT cluster FROM " . MAIN_TABLE_NAME . " WHERE address = '$address'";
	$result = $mysqli->query($query);

	if($row = $result->fetch_assoc())
	{
		$cluster_id = $row["cluster"];
		$query2 = "SELECT address FROM " . MAIN_TABLE_NAME . " WHERE cluster = $cluster_id";
		$result2 = $mysqli->query($query2);

		$cluster_addresses = array();
		while($row = $result->fetch_assoc())
		{
			$cluster_addresses[] = $row["address"];
		}

		$return_array["address"] = $address;
		$return_array["cluster_id"] = $row["cluster"];
		$return_array["cluster_addresses"] = $cluster_addresses;
		$return_array["cluster_btc"] = "0.0";

	    return_ajax_success("success", $return_array);
	}
	else
	{
		return_ajax_error();
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