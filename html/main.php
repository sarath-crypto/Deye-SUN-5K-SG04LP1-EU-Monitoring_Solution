<?php 

function drawLineGraph_solar($cachefilename,$pdprod,$pdload,$pdbuy,$psoc,$puload,$pgload,$pprod,$pgvolt,$pgdexp,$pgexp,$px){
	require_once ('jpgraph/jpgraph.php');
	require_once ('jpgraph/jpgraph_line.php');

	$graph = new Graph(1500,400);
	$graph->SetScale("textlin");
	$theme_class=new UniversalTheme;
	$graph->SetTheme($theme_class);
	$graph->img->SetAntiAliasing(false);
	$graph->title->Set($title);
	$graph->SetBox(false);

	$graph->SetMargin(40,20,36,63);
	$graph->img->SetAntiAliasing();
	$graph->yaxis->HideZeroLabel();
	$graph->yaxis->HideLine(false);
	$graph->yaxis->HideTicks(false,false);

	$graph->xgrid->Show();
	$graph->xgrid->SetLineStyle("solid");
	$graph->xaxis->SetTickLabels($px);
        $graph->xaxis->SetLabelAngle(90);
	$graph->xgrid->SetColor('#E3E3E3');

	$valid = $graph -> cache -> IsValid($cachefilename);
	if ($valid){
                return;
        }else{
		$graph->SetupCache($cachefilename, 1);
		$graph->legend->SetPos(0.33,0,'centre','top');

		$p1 = new LinePlot($pdprod);
		$graph->Add($p1);
		$p1->SetWeight(3);
		$p1->SetFillFromYMin(false);
		$p1->SetColor("#00FFFF");
		$val = end($pdprod)*100;
		$p1->SetLegend('Daily Production x100['.$val.'Wh]');

		$p2 = new LinePlot($pdload);
		$graph->Add($p2);
		$p2->SetWeight(3);
		$p2->SetFillFromYMin(false);
		$p2->SetColor("#800000");
		$val = end($pdload)*100;
		$p2->SetLegend('Daily Load x100['.$val.'Wh]');

		$p3 = new LinePlot($pdbuy);
		$graph->Add($p3);
		$p3->SetWeight(3);
		$p3->SetFillFromYMin(false);
		$p3->SetColor("#FF00FF");
		$val = end($pdbuy)*100;
		$p3->SetLegend('Daily Buy x100['.$val.'Wh]');

		$p4 = new LinePlot($psoc);
		$graph->Add($p4);
		$p4->SetStyle("dashed");
		$p4->SetWeight(3);
		$p4->SetFillFromYMin(false);
		$p4->SetColor("#f1c40f");
		$val = end($psoc);
		$p4->SetLegend('SOC '.$val.'%');

		$p5 = new LinePlot($puload);
		$graph->Add($p5);
		$p5->SetStyle("dashed");
		$p5->SetWeight(3);
		$p5->SetFillFromYMin(false);
		$p5->SetColor("#00008B");
		$val = end($puload)*10;
		$p5->SetLegend('Load x100['.$val.'W]');
	
		$p6 = new LinePlot($pgload);
		$graph->Add($p6);
		$p6->SetStyle("dashed");
		$p6->SetWeight(3);
		$p6->SetFillFromYMin(false);
		$p6->SetColor("#800080");
		$val = end($pgload)*10;
		$p6->SetLegend('Grid Load x100['.$val.'W]');

		$p7 = new LinePlot($pprod);
		$graph->Add($p7);
		$p7->SetStyle("dashed");
		$p7->SetWeight(3);
		$p7->SetFillFromYMin(false);
		$p7->SetColor("#7fff00");
		$val = end($pprod)*10;
		$p7->SetLegend('Production x10['.$val.'W]');

		$p8 = new LinePlot($pgvolt);
		$graph->Add($p8);
		$p8->SetStyle("dashed");
		$p8->SetWeight(3);
		$p8->SetFillFromYMin(false);
		$p8->SetColor("#ff0000");
		$val = end($pgvolt);
		$p8->SetLegend('Grid['.$val.'v]');

		$p9 = new LinePlot($pgdexp);
		$graph->Add($p9);
		$p9->SetWeight(3);
		$p9->SetFillFromYMin(false);
		$p9->SetColor("#008000");
		$p9->SetLegend('Daily Export Grid');

		$p10 = new LinePlot($pgexp);
		$graph->Add($p10);
		$p10->SetStyle("dashed");
		$p10->SetWeight(3);
		$p10->SetFillFromYMin(false);
		$p10->SetColor("#808000");
		$p10->SetLegend('Export Grid');

		$graph->legend->SetFrameWeight(1);
                $absolutePath = (CACHE_DIR . "" . $cachefilename);
		$graph -> Stroke($absolutePath);
	}
}

