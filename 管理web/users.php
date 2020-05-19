<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
?>

<table border="1" width="60%" cellpadding="0" cellspacing="0">
	<tr>
		<td>序号</td>
		<td>IP</td>
		<td>端口号</td>
		<td>登陆时间</td>
		<td>功能</td>
	</tr>
<?php
	session_start();
	$group_name=$_GET['group'];
	$number_name=$_GET['numbers'];
	echo "<h2>".$group_name."/".$number_name."</h2>";
	$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/";
	$fp = fopen($file_path.$group_name."/".$number_name."/"."info.txt",'r');

	$temp1=fgets($fp);
	$temp2=fgets($fp);
	$temp3=fgets($fp);
	$temp1=str_replace("\n","", $temp1);
	$temp3=str_replace("\n","", $temp3);
	$temp1=remove_utf8_bom($temp1);
	$temp3=remove_utf8_bom($temp3);
	$max_users=$temp1;
	$current_users=$temp3;
	if($current_users>0)
	{
		for($i=1;$i<=$current_users;$i++)
		{
			$temp_info=fgets($fp);
			$temp_info=str_replace("\n", "", $temp_info);
			$temp_info=remove_utf8_bom($temp_info);
			$info_array=explode(" ", $temp_info);
			echo "<tr>
			<td>$i</td>
			<td>$info_array[0]</td>
			<td>$info_array[1]</td>
			<td>$info_array[2]</td>
			<td><a href='drop_user.php?group=$group_name&numbers=$number_name&ip=$info_array[0]&port=$info_array[1]'>强制下线</a></td>
		</tr>";
		}
	}
	echo "<div><a href='numbers.php?group=$group_name'>返回上一级</a></div>
			<div><a href='index.php'>返回主页面</a></div>";


	function remove_utf8_bom($text)
	{
	    $bom = pack('H*','EFBBBF');
	    $text = preg_replace("/^$bom/", '', $text);
	    return $text;
	}
?>
</table>