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
	<link rel="stylesheet" href="content/templates/$options[templatename]/style.css" type="text/css" media="all" />
	<link rel="stylesheet" href="include/code.css" type="text/css" media="all" />

	<title>$options[title]</title>
</head>
<body>
<a href="http://www4.clustrmaps.com/zh/counter/maps.php?url=http://blog.pipul.org">
<img  width="0" height="0" src="http://www4.clustrmaps.com/stats/maps-no_clusters/blog.pipul.org-thumb.jpg"/></a>
<!--
EOT;
if ($os != 'OK!'){
	print <<<EOT
-->
<div id="mainpage">
	<table border="0" width="100%">
		<tr>
			<td width="20%"><a href="/"><img src="content/templates/$options[templatename]/screenshot.png" border="0"></a></td>
			<td width="75%"><center><b>Laodong's</b> Home Page 
<!--
EOT;
	if ($options['rss_enable']) {
		print <<<EOT
-->
			<a href="rss.php" target="_blank" title="RSS 2.0 订阅"><img src="content/templates/$options[templatename]/img/googlerss.gif" border="0" alt="RSS 2.0 订阅" /></a>
<!--
EOT;
	}
	print <<<EOT
-->
			<br>Power by <a href="http://pipul.org">Pipul</a></center>
			<br><center>$options[description]</center></td>
		</tr>
	</table>
<!--
EOT;
	if ($action == 'index') {
		print <<<EOT
-->
	<div id="aboutme">
		<h2>About me</h2>
		<p>I am a three year SE. student in Computer Science at Taiyuan University of Technology. This is my personal website where I write some technical articles and share with you. Occasionally have something interesting.</p>
		<p>My technical interests include distributed storage system and information retrieval. If you have any questions or comments, please don't hesitate to contact me by the email address: fangdong@pipul.org . As you see, the prefix is my name.</p>
	</div>
<!--
EOT;
	}
	print <<<EOT
-->
	<div id="post">
<!--
EOT;
	if(!empty($catedb) && $action == 'index'){
		print <<<EOT
-->
		<h2>Categories</h2>
		<ul>
<!--
EOT;
		foreach($catedb AS $data){
		print <<<EOT
-->
			<li><a href="./?action=index&amp;cid=$data[cid]">$data[name] [$data[articles]]</a> <a href="./rss.php?cid=$data[cid]" target="_blank" title="RSS 2.0 订阅这个分类"><img src="content/templates/$options[templatename]/img/rss.gif" border="0" alt="RSS 2.0 订阅这个分类" /></a> </li>

<!--
EOT;
		}
		print <<<EOT
-->
		</ul>
<!--
EOT;
	}

	require_once PrintEot($pagefile);

	print <<<EOT
-->
	</div>

	<div id="frlink">
<!--
EOT;
	if ($action == 'index') {
		require_once PrintEot('links');
	}
	if ($action == 'index') {
		print <<<EOT
-->
	</div>
	<div id="footer">
		<p>
<!--
EOT;
		if ($txp_uid) {
			print <<<EOT
-->
		欢迎您，$txp_user &raquo; <a href="post.php?action=logout">注销</a>
<!--
EOT;
			if ($txp_group == 1 || $txp_group == 2) {
				print <<<EOT
-->
		| <a href="admin/admincp.php" target="_blank">管理</a>
<!--
EOT;
			}
		}else{
			print <<<EOT
-->
		<a href="./?action=login">后台管理</a>
<!--
EOT;
		}
	}
	print <<<EOT
-->
		<img src="content/templates/$options[templatename]/img/valid-html401.png"></p>
	</div>
</div>
<!--
EOT;
}else{
	print <<<EOT
-->
<p>Thanks - But the blog does not support your system:<b>$os</b></p>
<hr>
<p>That is only for:</p>
<ul>
	<li>&#8250;&#8250; all the linux distribution,such as Slackware,Ubuntu,Fedora,Debian...</li>
	<li>&#8250;&#8250; The Unix or Unix-like operating systems:Mac,FreeBSD...</li>
	<li>&#8250;&#8250; It is also support the SunOS and AIX.</li>
</ul>
<p>If you have any question, contact me:<a href="mailto:fdcwqmst@gmail.com"><img src="content/uploads/2011/10/WB111024_002.png" alt=""></a></p>
<hr>
<p>Power by <a href="http://pipul.org">Textpress.</a></p>
<!--
EOT;
}
print <<<EOT
-->
</body>
</html>
<!--
EOT;
?>-->
