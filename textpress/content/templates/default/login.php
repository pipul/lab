<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta http-equiv="Content-Language" content="utf-8" />
		<meta http-equiv="Pragma" content="no-cache" />
		<meta name="keywords" content="$options[meta_keywords]" />
		<meta name="description" content="$options[meta_description]" />
		<link rel="alternate" title="$options[name]" href="rss.php" type="application/rss+xml" />
		<title>$options[title]</title>

<style type="text/css">
	body {
		background: #ebeff9;
	}
	input {
		width:390px;
		padding:0 15px;
	}
	img {
		vertical-align:bottom;
	}
	.uf_center {
		margin:15px auto;
	}
	.uf_center tr td {
		padding: 5px 10px 5px 10px;
	}
	.uf_header td{
		color: #fff;
		padding: 5px 10px;
		font-weight: bold;
		border-bottom: 1px solid #ddd;
		background: #066;
	}

	.uf_header a{
		color: #fff;
	}

	.uf_row td{
		background: #FFF8F8;
		border-bottom: 1px solid #ddd;
		padding: 5px 10px 5px 10px;
	}
	.footer {
		text-align: right;
		padding: 10px 200px;
		border-top: 2px solid #C2CFF1;
		background: #fff;
	}
</style>

	</head>

	<body>
		<form action="post.php" method="post" > 
			<table border="0" id="login" class="uf_center" width="400px">
				<tr class="uf_header">
					<td>Login in UserGroup</td>
				</tr>
				<tr>
					<td>UserName:</td>
				</tr>
				<tr>
					<td><input type="text" name="username" id="username" /></td>
				</tr>
				<tr>
					<td>PassWord:</td>
				</tr>
				<tr>
					<td><input type="password" name="password" id="password" /><input type="hidden" name="action" value="dologin" /><input type="hidden" name="formhash" value="$formhash" />
					</td>
				</tr>
<!--
EOT;
if ($options['seccode_enable']) {
	print <<<EOT
-->
				<tr>
					<td><img id="seccode" class="codeimg" src="include/seccode.php" alt="Reflash" border="0" onclick="this.src='include/seccode.php?update=' + Math.random()" /></td>
				</tr>
				<tr>
					<td><input name="clientcode" id="clientcode" /></td> 
				</tr>
<!--
EOT;
}
print <<<EOT
-->
				<tr align="center">
					<td><input type="submit" value="Login" style="width:100px;"></td> 
				</tr>
			</table>
		</form>
		<div class="footer">
			<p style="margin:5px auto;"> &copy; 2010-2011 Powered by <strong><a href="http://blog.pipul.org" target="_blank">TextPress 1.0</a></strong></p>
			<p>Version: Build 1.0</p>
		</div>
	</body>
</html>
<!--
EOT;
?>
