<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="5" >
    <link rel="stylesheet" type="text/css" href="style.css" media="screen"/>

	<title>User key access logins</title>

</head>

<body>

    <h1>User Logins</h1>
<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "keysystem";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT `employee_name`, `time_`, `date_`, `key_num`, `employee_id`, `key_taken`
FROM `employee`
INNER JOIN `key_event` ON `employee`.`employee_id` = `key_event`.`finger_id` "; 

echo '<table>
              <tr>
              <th>Date</th>
              <th>EVENT of time</th>
              <th>Employeee ID</th>
              <th>Employee name</th>
              <th>Key hall number</th>
              <th>Action(took/stored)</th>
              </tr>';

 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_date = $row["date_"];
        $row_time = $row["time_"];
        $row_id = $row["employee_id"];
        $row_name = $row["employee_name"];
        $row_key = $row["key_num"];
        $row_action = $row["key_taken"];
        
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
       // $row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr>
                <td>' . $row_date . '</td>
                <td>' . $row_time . '</td>
                <td>' . $row_id . '</td>
                <td>' . $row_name . '</td>
                <td>' . $row_key . '</td>
                <td>' . $row_action . '</td>
             </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>

</body>
</html>