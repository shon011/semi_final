<?php
include 'connect.php';
$distance = $_POST['distance'] ?? null;
$status = $_POST['status'] ?? null;

$data = mysqli_query($conn, "INSERT INTO records(distance, status) VALUES ('$distance','$status')");
if($data){
    echo "data inserted successfully";
}
else{
    echo "failed to insert data";
}
?>