<?php
include 'connect.php';
$sql = "SELECT * FROM records";
$result = mysqli_query($conn, $sql);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="style.css">
    <meta http-equiv="refresh" content="5">
    <title>Document</title>
</head>
<body>
    
   <center>
    <H2>SECURITY RECORDS</H2>
     <table border="1">
        <tr>
            <td>id</td>
            <td>distance</td>
            <td>status</td>
            <td>time</td>
        </tr>
        <?php
        while($row = mysqli_fetch_assoc($result)){?>
            <tr>
                <td><?php echo $row['id'];?></td>
                <td><?php echo $row['distance'];?></td>
                <td><?php echo $row['status'];?></td>
                <td><?php echo $row['time'];?></td>

            </tr>
        <?php }
        ?>
    </table>
   </center>
   <h2>images from database</h2>
   <?php
   $result = mysqli_query($conn, "SELECT * FROM images");
   while($row = mysqli_fetch_assoc($result)){?>
     <img src="uploads/<?php echo $row['images'];?>">

   <?php
   }
   ?>
</body>
</html>