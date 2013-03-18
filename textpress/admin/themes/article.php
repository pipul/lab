<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<script type="text/javascript" src="./js/editor.js"></script>
<script type="text/javascript" src="./js/calendar.js"></script>

<script type="text/javascript">
	function checkform() {
		if ($('title') && $('title').value == "") {
			alert("请输入标题.");
			return false;
		}
		if ($('cid') && $('cid').value == "")	{
			alert("请选择分类.");
			return false;
		}
		$('submit').disabled = true;
		return true;
	}

	function addattach(attachid){
		addhtml('[attach=' + attachid + ']');
	}
	var smdirurl = '$smdirurl';

	function insertsmiley(icon){
		addhtml('<img src="'+smdirurl+icon+'" border="0" alt="" />');
	}
</script>


<div class="cp_article">
<!--
EOT;
if ($action == 'list') {
	print <<<EOT
-->
	<p align="center">[ 显示: <a href="admincp.php?job=article&action=list&view=stick&cid=$cid">置顶</a> | <a href="admincp.php?job=article&action=list&view=hidden&cid=$cid">隐藏</a> ] [<a href="admincp.php?job=article&action=add">添加文章</a>]</p><br />

	<form action="admincp.php?job=article" enctype="multipart/form-data" method="POST" name="form" onsubmit="return checkform();">

		<input type="hidden" name="action" value="list">
		<input type="hidden" name="do" value="search">
		<p align="center"><select name="cateid"><option value="">选择分类</option>
<!--
EOT;
	foreach($catedb as $key => $cate){
		$selected = ($cate['cid'] == $article['cid']) ? 'selected' : '';
		print <<<EOT
-->
					<option value="$cate[cid]" $selected>$cate[name]</option>
<!--
EOT;
	}
	print <<<EOT
-->
		</select>  <input type="text" name="keywords" maxlength="50" value="">  <input type="text" name="startdate" value="" id="startdate" onclick="return showCalendar('startdate', 'y-m-d');"> - <input type="text" name="enddate" value="" id="enddate" onclick="return showCalendar('enddate', 'y-m-d');">  <input type="submit" name="submit" id="submit" value="Go"></p>
		<br /><br />

	</form>
<!--
EOT;
}
print <<<EOT
-->

	<form action="admincp.php?job=article" enctype="multipart/form-data" method="POST" name="form" onsubmit="return checkform();">
		<table width="960px" class="cp_center" border="0" cellspacing="0" cellpadding="0">
