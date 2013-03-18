<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta http-equiv="Content-Language" content="UTF-8" />
	<meta http-equiv="Pragma" content="no-cache" />
	<meta name="keywords" content="$options[meta_keywords]" />
	<meta name="description" content="$options[meta_description]" />
<!--
EOT;
if ($returnurl) {
	print <<<EOT
-->
	<meta http-equiv="REFRESH" content="$min;URL=$returnurl">
<!--
EOT;
}
print <<<EOT
-->
	<title>系统消息 $options[title_keywords]</title>
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
<body>
	<div class="box">
  		<h2 class="alert">$msg</h2>
<!--
EOT;
if ($returnurl) {
	print <<<EOT
-->
  		<p class="alertmsg"><a href="$returnurl">如果不想等待或浏览器没有自动跳转请点击这里</a></p>
<!--
EOT;
}
print <<<EOT
-->
	</div>
</body>
</html>
<!--
EOT;
?>-->
