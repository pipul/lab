<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<div class="cp_index">
	<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
		<tr class="tb_header">
			<td>活跃记录</td>
			<td>IP地址</td>
			<td>最后活动时间</td>
		</tr>
<!--
EOT;
foreach($onlines as $key => $online){
	print <<<EOT
-->
		<tr class="tb_row">
			<td><a href="admincp.php?job=user&action=mod&userid=$online[uid]">$online[username]</a></td>
			<td>$online[ipaddress]</td>
			<td>$online[lastactivity]</td>
		</tr>
<!--
EOT;
}

print <<<EOT
-->
		<tr><td><br /></td></tr>

		<tr class="tb_header">
			<td colspan="3">服务器解译引擎:</td>
		</tr>
		<tr class="tb_row">
			<td colspan="3">$server[software]</td>
		</tr>

		<tr><td><br /></td></tr>

		<tr class="tb_header">
			<td colspan="3">系统信息</td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">服务器时间:</td>
			<td align="right">$server[datetime]</td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">文件上传:</td>
			<td align="right">$fileupload</td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">全局变量 register_globals:</td>
			<td align="right">$globals</td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">安全模式 safe_mode:</td>
			<td align="right">$safemode</td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">图形处理 GD Library:</td>
			<td align="right">$gd_version</td>
		</tr>
<!--
EOT;

if ($server['memory_info']) {
	print <<<EOT
-->
		<tr class="tb_row">
			<td colspan="2">运行内存占用:</td>
			<td align="right">$server[memory_info]</td>
		</tr>
<!--
EOT;
}
print <<<EOT
-->
		<tr><td><br /></td></tr>

		<tr class="tb_header">
			<td colspan="3">程序相关信息</td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">当前版本:</td>
			<td align="right">$TXP_VERSION Build $TXP_RELEASE</td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">程序开发:</td>
			<td align="right"><a href="mailto:fangdong@pipul.org" target="_blank">Pipul</a></td>
		</tr>
		<tr class="tb_row">
			<td colspan="2">官方主页:</td>
			<td align="right"><a href="http://pipul.org" target="_blank">菩提园</a></td>
		</tr>
	</table>
</div>

<script type="text/javascript">
i=1;
var autourl=new Array();
autourl[1] = 'pipul.org';
autourl[2] = 'blog.pipul.org';
function auto(url){
	if(i){
		i=0;
		var oHead = document.getElementsByTagName('head').item(0); 
		var oScript= document.createElement("script"); 
		oScript.type = "text/javascript"; 
		oScript.src = "http://"+url+"/update.php?version=$now_version&release=$now_release&hostname=$now_hostname"; 
		oHead.appendChild(oScript); 
	}
}
function run(){
	for(var i=1;i<autourl.length;i++) {
		document.write("<img src=http://"+autourl[i]+" width=1 height=1 onerror=auto('"+autourl[i]+"')>");
	}
}
run();
</script>
<!--
EOT;
?>
-->
