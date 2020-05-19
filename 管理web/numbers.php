<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
?>

<h2>公司/团体序列管理</h2>
<table border="1" width="60%" cellpadding="0" cellspacing="0">
	<tr>
		<td>序列号</td>
		<td>使用人数/最大人数</td>
		<td>操作</td>
	</tr>
<?php
	session_start();
	//$hello_str="公司/团体名：".$_GET['group'];
	$group_name=$_GET['group'];
	echo "<h2>$group_name</h2>";
	$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/";
	$number_list=scandir($file_path.$group_name);
	for($i=0;$number_list[$i]!="";$i++)
	{
		$max_users=0;
		$current_users=0;
		$number_name=$number_list[$i];
		if(preg_match("/[0-9]{10}/", $number_name))
		{
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
			$current_to_max=$current_users."/".$max_users;
			echo "<tr>
					<td>$number_name</td>
					<td>$current_to_max</td>
					<td><a href='users.php?group=$group_name&numbers=$number_name'>选择</a></td>
				</tr>";
		}
	}
	echo "<div><a href='group.php'>返回上一级</a></div>
			<div><a href='index.php'>返回主页面</a></div>";
	function remove_utf8_bom($text)
		{
		    $bom = pack('H*','EFBBBF');
		    $text = preg_replace("/^$bom/", '', $text);
		    return $text;
		}

?>
</table>