function drawLineGraph_sensor($cachefilename,$ptemp_a,$ptemp_b,$ppress,$palt,$paqi,$pbat,$prdr,$ptherm,$pwl,$px){
	require_once ('jpgraph/jpgraph.php');
	require_once ('jpgraph/jpgraph_line.php');

	$graph = new Graph(1500,400);

	$graph->SetScale("textlin");
	$theme_class=new UniversalTheme;
	$graph->SetTheme($theme_class);
	$graph->img->SetAntiAliasing(false);
	$graph->title->Set($title);
	$graph->SetBox(false);

	$graph->SetMargin(40,20,36,63);
	$graph->img->SetAntiAliasing();
	$graph->yaxis->HideZeroLabel();
	$graph->yaxis->HideLine(false);
	$graph->yaxis->HideTicks(false,false);

	$graph->xgrid->Show();
	$graph->xgrid->SetLineStyle("solid");
	$graph->xaxis->SetTickLabels($px);
        $graph->xaxis->SetLabelAngle(90);
	$graph->xgrid->SetColor('#E3E3E3');

	$valid = $graph -> cache -> IsValid($cachefilename);
	if ($valid){
                return;
        }else{
		$graph -> SetupCache($cachefilename, 1);
		$graph->legend->SetPos(0.33,0,'centre','top');

		$p1 = new LinePlot($ptemp_a);
		$graph->Add($p1);
		$p1->SetWeight(3);
		$p1->SetColor("#f44336");
		$val = end($ptemp_a);
		$p1->SetLegend('TemperatureA ['.$val.'c]');
			
		$p2 = new LinePlot($ptemp_b);
		$graph->Add($p2);
		$p2->SetWeight(3);
		$p2->SetColor("#ff5722");
		$val = end($ptemp_b);
		$p2->SetLegend('TemperatureB ['.$val.'c]');

		$p3 = new LinePlot($ppress);
		$graph->Add($p3);
		$p3->SetWeight(3);
		$p3->SetColor("#58d68d");
		$val = end($ppress)*10;
		$p3->SetLegend('Pressure x10 ['.$val.'hPa/mB]');

		$p4 = new LinePlot($palt);
		$graph->Add($p4);
		$p4->SetWeight(3);
		$p4->SetColor("#76448a");
		$val = end($palt);
		$p4->SetLegend('Altitude ['.$val.'Ft]');

		$p5 = new LinePlot($paqi);
		$graph->Add($p5);
		$p5->SetWeight(3);
		$p5->SetColor("#f1c40f");
		$val = end($paqi);
		$p5->SetLegend('Air Quality ['.$val.']');

		$p6 = new LinePlot($pbat);
		$graph->Add($p6);
		$p6->SetWeight(3);
		$p6->SetColor("#d35400");
		$val = end($pbat);
		$p6->SetLegend('Battery ['.$val.'v]');

		$p7 = new LinePlot($prdr);
		$graph->Add($p7);
		$p7->SetWeight(3);
		$p7->SetColor("#ec7063");
		$val = end($prdr);
		$p7->SetLegend('Radar /60 ['.$val.']');
		
		$p8 = new LinePlot($ptherm);
		$graph->Add($p8);
		$p8->SetWeight(3);
		$p8->SetColor("#f505cb");
		$val = end($ptherm)*10;
		$p8->SetLegend('Thermistor x10 ['.$val.']');

		$p9 = new LinePlot($pwl);
		$graph->Add($p9);
		$p9->SetWeight(3);
		$p9->SetColor("#27cff5");
		$val = end($pwl)*2;
		$p9->SetLegend('Water Level /2 ['.$val.']');

		$graph->legend->SetFrameWeight(1);
                $absolutePath = (CACHE_DIR . "" . $cachefilename);
		$graph -> Stroke($absolutePath);
	}
}

