<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
?>
<?php
	session_start();
	echo "<h2>成功</h2>";
	echo "</br>";
	echo "<a>强制下线成功,这会有一定延迟，请等待一定时间</a>";
	$group_name=$_GET['group'];
	$number_name=$_GET['numbers'];
	$ip=$_GET['ip'];
	$port=$_GET['port'];

	$info_str="公司/团体名称:".$group_name." 序列号:".$number_name." ip:".$ip." 端口号:".$port."</br>";
	echo $info_str;
	$file_path="C:/Users/11650/Desktop/大二下_作业/admin_store/cmd.txt";
	$write_file=fopen($file_path,"a");
	fwrite($write_file,"DRP+".$group_name."+".$number_name."+".$ip."+".$port."\n");
	echo"<script>alert('强制下线成功,这需要一定时间,如未更新请尝试刷新');history.go(-1);</script>";
?>