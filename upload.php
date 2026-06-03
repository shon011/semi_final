<!-- 
<?php
include 'connect.php';
$filename = "IMG_" .time() ."jpg";
file_put_contents("uploads/". $filename, file_get_contents("php://input"));
mysqli_query($conn, "INSERT INTO images(images) VALUES ('$filename')");
echo "image inserted successfully";
?> -->
<?php
include 'connect.php';
$fillename = "IMG_" .time() . "jpg";
file_put_contents("uploads/" .$filename, file_get_contents("php://input"));
mysqli_query($conn, "INSERT INTO images(images) VALUES ($filename)");
echo "image inserted successfully";

?>