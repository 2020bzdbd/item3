<?php
	header('Content-type:text/html; charset=utf-8');
	// 开启Session
	session_start();
	if(isset($_POST['sign_up']))
	{
		//$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/".$username."/";
		$username = trim($_POST['username']);
		$password = trim($_POST['password']);
		$sure_password=trim($_POST['sure_password']);
		$email=trim($_POST['email']);
		$phone_number=trim($_POST['phone_number']);
		$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/".$username."/";
		if($password!=$sure_password)
		{
			echo "两次输入密码不一致，3秒后自动跳转注册页面";
			echo "$password "."  "."$sure_password";
			header('refresh:3; url=sign_up.php');
			exit;
		}
		//$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/".$username."/";
		elseif(file_exists($file_path))
		{
			echo "该账号已经存在，3秒后自动跳转注册页面";
			header('refresh:3; url=sign_up.php');
			exit;
		}
		else{
		mkdir($file_path,0777,true);
		$info_file=$file_path."info.txt";
		$write_file=fopen($info_file,"w");
		fwrite($write_file,$password."\n".$email."\n".$phone_number);
		fclose($write_file);

		echo "注册成功，3秒后自动跳转登陆页面";
		header('refresh:3; url=login_client.php');
		//exit;
	}
	//exit;
	}
?>