<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<div class="cp_comments">
	<p align="center">[ 查看评论: 
    <!--
EOT;
if ($articleid) {
	print <<<EOT
-->
	<a href="admincp.php?job=comment&action=cmlist&articleid=$articleid">文章内全部</a> |
<!--
EOT;
}
print <<<EOT
-->
	<a href="admincp.php?job=comment&action=cmlist&kind=hidden&articleid=$articleid">已隐藏</a> | <a href="admincp.php?job=comment&action=cmlist&kind=display&articleid=$articleid">已显示</a> ]</p><br />



	<form action="admincp.php?job=comment" method="POST" name="form">
<!--
EOT;
if($action == 'cmlist'){
	print <<<EOT
-->
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td>状态</td>
				<td>作者</td>
				<td>联系方法</td>
				<td>IP</td>
				<td>时间</td>
				<td>内容</td>
				<td><input name="chkall" value="on" type="checkbox" onClick="checkall(this.form)"></td>
			</tr>
<!--
EOT;
	if($commentdb) {
		foreach($commentdb as $key => $comment){
			print <<<EOT
-->
			<tr class="tb_row">
				<td><a href="admincp.php?job=comment&action=cmvisible&commentid=$comment[commentid]">$comment[visible]</a></td>
				<td>$comment[author]</td>
				<td>$comment[url]</td>
				<td><a href="admincp.php?job=comment&action=cmlist&ip=$comment[ipaddress]" title="查看此IP同一C段发表的评论">$comment[ipaddress]</a></a></td>
				<td>$comment[dateline]</td>
				<td><a href="admincp.php?job=comment&action=modcm&commentid=$comment[commentid]">$comment[content]</a></td>
				<td><input type="checkbox" name="comment[]" value="$comment[commentid]"></td>
			</tr>
<!--
EOT;
		}
	} else {
		print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="7">没有找到相关的记录</td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<input type="hidden" name="articleid" value="$articleid">

			<tr><td><br /></td></tr>

			<tr class="tb_header">
				<td colspan="2">记录:$tatol</td>
				<td colspan="5">$multipage</td>
			</tr>
			<tr>
				<td class="tablebottom" colspan="9"></td>
			</tr>
		</table>

		<table class="cp_center" cellpadding="0" cellspacing="0" border="0">
			<tr>
				<td align="right">
					<select name="do">
						<option value="">批量管理</option>
						<option value="hidden">隐藏选定</option>
						<option value="display">显示选定</option>
						<option value="del">删除选定</option>
					</select>
					<input type="submit" value="确定" >
					<input type="hidden" name="action" value="domore{$action}">
				</td>
			</tr>
		</table>
		
<!--
EOT;
} elseif ($action == 'modcm') {
	print <<<EOT
-->
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td colspan="2"><a name="编辑评论"></a>编辑评论</td>
			</tr>
			<tr class="tb_row">
				<td>所在文章:</td>
				<td><a href="admincp.php?job=article&action=mod&articleid=$comment[articleid]">$comment[title]</a></td>
			</tr>
			<tr class="tb_row">
				<td>评论作者:</td>
				<td><input class="formfield" type="text" name="author" size="50" value="$comment[author]"></td>
			</tr>
			<tr class="tb_row">
				<td>评论作者联系方式:</td>
				<td><input class="formfield" type="text" name="url" size="50" value="$comment[url]"></td>
			</tr>
			<tr class="tb_row">
				<td valign="top">评论内容:</td>
				<td><textarea class="formarea" type="text" name="content" cols="75" rows="20">$comment[content]</textarea></td>
			</tr>
			<input type="hidden" name="commentid" value="$comment[commentid]">
			<input type="hidden" name="articleid" value="$comment[articleid]">
			<input type="hidden" name="action" value="domodcm">
			<tr class="tb_row">
				<td colspan="2" align="center">
					<input type="submit" value="提交" class="formbutton">
					<input type="reset" value="重置" class="formbutton">
				</td>
			</tr>
			<tr>
				<td class="tablebottom" colspan="9"></td>
			</tr>
		</table>
<!--
EOT;
} elseif ($action == 'delallcm') {
	print <<<EOT
-->
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="alertheader">
				<td>删除所有评论</td>
			</tr>
			<tr>
				<td class="alertbox"><p><b>如果不想显示所有评论,隐藏所有评论即可.</b></p><p><b>这是危险操作且不可恢复,确实要删除所有评论吗?</b></p><p><input type="submit" value="确认" class="formbutton"><input type="hidden" name="action" value="dodelallcm"></p></td>
			</tr>
			<tr>
				<td class="tablebottom" colspan="9"></td>
			</tr>
		</table>
<!--
EOT;
}
print <<<EOT
-->
	</form>
</div>
<!--
EOT;
?>
-->
