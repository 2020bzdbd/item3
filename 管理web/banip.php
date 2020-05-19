<?php
	session_start();
	if(!isset($_SESSION['islogin']))
	{
		header('refresh:3; url=login_client.php');
		echo "还没登陆,3秒后跳转登陆页面";
		exit;
	}
?>
<h2>封禁IP</h2>
<div><a href="index.php">返回主页面</a></div>
</br>
<table border="1" width="60%" cellpadding="0" cellspacing="0">
	<tr>
		<td>序号</td>
		<td>ip</td>
		<td>操作</td>
	</tr>
	<?php
	$file_path="C:/Users/11650/Desktop/大二下_作业/admin_store/forbid.txt";
	$fp = fopen($file_path,'r');
	for($i=1;$temp_ip=fgets($fp);$i++)
	{
		$temp_ip=str_replace("\n", "", $temp_ip);
		$temp_ip=remove_utf8_bom($temp_ip);
		echo"<tr>
		<td>$i</td>
		<td>$temp_ip</td>
		<td><a href='remove_banip.php?ip=$temp_ip'>移除</a></td>
		</tr>";

	}
	function remove_utf8_bom($text)
	{
	    $bom = pack('H*','EFBBBF');
	    $text = preg_replace("/^$bom/", '', $text);
	    return $text;
	}
	?>
</table>
<form action="ban.php" method="post">
		<fieldset>
			<legend>新增封禁</legend>
			<ul>
				<li>
					<label>封禁IP:</label>
					<input type="text" name="banip">
				</li>
				<li>
					<label> </label>
					<input type="submit" name="ban" value="确定">
				</li>
			</ul>
		</fieldset>
	</form>