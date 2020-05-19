<?php
	session_start();
	$username=$_SESSION['username'];
	if(isset($_SESSION['islogin']))
	{

	}
	else
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆";
		exit;
	}
?>

<h2>查看序列号</h2>
<table border="1" width="100%" cellpadding="0" cellspacing="0">
<tr>
	<td>序列号</td>
	<td>使用人数</td>
</tr>
<?php
	session_start();
	$username=$_SESSION['username'];
	if(isset($_SESSION['islogin']))
	{
		$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/".$username."/";
		$list=scandir($file_path);
		for($i=0;$list[$i]!="";$i++)
		{
			$numbers=$list[$i];
			if(preg_match("/[0-9]{10}/", $numbers))
			{
				$fp = fopen($file_path.$numbers."/"."info.txt",'r');
				$read_number_of_users=fgets($fp);
				echo "<tr>
					<td>$numbers</td>
					<td>$read_number_of_users</td>
				</tr>";
			}
		}
	}
	else
	{
		exit;
	}
?>
</table>
<a href="index.php">返回主页面</a>