<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<script type="text/javascript">
function really(d,m,n) {
	if (confirm(m)) {
		window.location.href='admincp.php?job=template&action=delonetag&tag='+d+'&tagid='+n;
	}
}
</script>

<div class="cp_template">

<!--
EOT;
if($action == 'template'){
	print <<<EOT
-->
	<form action="admincp.php?job=template" method="post" name="form">
		<input type="hidden" name="action" value="donewtemplate">
		<p align="center"><input type="text" name="newtemplatename" /> <input type="submit" value="新建" /></p>
	</form>

	<br />

	<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
		<tr class="tb_header">
			<td>当前模板</td>
			<td>模板信息</td>
			<td>概要描述</td>
			<td>模板操作</td>
		</tr>
<!--
EOT;
	if ($current_template_info) {
		print <<<EOT
-->
		<tr class="tb_row">
			<td><img alt="$current_template_info[name]" src="$current_template_info[screenshot]" border="0" /></td>
			<td><p>模板名称:$current_template_info[name]</p><p>制作者:$current_template_info[author]</p><p>当前版本:$current_template_info[version]</p><p>模板路径:<b>$current_template_info[templatedir]</b></p></td>
			<td>$current_template_info[description]</td>
			<td>[<a href="admincp.php?job=template&action=settemplate&name=default">关闭</a>][<a href="admincp.php?job=template&action=filelist&path=$current_template_info[dirurl]">修改</a>]</td>
		</tr>
<!--
EOT;
	} else {
		print <<<EOT
-->
		<tr class="tb_row">
			<td colspan="4">没有当前主题的相关资料</td>
		</td>
<!--
EOT;
	}
	print <<<EOT
-->
	</table>

	<br /><br />

	<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
		<tr class="tb_header">
			<td>可用模板</td>
			<td>模板信息</td>
			<td>概要描述</td>
			<td>模板操作</td>
		</tr>
<!--
EOT;
	if ($available_template_db) {
		foreach($available_template_db as $id => $template){
			print <<<EOT
-->
		<tr class="tb_row">
			<td><img src="$template[screenshot]" border="0" /></td>
			<td><p>模板名称:$template[name]</p><p>制作者:$template[author]</p><p>当前版本:$template[version]</p><p>模板路径:<b>$template[templatedir]</b></p></td>
			<td><p>$template[description]</p></td>
			<td>[<a href="admincp.php?job=template&action=settemplate&name=$template[dirurl]">启用</a>] [<a href="admincp.php?job=template&action=filelist&path=$template[dirurl]">修改</a>]</td>
		</tr>

<!--
EOT;
		}
	} else {
		print <<<EOT
-->
		<tr class="tb_row">
			<td colspan="4"><b>没有可用模板</b></td>
		</tr>
<!--
EOT;
	}
	print <<<EOT
-->
	</table>
<!--
EOT;
} elseif($action == 'filelist'){
	print <<<EOT
-->
	<table width="400px" class="cp_center" border="0" cellspacing="0" cellpadding="0">
		<tr class="tb_header">
			<td width="60%">文件名</td>
			<td width="40%" align="right">操作</td>
		</tr>
<!--
EOT;
	foreach($filedb as $key => $file){
		print <<<EOT
-->
		<tr class="tb_row">
			<td>$file[filename]</td>
			<td align="right">[<a href="admincp.php?job=template&action=mod&path=$path&file=$file[filename]&ext=$file[extension]">编辑</a>] [<a href="admincp.php?job=template&action=del&path=$path&file=$file[filename]&ext=$file[extension]">删除</a>]</td>
		</tr>
<!--
EOT;
	}
	print <<<EOT
-->
		<tr class="tb_header">
			<td colspan="2">共有<b>$tp_count</b>个模板文件</td>
		</tr>
	</table>
<!--
EOT;
} elseif ($action == 'mod') {
	print <<<EOT
-->
	<form action="admincp.php?job=template" method="post" name="form">
		<input type="hidden" name="action" value="savefile">
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">	
<!--
EOT;
	if (!$writeable) {
		print <<<EOT
-->
			<tr class="tb_header">
				<td><b>警告:</b></td>
			</tr>
			<tr class="tb_row">
				<td>当前模板文件不可写入, 请设置为 0777 权限后再编辑此文件.</td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<input type="hidden" name="path" value="$path">
			<input type="hidden" name="file" value="$file">
			<input type="hidden" name="ext" value="$ext">
			<tr class="tb_header">
				<td>你当前正在编辑 $path -> $file 模板文件 <a href="###" onclick="resizeup('filecontent');">[+]</a> <a href="###" onclick="resizedown('filecontent');">[-]</a></td>
			</tr>
			<tr class="tb_row" align="center">
				<td><textarea id="filecontent" cols="80" rows="25" name="content" style="width:650px;height:400px;">$contents</textarea></td>
			</tr>
			<tr><td><br /></td></tr>
			<tr align="center">
				<td><input type="submit" value="保存" /> <input type="reset" value="重置" /></td>
			</tr>
		</table>
	</form>
<!--
EOT;
} elseif ($action == 'del') {
	print <<<EOT
-->
	<form action="admincp.php?job=template" method="post" name="form">
		<input type="hidden" name="path" value="$path">
		<input type="hidden" name="file" value="$file">
		<input type="hidden" name="ext" value="$ext">
		<input type="hidden" name="action" value="delfile">

		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td>警告#</td>
			</tr>
			<tr class="tb_row">
				<td>你正在删除 [<a href="admincp.php?job=template&action=filelist&path=$path">$path</a>] 模板的 [<a href="admincp.php?job=template&action=mod&path=$path&file=$file">$file</a>] 文件。<b>注意: 删除模板文件将不会显示和该模板有关的一切页面，确定吗？</b></td>
			</tr>
			<tr><td><br /></td></tr>
			<tr align="center">
				<td><input type="submit" value="确认" /></td>
			</tr>
		</table>
	</form>
<!--
EOT;
}
print <<<EOT
-->
</div>
<!--
EOT;
?>
-->
