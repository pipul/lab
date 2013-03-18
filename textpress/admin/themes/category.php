<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<div class="cp_category">
	<form action="admincp.php?job=category" method="POST">
		<table class="cp_center" width="500px" border="0"  cellspacing="0" cellpadding="0" >

<!--
EOT;
if($action == 'catelist'){
	print <<<EOT
-->
			<input type="hidden" name="action" value="doaddcate">

			<tr class="tb_header">
				<td width="20%">序列</td>
				<td width="25%">名称</td>
				<td width="40%">操作</td>
				<td width="15%">文章数</td>
			</tr>
<!--
EOT;
	if($catedb) {
		foreach($catedb as $key => $cate){
			print <<<EOT
-->
			<tr class="tb_row">
				<td>$cate[cid]</td>
				<td>$cate[name]</td>
				<td><a href="admincp.php?job=article&action=add&cid=$cate[cid]">添加文章</a> - <a href="admincp.php?job=category&action=modcate&cid=$cate[cid]">编辑</a> - <a href="admincp.php?job=category&action=delcate&cid=$cate[cid]">删除</a></td>
				<td>$cate[articles]</td>
			</tr>
<!--
EOT;
		}
	} else {
		print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="4">没有找到相关记录</td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<tr><td><br /></td></tr>
			<tr align="right">
				<td colspan="4"><input type="text" name="name" maxlength="50" value="">  <input type="submit" value="增加" /></td>
			</tr>
<!--
EOT;
} elseif ($action == 'modcate') {
	print <<<EOT
-->
			<input type="hidden" name="action" value="domodcate">
			<input type="hidden" name="cid" value="$cate[cid]">
			<tr class="tb_header">
				<td>序号</td>
				<td>名称</td>
			</tr>
			<tr class="tb_row">
				<td>$cate[cid]</td>
				<td><input type="text" name="name" maxlength="50" value="$cate[name]"></td>
			</tr>

			<tr><td><br /></td></tr>

			<tr>
				<td colspan="2" align="center"><input type="submit" value="确定"></td>
			</tr>
<!--
EOT;
} elseif ($action == 'delcate'){
	print <<<EOT
-->
			<input type="hidden" name="cid" value="$cate[cid]">
			<input type="hidden" name="action" value="dodelcate">
			<tr class="tb_header">
				<td>$cate[name]</td>
			</tr>
			<tr class="tb_row">
				<td>您确定要删除该分类吗? 本操作不可恢复，并会删除该分类中的所有文章、附件、评论</td>
			</tr>

			<tr><td><br /></td></tr>

			<tr align="center">
				<td><input type="submit" value="确认"></td>
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
