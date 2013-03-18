<?php 
include('fckeditor.php') ;
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
	<head>
		<title>FCKeditor</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta name="robots" content="noindex, nofollow">
	</head>
	<body>
		<h1>FCKeditor</h1>
		<p><a href="?toolbar=Basic">Basic</a> - <a href="?toolbar=Default">Default</a></p>
		<form action="posteddata.php" method="post">
<?php
$sBasePath = $_SERVER['PHP_SELF'] ;
$sBasePath = substr( $sBasePath, 0, strpos( $sBasePath, "_samples" ) ) ;

$oFCKeditor = new FCKeditor('FCKeditor1') ;
$oFCKeditor->BasePath	= $sBasePath ;
if ($_GET['toolbar'] == 'Basic') {
	$oFCKeditor->ToolbarSet = 'Basic';
}
$oFCKeditor->Value		= '<p>This is some <strong>sample text</strong>. You are using <a href="http://www.fckeditor.net/">FCKeditor</a>.</p>
<p><strike>支持语法高亮了.高兴吗?</strike></p>
<div class="codeText">
<div class="codeHead">PHP代码</div>
<ol class="dp-c">
    <li class="alt"><span><span class="keyword">if</span><span>&nbsp;(</span><span class="vars">$options</span><span>[</span><span class="string">\'remote_open\'</span><span>])&nbsp;{ &nbsp;&nbsp;</span></span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="vars">$uinfo</span><span>&nbsp;=&nbsp;</span><span class="func">parse_url</span><span>(</span><span class="vars">$options</span><span>[</span><span class="string">\'url\'</span><span>]); &nbsp;&nbsp;</span></li>
    <li class="alt"><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="vars">$ourhost</span><span>&nbsp;=&nbsp;</span><span class="func">str_replace</span><span>(</span><span class="string">\'www.\'</span><span>,&nbsp;</span><span class="string">\'\'</span><span>,&nbsp;</span><span class="vars">$uinfo</span><span>[</span><span class="string">\'host\'</span><span>]); &nbsp;&nbsp;</span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="comment">//如果包含端口信息,去掉端口,否则都会禁止下载 </span><span>&nbsp;&nbsp;</span></li>
    <li class="alt"><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="keyword">if</span><span>&nbsp;(</span><span class="func">strpos</span><span>(</span><span class="vars">$ourhost</span><span>,</span><span class="string">\':\'</span><span>)){ &nbsp;&nbsp;</span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="vars">$ourhost</span><span>&nbsp;=&nbsp;</span><span class="func">substr</span><span>(</span><span class="vars">$ourhost</span><span>,&nbsp;0,&nbsp;</span><span class="func">strrpos</span><span>(</span><span class="vars">$ourhost</span><span>,&nbsp;</span><span class="string">\':\'</span><span>)); &nbsp;&nbsp;</span></li>
    <li class="alt"><span>&nbsp;&nbsp;&nbsp;&nbsp;} &nbsp;&nbsp;</span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="vars">$uinfo</span><span>&nbsp;=&nbsp;</span><span class="func">parse_url</span><span>(</span><span class="vars">$_SERVER</span><span>[</span><span class="string">\'HTTP_REFERER\'</span><span>]); &nbsp;&nbsp;</span></li>
    <li class="alt"><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="vars">$remotehost</span><span>&nbsp;=&nbsp;</span><span class="func">str_replace</span><span>(</span><span class="string">\'www.\'</span><span>,&nbsp;</span><span class="string">\'\'</span><span>,&nbsp;</span><span class="vars">$uinfo</span><span>[</span><span class="string">\'host\'</span><span>]); &nbsp;&nbsp;</span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="keyword">if</span><span>&nbsp;(</span><span class="func">strpos</span><span>(</span><span class="vars">$remotehost</span><span>,</span><span class="string">\':\'</span><span>)){ &nbsp;&nbsp;</span></li>
    <li class="alt"><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="vars">$remotehost</span><span>&nbsp;=&nbsp;</span><span class="func">substr</span><span>(</span><span class="vars">$remotehost</span><span>,&nbsp;0,&nbsp;</span><span class="func">strrpos</span><span>(</span><span class="vars">$remotehost</span><span>,&nbsp;</span><span class="string">\':\'</span><span>)); &nbsp;&nbsp;</span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;} &nbsp;&nbsp;</span></li>
    <li class="alt"><span>&nbsp;&nbsp;&nbsp;&nbsp;unset(</span><span class="vars">$uinfo</span><span>); &nbsp;&nbsp;</span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="keyword">if</span><span>&nbsp;(</span><span class="vars">$ourhost</span><span>&nbsp;!=&nbsp;</span><span class="vars">$remotehost</span><span>)&nbsp;{ &nbsp;&nbsp;</span></li>
    <li class="alt"><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;message(</span><span class="string">\'附件禁止从地址栏直接输入或从其他站点链接访问\'</span><span>,&nbsp;</span><span class="string">\'./\'</span><span>); &nbsp;&nbsp;</span></li>
    <li class=""><span>&nbsp;&nbsp;&nbsp;&nbsp;} &nbsp;&nbsp;</span></li>
    <li class="alt"><span>}&nbsp;&nbsp;</span></li>
</ol>
</div>';
$oFCKeditor->Create() ;
?>
			<br />
			<input type="submit" value="Submit">
		</form>
	</body>
</html>