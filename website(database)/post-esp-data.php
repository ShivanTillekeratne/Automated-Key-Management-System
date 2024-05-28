<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "keysystem";

/*$conn = mysqli_connect($dbServername, $dbUsername, $dbPassword, $dbName);*/

$finger_id = $key_num = $key_taken = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $finger_id = test_input($_POST["finger_id"]);
        $key_num = test_input($_POST["key_num"]);
        $key_taken = test_input($_POST["action"]);
        $date = test_input($_POST["date"]);
        $time = test_input($_POST["time"]);

        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO event (finger_id,date_,time_,key_num,key_taken)
        VALUES ('" . $finger_id . "', '" . $date_ ."','" . $time_ ."' ,'" . $key_num . "', '" . $key_taken . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}