function drawPieGraph($cachefilename, $ydata){
        require_once ('jpgraph/jpgraph.php');
	require_once ('jpgraph/jpgraph_pie.php');
	$graph = new PieGraph(200,200);
        $valid = $graph -> cache -> IsValid($cachefilename);
	if ($valid){
                return;
        }else{
		$graph -> SetupCache($cachefilename, 1);
                $graph->clearTheme();
		$graph->title->Set("Disk Usage");
		$pie = new PiePlot($ydata);
		$colors = array('#FF0000', '#00FF00');
		$pie->SetSliceColors($colors);
                $graph->Add($pie);
                $absolutePath = (CACHE_DIR . "" . $cachefilename);
                $graph -> Stroke($absolutePath);
	}
}

	session_start();
	if ($_SERVER["REQUEST_METHOD"] == "POST") {
		$pass = htmlspecialchars($_POST['pass_word']);
		if (empty($pass) == FALSE){
			$conn = new mysqli('localhost','userpwrsys','pwrsys123','pwrsys');
			if ($conn->connect_error) {
				die("Connection failed: " . $conn->connect_error);
			}else{
				$sql = "SELECT access FROM cfg";	
				$result = $conn->query($sql);
				$dbpass = "";
				if ($result->num_rows > 0) {
					$row = $result->fetch_assoc();
					$dbpass = $row['access'];
				}
				if($pass == $dbpass){
					$_SESSION['auth'] = true;
				}
				$conn->close();
			}
		}
	}
	if($_SESSION['auth']){
		header("Refresh: 1");
		$conn = new mysqli('localhost','userpwrsys','pwrsys123','pwrsys');
		if ($conn->connect_error) {
			die("Connection failed: " . $conn->connect_error);
		}else{

			echo '<html><body>';
                        echo '<table>';
			echo '<tr style="height:200px;text-align: center; vertical-align: middle;">';
                        echo '<td width="200px">';
			$sql = "SELECT ts,data FROM out_img";	
			$result = $conn->query($sql);
			if ($result->num_rows > 0) {
				while($row = $result->fetch_assoc()) {
					echo '<div class="caption"><img src="data:image/jpeg;base64,'.base64_encode($row['data']).'"/></div>';
					$time=$row['ts'];
				}						
			}
                        echo '</td>';
			
                        echo '<td width="200px">';
			$sql = "SELECT data FROM out_tig";	
			$result = $conn->query($sql);
			if ($result->num_rows > 0) {
				while($row = $result->fetch_assoc()) {
					echo '<div class="caption"><img src="data:image/jpeg;base64,'.base64_encode($row['data']).'"/></div>';
				}						
			}
			echo '</td>';

  			echo '<td width="200px">';
			$sql = "SELECT data FROM out_tim";
			$result = $conn->query($sql);
			if ($result->num_rows > 0) {
				while($row = $result->fetch_assoc()) {
					echo '<div class="caption"><img src="data:image/jpeg;base64,'.base64_encode($row['data']).'"/></div>';
				}						
			}
			echo '</td>';
			echo '<td width="260px">';
			echo $time;
			
			$times = posix_times();
			$now = $times['ticks'];
			$days = intval($now / (60*60*24*100));
			$remainder = $now % (60*60*24*100);
			$hours = intval($remainder / (60*60*100));
			$remainder = $remainder % (60*60*100);
			$minutes = intval($remainder / (60*100));
			echo "<br>[".$days."]Days [".$hours."]Hours [".$minutes."]Minutes";
			echo '<br><a href="summary.php">Show Summary</a>';
			echo '<br><a href="config.php">Show Configuration</a>';

		
			$df = disk_free_space("/");
			$ds = disk_total_space("/");
			$du = ($df/$ds)*100;
			$ydata = array(100-$du,$du);
			$f_pie = 'graph/pie.png';
			$graph = drawPieGraph($f_pie, $ydata);
		
			echo '<img style="vertical-align: bottom;"  width="100%" height="100%" src=';
			echo $f_pie;
			echo '></img></td>';
			echo '</tr></table>';
			
			date_default_timezone_set('Asia/Kolkata');
			$timezone = date_default_timezone_get();
			$sql = "select * from hour";
			$result = $conn->query($sql);
			if ($result->num_rows > 0) {
				while($row = $result->fetch_assoc()) {
					$temp_a[] = $row["temp_a"]; 
					$temp_b[] = $row["temp_b"]; 
					$press[] = $row["press"]/10;
					$alt[] = $row["alt"];
					$aqi[] = $row["aqi"];
					$bat[] = $row["bat"];
					$rdr[] = $row["rdr"]*60;
					$therm[] = $row["therm"]/10;
					$wl[] = $row["wl"]/2;
					$dprod[] = $row["dprod"];
					$dload[] = $row["dload"];
					$dbuy[] = $row["dbuy"];
					$soc[] = $row["soc"];
					$uload[] = $row["uload"]/10;
					$gload[] = $row["gload"]/10;
					$prod[] = $row["prod"]/10;
					$gvolt[] = $row["gvolt"]/10;
					$gdexp[] = $row["gdexp"];
					$gexp[] = $row["gexp"];
					$ts[] = $row["ts"]; 
				}						
			}
			$f_solar = 'graph/solar.png';
			$graph = drawLineGraph_solar($f_solar,$dprod,$dload,$dbuy,$soc,$uload,$gload,$prod,$gvolt,$gdexp,$gexp,$ts);
			echo '<table><tr>';
			echo '<td><img style="vertical-align: bottom;" src=';
			echo $f_solar;
			echo '></img></td></tr>';
				
			$f_sensor = 'graph/sensor.png';
			$graph = drawLineGraph_sensor($f_sensor,$temp_a,$temp_b,$press,$alt,$aqi,$bat,$rdr,$therm,$wl,$ts);
			echo '<tr><td>';
			echo '<img style="vertical-align: bottom;" src=';
			echo $f_sensor;
			echo '></img></td></tr>';
			
			echo '</table>';
			echo '</body>';
			$conn->close();
		}
	}else{
		echo '<meta http-equiv = "refresh" content = "0; url = index.php" />';
	}
?>
