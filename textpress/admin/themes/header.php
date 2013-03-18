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
	<meta name="copyright" content="TextPress" />
	<title>DashBoard</title>
	<link rel="stylesheet" href="themes/cp.css" type="text/css">
	<link rel="stylesheet" href="themes/sdmenu/sdmenu.css" type="text/css"/>
	<script type="text/javascript" src="themes/sdmenu/sdmenu.js"></script>
	<script type="text/javascript">
	// <![CDATA[
	var myMenu;
	window.onload = function() {
		myMenu = new SDMenu("my_menu");
		myMenu.init();
	};
	// ]]>
	</script>
</head>
<body>
<div id="container">
<!--
EOT;
if (isset($adminitem) && $adminitem && $userinfo) {
	print <<<EOT
-->
	<div style="float: left" id="my_menu" class="sdmenu" >
<!--
EOT;
	foreach ($adminitem AS $ul => $sub)	{
		print <<<EOT
-->
		<div>
			<span>$ul</span>
<!--
EOT;
		foreach ($sub AS $name => $link) {
			print <<<EOT
-->
			<a href="$link">&nbsp;&nbsp;&nbsp;&nbsp;$name</a>
<!--
EOT;
		}
		print <<<EOT
-->
		</div>
<!--
EOT;
	}
	print <<<EOT
-->
	</div>
<!--
EOT;
}
?>
-->
