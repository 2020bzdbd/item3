<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
?>

<h2>查询结果</h2>
<table border="1" width="60%" cellpadding="0" cellspacing="0">
	<tr>
		<td>公司名</td>
		<td>序列号</td>
		<td>IP</td>
		<td>端口号</td>
		<td>登陆时间</td>
		<td>功能</td>
	</tr>
	<?php
		session_start();
		if(!isset($_POST['search']))
		{
			header('location:search.php');
			exit;
		}
		$group_name_criteria=trim($_POST['group_name']);
		$number_name_criteria=trim($_POST['number_name']);
		$ip_criteria=trim($_POST['ip']);
		$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/";
		$group_list=scandir($file_path);
		for($i=0;$group_list[$i]!="";$i++)
		{
			$group_name=$group_list[$i];
			//echo $group_name."*"."</br>";
			if(preg_match("/[0-9a-zA-Z]{1,}/", $group_name)&&($group_name==$group_name_criteria||$group_name_criteria==""))
			{
				//echo $group_name."*"."</br>";
				$number_list=scandir($file_path.$group_name);
				$max_users=0;$current_users=0;
				$numbers_num=0;
				for($j=0;$number_list[$j]!="";$j++)
				{
					$number_name=$number_list[$j];
					$max_users=0;$current_users=0;
					if(preg_match("/[0-9]{10}/", $number_name)&&($number_name==$number_name_criteria||$number_name_criteria==""))
					{
						$numbers_num++;
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
							for($k=1;$k<=$current_users;$k++)
							{
								$temp_info=fgets($fp);
								$temp_info=str_replace("\n", "", $temp_info);
								$temp_info=remove_utf8_bom($temp_info);
								$info_array=explode(" ", $temp_info);
								if($ip_criteria==$info_array[0]||$ip_criteria=="")
								{
								echo "<tr>
								<td>$group_name</td>
								<td>$number_name</td>
								<td>$info_array[0]</td>
								<td>$info_array[1]</td>
								<td>$info_array[2]</td>
								<td><a href='users.php?group=$group_name&numbers=$number_name'>跳转</a></td>
								</tr>";
							}
							}
						}
						fclose($fp);
					}
				}
				
			}
		}
		echo "<div><a href='search.php'>返回上一级</a></div>
			<div><a href='index.php'>返回主页面</a></div>";
		function remove_utf8_bom($text)
		{
		    $bom = pack('H*','EFBBBF');
		    $text = preg_replace("/^$bom/", '', $text);
		    return $text;
		}
	?>
</table>