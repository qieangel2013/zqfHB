<?php
$type=ini_get('zqfHB.type');
switch ($type) {
    case 1:
        $redis = new Redis();
        $redis->connect(ini_get('zqfHB.host'),ini_get('zqfHB.port'));
        $zqfHB_idx = $redis->lrange('zqfHB',0,-1);
        foreach ($zqfHB_idx as $k => $v) {
            $slowlog = unserialize($v);
            $data[ $slowlog['filename'] ][] = $slowlog;
        }
        break;
    case 2:
        $memcache = new Memcache();
        $memcache->connect(ini_get('zqfHB.host'),ini_get('zqfHB.port'));
        $zqfHB_idx = $memcache->get('zqfHB_idx');
        for($i = 1; $i < $zqfHB_idx; $i++){
            $idx = "zqfHB_{$i}";
            $slowlog = unserialize($memcache->get($idx));
            $data[ $slowlog['filename'] ][] = $slowlog;
        }
        break;
    default:
        echo 'not installed zqfHB extension,please install https://github.com/qieangel2013/zqfHB\r\n';
        break;
}
if($data){
    $total_duration = 0;

    foreach($data as $key=>$item){
        $item_total_duration = 0;
        foreach($item as $slowlog){
            $item_total_duration += $slowlog['duration'];
            $data[$key]['filename'] = $slowlog['filename'];
        }

        $data[$key]['aver_duration'] = (int)($item_total_duration / count($item));
        $total_duration += $data[$key]['aver_duration'];
    }

    foreach($data as $key=>$item){
        $data[$key]['percent'] = number_format( ($item['aver_duration'] / $total_duration), 2, '.', '');
    }


}else{
    echo "无数据";
    exit;
}
?>


<!doctype html>
<html>
<head>
<script src="js/jquery-1.8.0.min.js" type="text/javascript"></script>
<script src="js/highcharts.js"></script>
<script src="js/modules/exporting.js"></script>
<script type="text/javascript" charset="utf-8">
	<?php 
	foreach($data as $filename=>$slowlog){
		foreach($slowlog as $key=>$item){
			if(in_array($key, array('filename', 'percent', 'aver_duration'))) continue;
			$json[$filename][] = $item;
		}
	}

	echo "var DATA = $.parseJSON('".json_encode($json)."');";
	?>


	$(function () {
        $('#container').highcharts({
            chart: {
                plotBackgroundColor: null,
                plotBorderWidth: null,
                plotShadow: false
            },
            title: {
                text: 'PHP benchmark(平均执行时间)'
            },
            subtitle: {
                text: '统计结果仅针对执行时间超过slowlog.slow_maxtime的数据',
                x: -20
            },
            tooltip: {
        	    pointFormat: '点击查看详细列表',
            	percentageDecimals: 1
            },
            plotOptions: {
                pie: {
                    allowPointSelect: true,
                    cursor: 'pointer',
                    dataLabels: {
                        enabled: true,
                        color: '#000000',
                        connectorColor: '#000000',
                        formatter: function() {
                            return '<b>'+ this.point.name +'</b>: '+ this.percentage.toFixed(2) +' %';
                        }
                    }
                }
            },
            series: [{
                type: 'pie',
                name: 'ExecuteTime',
                data: [
                	<?php foreach($data as $key=>$item){?>
                    {
                        name: '<?php echo $item['filename'];?>',
                        y: <?php echo $item['percent'];?>,
                        events:{
                        	click:function(){
                        		show_slowlog('<?php echo $item['filename'];?>');
                        	}
                        }
                    },
                    <?php }?>
                ]
            }]
        });
    });

	var show_slowlog = function(filename){
		var categories = [];
		var data 	   = [];

		for(key in DATA[filename]){
			categories.push("id:"+DATA[filename][key].id);
			data.push(parseInt(DATA[filename][key].duration)/1000000);
		};

		console.log(data);
		console.log(categories);
		

		$('#slowlog').highcharts({
            chart: {
                type: 'line',
                marginRight: 130,
                marginBottom: 25
            },
            title: {
                text: filename+'执行时间详细统计',
                x: -20 //center
            },
            xAxis: {
                categories: categories
            },
            yAxis: {
                title: {
                    text: '执行时间详细统计(单位秒)'
                },
                plotLines: [{
                    value: 0,
                    width: 1,
                    color: '#808080'
                }]
            },
            tooltip: {
                valueSuffix: 's'
            },
            legend: {
                layout: 'vertical',
                align: 'right',
                verticalAlign: 'top',
                x: -10,
                y: 100,
                borderWidth: 0
            },
            series: [{
                name: filename,
                data: data
            }]
        });
	}
</script>
</head>

<body style="text-align:center;">
	<div id="container" style="width:800px;margin-right:auto;margin-left:auto;"></div>
	<hr/>
	<div id="slowlog" style="width:800px;margin-right:auto;margin-left:auto; margin-top:20px"></div>
</body>
</html>