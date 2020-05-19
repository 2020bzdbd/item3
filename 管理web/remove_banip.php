<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
	$ip=$_GET['ip'];
	$file_path="C:/Users/11650/Desktop/大二下_作业/admin_store/cmd.txt";
	$write_file=fopen($file_path,"a");
	fwrite($write_file,"UBN+"."$ip"."\n");
	echo"<script>alert('移除成功,这需要一定时间,如未更新请尝试刷新');history.go(-1);</script>";
?>