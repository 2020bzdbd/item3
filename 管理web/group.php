<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
?>

<h2>公司或团体</h2>
<div><a href="index.php">返回</a></div>
<table border="1" width="60%" cellpadding="0" cellspacing="0">
	<tr>
		<td>名称</td>
		<td>拥有序列号数</td>
		<td>使用人数/人数上限</td>
		<td>操作</td>
	</tr>
	<?php
		session_start();
		$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/";
		$group_list=scandir($file_path);
		for($i=0;$group_list[$i]!="";$i++)
		{
			$group_name=$group_list[$i];
			//echo $group_name."*"."</br>";
			if(preg_match("/[0-9a-zA-Z]{1,}/", $group_name))
			{
				//echo $group_name."*"."</br>";
				$number_list=scandir($file_path.$group_name);
				$max_users=0;$current_users=0;
				$numbers_num=0;
				for($j=0;$number_list[$j]!="";$j++)
				{
					$number_name=$number_list[$j];
					if(preg_match("/[0-9]{10}/", $number_name))
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
						$max_users+=$temp1;
						$current_users+=$temp3;
						//echo " temp1 ".$temp1." temp3: ".$temp3;
					}
				}
				$current_to_max=$current_users."/".$max_users;
				echo "<tr>
					<td>$group_name</td>
					<td>$numbers_num</td>
					<td>$current_to_max</td>
					<td><a href='numbers.php?group=$group_name'>选择</a></td>
				</tr>";
			}
		}
		function remove_utf8_bom($text)
		{
		    $bom = pack('H*','EFBBBF');
		    $text = preg_replace("/^$bom/", '', $text);
		    return $text;
		}
	?>
</table>