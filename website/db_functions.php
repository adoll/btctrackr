<?php

define("DB_USERNAME", "root");
define("DB_PASSWORD", "privacy");
define("DB_HOSTNAME", "localhost");
define("DB_NAME", "test");


function db_connect()
{
	$mysqli = new mysqli(DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_NAME);
	if ($mysqli->connect_errno) 
	{
    	//echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
		return FALSE;
	}	
	
	return $mysqli;	
}


?>
