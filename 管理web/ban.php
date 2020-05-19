<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
	if (isset($_POST['ban']))
	{
		$banip=trim($_POST['banip']);
		if(preg_match("/(([1-2]{1}[0-9]{2})|[0-9]{1,2})(\.(([1-2]{1}[0-9]{2})|[0-9]{1,2})){3}/", $banip))
		{
			$file_path="C:/Users/11650/Desktop/大二下_作业/admin_store/cmd.txt";
			$write_file=fopen($file_path,"a");
			fwrite($write_file,"BAN+"."$banip"."\n");
			echo"<script>alert('封禁成功,这需要一定时间,如未更新请尝试刷新');history.go(-1);</script>";
		}
		else
		{
			echo"<script>alert('错误的IP');history.go(-1);</script>";
			//exit;
		}
	}
?>