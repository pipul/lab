<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
if($action == 'list'){
	print <<<EOT
-->

<div class="cp_links">

	<form action="admincp.php?job=link" method="POST" name="form">
		<input type="hidden" name="action" value="addlink">
		<table width="300px" class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td colspan="2">添加链接</td>
			</tr>
			<tr class="tb_row">
				<td width="50%">状态:</td>
				<td><select name="visible"><option value="1" selected>显示</option><option value="0">隐藏</option></select></td>
			</tr>
			<tr class="tb_row">
				<td>名称:</td>
				<td><input name="name" type="text" value=""></td>
			</tr>
			<tr  class="tb_row">
				<td>地址:</td>
				<td><input name="url" type="text" value=""></td>
			</tr>
			<tr class="tb_row">
				<td>描述:</td>
				<td><input name="note" type="text" value=""></td>
			</tr>
			<tr><td><br /></td></tr>
			<tr align="center">
				<td colspan="2"><input type="submit" value="增加链接" /></td>
			</tr>
		</table>
	</form>
	<br />

	<form action="admincp.php?job=link" method="POST" name="form">
		<table width="960px" class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<input type="hidden" name="action" value="domorelink" />
			<tr class="tb_header">
				<td width="10%">排序</td>
				<td width="10%">状态</td>
				<td width="18%">名称</td>
				<td width="28%">地址</td>
				<td width="28%">描述</td>
				<td width="6%"><input name="chkall" value="on" type="checkbox" onclick="checkall(this.form)"></td>
			</tr>
<!--
EOT;
	if($linkdb) {
		foreach($linkdb as $key => $link){
			print <<<EOT
-->
			<tr class="tb_row">
				<td><input type="text" value="$link[displayorder]" name="displayorder[$link[linkid]]" size="1" /></td>
				<td><select name="visible[$link[linkid]]">$link[visible]</select></td>
				<td><input name="name[$link[linkid]]" type="text" value="$link[name]" size="15" /></td>
				<td><input name="url[$link[linkid]]" type="text" value="$link[url]" size="25" /></td>
				<td><input name="note[$link[linkid]]" type="text" value="$link[note]" size="25" /></td>
				<td><input type="checkbox" name="delete[]" value="$link[linkid]" /></td>
			</tr>
<!--
EOT;
		}
	} else {
		print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="6">没有找到相关记录</td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<tr><td><br /></td></tr>
			<tr>
				<td colspan="6" align="center"><input type="submit" value="更新 / 删除所选" /></td>
			</tr>
<!--
EOT;
}
print <<<EOT
-->
		</table>
	</form>
</div>
<!--
EOT;
?>
-->
