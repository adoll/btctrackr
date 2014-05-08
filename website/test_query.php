<?php

require_once("db_functions.php");

define("MAIN_TABLE_NAME", "test");

// This is an AJAX handler for various btctrackr core functions

	$address = "1ELmSkQWnqgbBZNzxAZHts3MEYCngqRBeD";
	$mysqli = db_connect();
	$address = $mysqli->real_escape_string($address);
	$query = "SELECT cluster FROM " . MAIN_TABLE_NAME . " WHERE address = '$address'";
	$result = $mysqli->query($query);
	var_dump($result); 
	echo "<br />";
	if($row = $result->fetch_assoc())
	{
	    echo "cluster:" . $row["cluster"];
	}

?>