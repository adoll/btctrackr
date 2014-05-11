<?php

error_reporting(E_ALL);
ini_set('display_errors', '1');

$address = "1Shremdh9tVop1gxMzJ7baHxp6XX2WWRW";
$url = "http://btc.blockr.io/api/v1/address/balance/" . $address;

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


foreach($cluster as $address)
{
	$url .= "," . $address;
}
echo $url . "<br />";
$result = file_get_contents($url);
$object = json_decode($result);

var_dump($object);
$data = $object["data"];

echo "-------------------";

var_dump($data);

?>