<?php

require_once("db_functions.php");

$mysqli = db_connect();
$result = $mysqli->query("SELECT * FROM production");

while($row = $result->fetch_assoc())
{
    echo $row['address'] . '|' . $row['cluster'] . "<br />";
}

?>