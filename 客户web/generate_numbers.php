<?php
	session_start();
	if(isset($_POST['sure']))
	{
		$client_numbers_of_user = trim($_POST['client_numbers_of_user']);
		$a=mt_rand(0,9); 
		for ($x=0; $x<=8; $x++) {
		  $a.=mt_rand(0,9);
		}
		$username=$_SESSION['username'];
		//echo $a;
		//echo "<br>";
		//echo $a;
		$file_path="C:/Users/11650/Desktop/大二下_作业/user_info/".$username."/" ;
		$file_name=$file_path.$a."/";
		while(file_exists($file_name))
		{
			$a=mt_rand(0,9); 
			for ($x=0; $x<=8; $x++) {
		  	$a.=mt_rand(0,9);
			}
			$file_name=$file_path.$a."/";
		}
		mkdir($file_name,0777,true);
		$info_file=$file_name."info.txt";
		
		$write_file=fopen($info_file,"w");
		$serial_info=$client_numbers_of_user."\n"."TRUE"."\n"."0"."\n"."No user";
		fwrite($write_file,$serial_info);
		fclose($write_file);
		$_SESSION['numbers']=$a;
		$_SESSION['client_numbers_of_user']=$client_numbers_of_user;
		header("location:purchase_done.php");
	}
?>