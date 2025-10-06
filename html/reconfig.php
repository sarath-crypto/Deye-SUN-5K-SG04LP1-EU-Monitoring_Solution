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

	$dir_max = 0;
	$sip = 0;
	$access = 0;
	$sn = 0;
	$pkey = 0;
	$mkey = 0;
	$rb = 0;
	$wled = 0;
	$mask = 0;
	$x = -1;
	$y = -1;
	$w = -1;
	$h = -1;
	$id = 0;

	if ($_SERVER["REQUEST_METHOD"] == "POST") {
		$dir_max = htmlspecialchars($_POST['dir_max']);
		$access = htmlspecialchars($_POST['access']);
		$sip = htmlspecialchars($_POST['sip']);
		$sn = htmlspecialchars($_POST['sn']);
		$pkey = htmlspecialchars($_POST['pkey']);
		$mkey = htmlspecialchars($_POST['mkey']);
		$rb = htmlspecialchars($_POST['reboot']);
		$wled = htmlspecialchars($_POST['wled']);
		$mask = htmlspecialchars($_POST['mask']);
		$mip = htmlspecialchars($_POST['mip']);
		$phr = htmlspecialchars($_POST['phr']);
		$x =  htmlspecialchars($_POST['x']);
		$y =  htmlspecialchars($_POST['y']);
		$w =  htmlspecialchars($_POST['w']);
		$h =  htmlspecialchars($_POST['h']);
		$id =  htmlspecialchars($_POST['id']);
	}
        if($_SESSION['auth']){
                $conn = new mysqli('localhost','userpwrsys','pwrsys123','pwrsys');
                if ($conn->connect_error) {
                        die("Connection failed: " . $conn->connect_error);
		}else{
			$sql = "UPDATE cfg SET ";
			if($dir_max >= 0)$sql = $sql."dir_max=".$dir_max.",";	
			if($access)$sql = $sql."access='".$access."',";	
			if($sip)$sql = $sql."sip='".$sip."',";	
			if($sn)$sql = $sql."sn='".$sn."',";	
			if($pkey)$sql = $sql."pkey='".$pkey."',";	
			if($mkey)$sql = $sql."mkey='".$mkey."',";	
			if($mip)$sql = $sql."mip='".$mip."',";	
			if($phr)$sql = $sql."phr='".$phr."',";	
			if($rb == "yes")$sql = $sql."rb=1,";	
			else $sql = $sql."rb=0,";	
			if($wled == "on")$sql = $sql."wled=1,";
			else $sql = $sql."wled=0,";	
			$sql = rtrim($sql,",");
			$result = $conn->query($sql);
			if(($mask == "Add") && ($x >= 0) && ($y >= 0) && ($w >= 0) && ($h >= 0)){
				$sql = "INSERT INTO mask SET ";
				$sql = $sql."x=".$x.",y=".$y.",w=".$w.",h=".$h.",";	
				$sql = rtrim($sql,",");
				$result = $conn->query($sql);

			}else if($id){
				$sql = "DELETE FROM mask WHERE id=".$id;
				$result = $conn->query($sql);
			}
			sleep(5);
			header("Location: config.php");
		}
		$conn->close();
	}
?>