<!--
EOT;
if ($action == 'list') {
	print <<<EOT
-->
			<tr class="tb_header">
				<td>标题</td>
				<td nowrap>分类</td>
				<td nowrap>评论</td>
				<td nowrap>附件</td>
				<td nowrap>时间</td>
				<td nowrap>操作</td>
				<td nowrap><input name="chkall" value="on" type="checkbox" onclick="checkall(this.form)"></td>
			</tr>
<!--
EOT;
	if($articledb) {
		foreach($articledb as $key => $article){
			print <<<EOT
-->
			<tr class="tb_row">
				<td><a href="admincp.php?job=article&action=visible&articleid=$article[articleid]">$article[visible]</a>
<!--
EOT;
			if($article['stick']){
				print <<<EOT
-->
[置顶]
<!--
EOT;
			}
			print <<<EOT
-->
					<a href="admincp.php?job=article&action=mod&articleid=$article[articleid]">$article[title]</a></td>
				<td><a href="admincp.php?job=article&action=list&cid=$article[cid]">$article[cname]</a></td>
				<td>$article[comments]</td>
				<td>$article[attachment]</td>
				<td>$article[dateline]</td>
				<td><a href="admincp.php?job=comment&action=cmlist&articleid=$article[articleid]">评论</a></td>
				<td><input type="checkbox" name="article[]" value="$article[articleid]"></td>
			</tr>
<!--
EOT;
		}
	} else {
		print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="7">没有找到相关记录</td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<tr><td><br /></td></tr>

			<tr class="tb_header">
				<td colspan="1">记录:$tatol</td>
				<td colspan="7">$multipage</td>
			</tr>



<!--
EOT;
} elseif (in_array($action, array('add', 'mod'))) {
	print <<<EOT
-->
			<tr class="tb_header">
				<td colspan="2">$tdtitle</td>
			</tr>
			<tr class="tb_row">
				<td>文章标题:</td>
				<td><input type="text" name="title" id="title" size="35" value="$article[title]"></td>
			</tr>
			<tr class="tb_row">
				<td>选择分类:</td>
				<td><select name="cid" id="cid">
					<option value="">分类目录</option>
<!--
EOT;
	foreach($catedb as $key => $cate){
		$selected = ($cate['cid'] == $article['cid']) ? "selected" : "";
		print <<<EOT
-->
					<option value="$cate[cid]" $selected>$cate[name]</option>
<!--
EOT;
	}
	print <<<EOT
-->
				</select></td>
			</tr>
			<tr class="tb_row">
				<td valign="top">文章描述:</td>
				<td>$descriptionarea</td>
			</tr>
			<tr class="tb_row">
				<td valign="top">文章内容:
<!--
EOT;
	if ($insertsm && $smfiles) {
		print <<<EOT
-->
					<div>
						<table cellpadding="0" cellspacing="0" border="0">
							<tr>
<!--
EOT;
		$br = 0;
		foreach($smfiles as $smile){
			$br++;
			print <<<EOT
-->
								<td><img src="../admin/fckeditor/editor/images/smiley/{$smile}" alt="" onclick="insertsmiley('$smile')" /></td>
<!--
EOT;
			if ($br >= 3) {
				print <<<EOT
-->
							</tr>
							<tr>
<!--
EOT;
				$br = 0;
			}
		}
		if ($br != 0) {
			for($i=$br; $i<3;$i++){
				print <<<EOT
-->
								<td></td>
<!--
EOT;
			}
		}
		print <<<EOT
-->
							</tr>
						</table>
					</div>
<!--
EOT;
	}
	print <<<EOT
-->
				</td>
				<td>$contentarea</td>
			</tr>
			<tr class="tb_row">
				<td>阅读密码:</td>
				<td><input type="text" name="readpassword" maxlength="20" value="$article[readpassword]"> 20个字符以内</td>
			</tr>
<!--
EOT;
	print <<<EOT
-->
			<tr class="tb_row">
				<td valign="top">更多选项:</td>
				<td>
					<input name="closecomment" type="checkbox" value="1" $closecomment_check>关闭评论<br />
					<input name="visible" type="checkbox" value="1" $visible_check>显示本文<br />
					<input name="stick" type="checkbox" value="1" $stick_check>本文置顶<br />
					<input name='edittime' type="checkbox" value="1">发布时间 <input name="newyear" type="text" value="$newyear" maxlength="4" style="width:30px"> 年 <input name="newmonth" type="text" value="$newmonth" maxlength="2" style="width:15px"> 月 <input name="newday" type="text" value="$newday" maxlength="2" style="width:15px"> 日 <input name="newhour" type="text" value="$newhour" maxlength="2" style="width:15px"> 时 <input name="newmin" type="text" value="$newmin" maxlength="2" style="width:15px"> 分 <input name="newsec" type="text" value="$newsec" maxlength="2" style="width:15px"> 秒 <input type="button" onclick="alert('有效的时间戳典型范围是从格林威治时间 1901 年 12 月 13 日 星期五 20:45:54 到 2038年 1 月 19 日 星期二 03:14:07\\n\\n该日期根据 32 位有符号整数的最小值和最大值而来\\n\\n取值说明: 日取 01 到 30 之间, 时取 0 到 24 之间, 分和秒取 0 到 60 之间!');" value="时间说明"></td>
			</tr>
<!--
EOT;
	if($attach_tatol > 0){
		print <<<EOT
-->
			<tr class="tb_row">
				<td valign="top">已上传的附件:</td>
				<td>
<!--
EOT;
		foreach($attachdb as $key => $attach){
			print <<<EOT
-->
					<input type="checkbox" name="keep[]" value="$attach[attachmentid]" checked> 保留 <a href="../attachment.php?id=$attach[attachmentid]" target="_blank"><b>$attach[filename]</b></a> ($attach[dateline], $attach[filesize]) <b> <a href="###" onclick="addattach('$attach[attachmentid]')">插入文章</a></b><br />
<!--
EOT;
		}
		print <<<EOT
-->
				</td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<tr class="tb_row">
				<td valign="top">上传新附件:</td>
				<td>
					<table cellspacing="0" cellpadding="0" border="0">
						<tbody id="attachbodyhidden" style="display:none"><tr><td><input type="file" name="attach[]" value=""/><span id="localfile[]"></span><input type="hidden" name="localid[]" /></td></tr></tbody>
						<tbody id="attachbody"></tbody>
					</table>
					<script type="text/javascript" src="./js/attachment.js"></script>
				</td>
			</tr>
		</table>

		<input type="hidden" name="action" value="$act">
		<input type="hidden" name="articleid" value="$articleid">
		<br />
		<table class="cp_center" cellpadding="0" cellspacing="0" border="0">
			<tr>
				<td><input type="submit" name="submit" id="submit" value="提交" /></td>
				<td><input type="reset" value="重置" /></td>
			</tr>

<!--
EOT;
} elseif ($_POST['do'] == 'move') {
	foreach($articledb as $key => $article){
		print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="3"><a href="admincp.php?job=article&action=mod&articleid=$article[articleid]">$article[title]</a><input type="hidden" name="article[]" value="$article[articleid]"></td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
				
			<tr class="tb_row" align="center">
				<td colspan="2">所选文章移动到</td>
				<td>
					<select name="cid">
<!--
EOT;
	foreach($catedb as $key => $cate){
		print <<<EOT
-->
						<option value="$cate[cid]">$cate[name]</option>
<!--
EOT;
	}
	print <<<EOT
-->
					</select>
				</td>
			</tr>
			<tr><td colspan="3"><br /></td></tr>
			<tr align="center">
				<td colspan="3">
					<input type="submit" name="submit" id="submit" value="确认">
					<input type="hidden" name="action" value="domove">
				</td>
			</tr>
			
<!--
EOT;
} elseif ($_POST['do'] == 'delete') {
	foreach($articledb as $key => $article){
		print <<<EOT
-->
			<tr class="tb_row">
				<td><a href="admincp.php?job=article&action=mod&articleid=$article[articleid]">$article[title]</a><input type="hidden" name="article[]" value="$article[articleid]"></td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<tr class="tb_header">
				<td>注意: 删除文章的同时会连同相关评论、引用、附件一起删除，确定吗？</td>
			</tr>

			<tr><td><br /></td></tr>

			<tr align="center">
				<td>
					<input type="submit" name="submit" id="submit" value="确认">
					<input type="hidden" name="action" value="dodelete">
				</td>
			</tr>
<!--
EOT;
}
print <<<EOT
-->
		</table>
<!--
EOT;
if ($action == 'list') {
	print <<<EOT
-->
		<table class="cp_center" cellpadding="0" cellspacing="0" border="0" height="40">
			<tr>
				<td align="right">
					<select name="do">
						<option value="">批量操作</option>
						<option value="stick">所选置顶</option>
						<option value="unstick">取消置顶</option>
						<option value="hidden">设置隐藏</option>
						<option value="display">设置可见</option>
						<option value="delete">批量删除</option>
						<option value="move">批量移动</option>
					</select>
					<input type="submit" name="submit" id="submit" value="确定" class="formbutton"><input type="hidden" name="action" value="domore">
				</td>
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
