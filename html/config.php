<?php
	session_start();
	if(!isset($_SESSION['auth'])){
		header("Location: index.php");
		exit();
	}
	if($_SESSION['auth'] == 0){
		header("Location: index.php");
		exit();
	}
	
	echo '<!DOCTYPE html><html><head><style>table,th,td{border: 1px solid white;border-collapse: collapse;}th,td{background-color: #96D4D4;}</style></head><body>';
	echo "<a href='main.php'>Back to main page</a>";

	$conn = new mysqli('localhost','userpwrsys','pwrsys123','pwrsys');
        if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error);
        }else{
		$sql = "SELECT * FROM cfg";
		$result = $conn->query($sql);
		if ($result->num_rows > 0) {
			while($row = $result->fetch_assoc()){
        			$dir_max = $row['dir_max'];
        			$ip = $row['ip'];
				$access = $row['access'];
				$sn = $row['sn'];
			}
			echo '<table><tr>';
			echo '<th style="color:red">Configurations</th>';
			echo '</tr>';
			echo '<form action="reconfig.php" method="POST" id="action-form">';
			echo '<table>';
			echo '<tr width=100%><td>DIR_MAX[1-255]</td><td><input type="number" id="dir_max" name="dir_max" value="';
			echo $dir_max;
			echo '"></td></tr>';
			echo '<tr width=100%><td>IP Address</td><td><input type="text" id="ip" name="ip" value="';
			echo $ip;
			echo '"></td></tr>';
			echo '<tr width=100%><td>Serial Number</td><td><input type="text" id="sn" name="sn" value="';
			echo $sn;
			echo '"></td></tr>';
			echo '<tr width=100%><td>ACCESS</td><td><input type="text" id="access" name="access" value="';
			echo $access;
			echo '"></td></tr>';
			echo '<tr><td>';
			echo '<label for="reboot">Reboot</label>
				<select name="reboot" id="reboot">
				<option value="no">No</option>
				<option value="yes">Yes</option>
				</select>';
			echo '</td><td>';
			echo '<label for="wled">WLED</label>
				<select name="wled" id="wled">
				<option value="off">OFF</option>
				<option value="on">ON</option>
				</select>';
			echo '<label for="mask">MASK</label>
				<select name="mask" id="mask">
				<option value="Add">Add</option>
				<option value="Remove">Remove</option>
				</select>';
			echo '</table>';

			$sql = "SELECT * FROM mask";
			$result = $conn->query($sql);
			if ($result->num_rows > 0) {
				echo '<table>';
				echo '<th style="color:blue">MASKS</th>';
				while($row = $result->fetch_assoc()) {
					echo '<tr><td>ID</td><td>';
					echo $row['id'];
					echo '</td><td>X</td><td>';
					echo $row['x'];
					echo '</td><td>Y</td><td>';
					echo $row['y'];
					echo '</td><td>W</td><td>';
					echo $row['w'];
					echo '</td><td>H</td><td>';
					echo $row['h'];
					echo '</td></tr>';
				}
				echo '</table>';
			}
			 
			echo '<table>';
			echo '<tr width=100%><td>ID</td><td><input type="number" id="id" name="id"></td></tr>';
			echo '<tr width=100%><td>X</td><td><input type="number" id="x" name="x"></td></tr>';
			echo '<tr width=100%><td>Y</td><td><input type="number" id="y" name="y"></td></tr>';
			echo '<tr width=100%><td>W</td><td><input type="number" id="w" name="w"></td></tr>';
			echo '<tr width=100%><td>H</td><td><input type="number" id="h" name="h"></td>';
			echo '<td><input type="submit" value="Submit"></td></tr>';
			echo '</table>';

			echo '</form>';
		}

		echo '<canvas id="myCanvas" width="360" height="360" style="border:1px solid black">Browser does not support canvas</canvas>';
		echo '<script> const canvas = document.getElementById("myCanvas");';
		echo 'const ctx = canvas.getContext("2d");';

		$sql = "SELECT * FROM mask";
		$result = $conn->query($sql);
		if ($result->num_rows > 0) {
			while($row = $result->fetch_assoc()){
 				$x = $row['x'];
        			$y = $row['y'];
        			$w = $row['w'];
				$h = $row['h'];
				echo 'ctx.fillStyle = "blue";';
				echo 'ctx.fillRect('.$x.','.$y.','.$w.','.$h.');';
			}
		}

                echo 'ctx.beginPath();';
                echo 'ctx.moveTo(0,180);';
		echo 'ctx.lineTo(360,180);';
             	echo 'ctx.moveTo(180,0);';
                echo 'ctx.lineTo(180,360);';
                echo 'ctx.stroke();';

                echo 'ctx.font = "20px Arial ";';
                echo 'ctx.fillStyle = "red";';
                echo 'ctx.fillText("180",180,200,50);';
		
		echo '</script>';

		$conn->close();
	}
?>
