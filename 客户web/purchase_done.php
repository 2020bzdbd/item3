<?php
	session_start();
	$numbers = $_SESSION['numbers'];
	if($numbers!='')
	{
		$username=$_SESSION['username'];
		$client_numbers_of_user=$_SESSION['client_numbers_of_user'];
		echo "<h2>感谢您的购买</h2>";
		echo "<table border=\"1\" width=\"50%\" cellpadding=\"0\" cellspacing=\"0\">
			 <tr>
			 <td>购买账号</td>
			 <td>$username</td>
			 </tr>
			 <tr>
			 <td>使用人数</td>
			 <td>$client_numbers_of_user</td>
			 </tr>
			 <tr>
			 <td>序列号</td>
			 <td>$numbers</td>
			 </tr>
			 </table>
			 <a>请牢记你的序列号!!!!</a>";
		//echo "序列号".$numbers;
		echo"</br>";
		echo"<a href='index.php'>返回个人中心</a>";
	}
?>