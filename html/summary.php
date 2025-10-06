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
	
	$dirs = array();
	$dir = "data/";

	$dirs = glob("data/*",GLOB_ONLYDIR);
	usort($dirs, function($a,$b){
 		 return filemtime($a) - filemtime($b);
	});
	$icons = array();		
	foreach ($dirs as $e){
		$icons[] = substr($e,5);
	}
	
function drawBarGraph_two($cachefilename, $ydata1, $ydata2,$xdata, $height,$color1,$color2,$ylegend,$xlegend,$title){
        require_once ('jpgraph/jpgraph.php');
        require_once ('jpgraph/jpgraph_utils.inc.php');
        require_once ('jpgraph/jpgraph_bar.php');

        $graph = new Graph(1228,$height);
        $valid = $graph -> cache -> IsValid($cachefilename);
        if ($valid){
                return;
        }else{
                $graph -> SetupCache($cachefilename, 1);
                $graph->clearTheme();
                $graph -> SetScale("textlin");
                $b1plot = new BarPlot($ydata1);
                $b1plot->SetFillColor($color1);
		$b1plot->SetWidth(1.0);
		$b2plot = new BarPlot($ydata2);
          	$b2plot->SetFillColor($color2);
		$b2plot->SetWidth(1.0);
		$gbplot = new GroupBarPlot(array($b1plot,$b2plot));
		$graph->Add($gbplot);
                $graph->title->Set($title);
                $graph->xaxis->SetTickLabels($xdata);
                $graph->xaxis->title->Set($xlegend);
                $graph->yaxis->title->Set($ylegend);
                $graph->title->SetFont(FF_FONT1,FS_BOLD);
                $graph->yaxis->title->SetFont(FF_FONT1,FS_BOLD);
                $graph->xaxis->title->SetFont(FF_FONT1,FS_BOLD);
                $absolutePath = (CACHE_DIR . "" . $cachefilename);
                $graph -> Stroke($absolutePath);
        }
}


function drawBarGraph($cachefilename, $ydata, $xdata, $height,$color,$ylegend,$xlegend,$title){
        require_once ('jpgraph/jpgraph.php');
        require_once ('jpgraph/jpgraph_utils.inc.php');
        require_once ('jpgraph/jpgraph_bar.php');

        $graph = new Graph(1228,$height);
        $valid = $graph -> cache -> IsValid($cachefilename);
        if ($valid){
                return;
        }else{
                $graph -> SetupCache($cachefilename, 1);
                $graph->clearTheme();
                $graph -> SetScale("textlin");
                $bplot = new BarPlot($ydata);
                $bplot->SetFillColor($color);
                $bplot->SetWidth(1.0);
                $graph->Add($bplot);
                $graph->title->Set($title);
                $graph->xaxis->SetTickLabels($xdata);
                $graph->xaxis->title->Set($xlegend);
                $graph->yaxis->title->Set($ylegend);
                $graph->title->SetFont(FF_FONT1,FS_BOLD);
                $graph->yaxis->title->SetFont(FF_FONT1,FS_BOLD);
                $graph->xaxis->title->SetFont(FF_FONT1,FS_BOLD);
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
		$graph->SetShadow();
		$graph->title->Set("Disk Usage");
		$pie = new PiePlot($ydata);
		$colors = array('#FF0000', '#00FF00');
		$pie->SetSliceColors($colors);
                $graph->Add($pie);
                $absolutePath = (CACHE_DIR . "" . $cachefilename);
                $graph -> Stroke($absolutePath);
        }
}
	$df = disk_free_space("/");
	$ds = disk_total_space("/");
	$du = ($df/$ds)*100;
	
	echo "<a href='main.php'>Back to main page</a>";

	$ydata = array(100-$du,$du);
	$f_pie = 'graph/pie.png';
	$graph = drawPieGraph($f_pie, $ydata);

	foreach ($dirs as $e){
		$dir = $e."/IMG_*.jpg";
		$fl = glob($dir);
		$sz = sizeof($fl);
		if($sz > 0)$sz = 1;
		$ydata1[] = $sz;
		$dir = $e."/IRI_*.jpg";
		$fl = glob($dir);
		$sz = sizeof($fl);
		if($sz > 0)$sz = 1;
		$ydata2[] = $sz;
	}
	$f_bar_usage = 'graph/bar_usage.png';
        $graph = drawBarGraph_two($f_bar_usage,$ydata1,$ydata2,$icons,125,'blue','#278BF5',"Usage","Date","Number of Entries Vs Dates");

	unset($ydata);
	foreach ($dirs as $e){
		$dir = $e."/*.tmr";
		$fl = glob($dir);
		$sz = sizeof($fl);
		if($sz > 0)$sz = 1;
		$ydata[] = $sz;
	}
	$f_bar_uptime = 'graph/bar_uptime.png';
        $graph = drawBarGraph($f_bar_uptime,$ydata,$icons,75,'red',"Uptime","Date","Downtime Vs Dates");

	$io = popen ( '/usr/bin/uptime ', 'r' );
    	$ut = fgets ( $io, 4096);
	pclose ($io);
	echo "&emsp;[UpTime:" . $ut . "]";
	
	echo '<table>';
	echo '<tr>';
	echo '<td width="1228 px">';
	echo '<img style="vertical-align: bottom;"  src=';
	echo $f_bar_usage;
	echo '></img></td>';

	echo '<td rowspan="2" width="200px">';
	echo '<img style="vertical-align: bottom;"  width="100%" height="100%" src=';
	echo $f_pie;
	echo '></img></td>';
	echo '</tr>';

	echo '<tr  style="height:50px">';
	echo '<td width="1228 px">';
	echo '<img style="vertical-align: bottom;"  src=';
	echo $f_bar_uptime;
	echo '></img></td>';
	echo '</tr>';

	echo '</table>';

	echo '<form action="analytics.php" method="POST" id="action-form"></form>';

	header("Refresh: 60");
	
	echo 	'<style>
		table, th, td {
		border: 1px solid black;
		}
		th, td {
  		background-color: #96D4D4;
		}
		</style>';
		
	$nc = 0;
	echo '<form><table>';
	foreach ($icons as $e){
		$en = 0;
		$fz = 0;
		if( $nc == 0){
			echo '<tr style="height:200px;text-align: center; vertical-align: middle;">';
		}
		echo '<td width="200px">';
		echo "<input type='submit' value=$e name='data' style='height:100px;width:100px;font-size:60px' form='action-form'></td>";

		$nc += 1;	
		if( $nc == 7){
			echo '</tr>';
			$nc = 0;
		}
	}
	echo '</table></form>';
?>
