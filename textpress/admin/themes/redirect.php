<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
--><!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>系统消息</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta HTTP-EQUIV="REFRESH" content="$min;URL=$url">
	<style type="text/css">
	* {

		padding: 0;
		margin: 0;
	}
	body {
		text-align:center;
		font-size:12px;
		color: #333;
		font-family: tahoma,verdana,sans-serif;
	}
	a {
		color: #00f;
		text-decoration: none;
	}
	a:hover {
		color: #523;
		text-decoration: none;
	}
	.box {
		border: 2px #B1B6D2 solid;
		width: 500px;
		margin: 100px auto;
		background-color: #F7F8FA;
		background-repeat: repeat-x;
		background-position: left top;
		text-align: center;
		padding: 30px;
	}
	.alert {
		color: #963;
		font-size:14px;
		margin-bottom: 25px;
	}
	.alertmsg {
		background-color: transparent;
		margin-top: 25px;
	}
	</style>
</head>
<body style="text-align:center">
	<div class="box">
		<h2 class="alert">$msg</h2>
		<div class="alertmsg"><a href="$url">如果你不想等待或浏览器没有自动跳转请点击这里跳转</a></div>
	</div>
</body>
</html>
<!--
EOT;
?>
-->
