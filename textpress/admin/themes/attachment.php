<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<div class="cp_attach">
	<p align="center">[附件管理:<a href="admincp.php?job=attachment&action=repair">附件修复</a> <a href="admincp.php?job=attachment&action=clear">附件清理</a>] [ 查看类型: <a href="admincp.php?job=attachment&action=list&view=image&articleid=$articleid">图片附件</a> <a href="admincp.php?job=attachment&action=list&view=file&articleid=$articleid">文件附件</a> ]</p>
	<br />
	<table class="cp_center" border="0" cellspacing="0" cellpadding="0">

<!--
EOT;
if($action == 'list'){
	if (!$articleid) {
		print <<<EOT
-->
		<tr class="tb_header">
			<td colspan="5">附件概要信息</td>
		</tr>
		<tr class="tb_row">
			<td>存档路径: $a_dir$warning</td>
			<td>子目录: $dircount</td>
			<td>附件数量: $stats[count]</td>
			<td>附件大小: $stats[sum]</td>
			<td>磁盘占用: $realattachsize</td>
		</tr>

<!--
EOT;
	} else {
		print <<<EOT
-->
		<form action="admincp.php?job=attachment" method="post" enctype="multipart/form-data">
			<input type="hidden" name="action" value="addattachtoarticle" />
			<input type="hidden" name="articleid" value="$articleid" />
			<tr class="tb_header">
				<td colspan="2">上传新附件到该文章 <a href="###" onclick="addattachfrom();">[+]</a>&nbsp;<a href="###" onclick="removeattachfrom();">[-]</a></td>
			</tr>
			<tr class="tb_row">
				<td colspan="2">图片超过2M缩略图和水印均不生效.如果上传大于2M的图片请自行处理.</td>
			</tr>
			<tbody id="attachbodyhidden" style="display:none"><tr class="tb_row"><td>附件:</td><td><input type="file" name="attach[]"></td></tr></tbody>
			<tbody id="attachbody"><tr class="tb_row"><td>附件:</td><td><input type="file" name="attach[]"></td></tr></tbody>

			<tr><td><br /></td></tr>

			<tr>
				<td colspan="2" align="center"><input type="submit" value="上传" /></td>
			</tr>
		</form>
<!--
EOT;
	}
	print <<<EOT
-->
	</table>

	<br /><br /><br />

	<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
		<form action="admincp.php?job=attachment"  method="post">
			<input type="hidden" name="action" value="delattachments" />
			<input type="hidden" name="articleid" value="$articleid" />
			<tr class="tb_header">
				<td>附件名</td>
				<td>附件信息</td>
				<td>所在目录</td>
				<td>缩略图</td>
				<td>上传时间</td>
				<td>下载次数</td>
				<td>文章</td>
				<td><input name="chkall" value="on" type="checkbox" onclick="checkall(this.form)" /></td>
			</tr>
<!--
EOT;
	if($attachdb) {
		foreach($attachdb as $key => $attach){
			print <<<EOT
-->
			<tr class="tb_row">
				<td><a href="../attachment.php?id=$attach[attachmentid]" target="_blank" title="$attach[filepath]">$attach[filename]</a></td>
				<td>大小:$attach[filesize]<br />类型:$attach[filetype]</td>
				<td>$attach[subdir]</td>
				<td>$attach[thumb]</td>
				<td>$attach[dateline]</td>
				<td>$attach[downloads]</td>
				<td><a title="$attach[article]" href="../?action=show&id=$attach[articleid]" target="_blank">查看</a></td>
				<td><input type="checkbox" name="attachment[]" value="$attach[attachmentid]" /></td>
			</tr>
<!--
EOT;
		}
	} else {
		print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="8">没有找到相关记录</td>
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


			<tr><td><br /></td></tr>
			<tr>
				<td colspan="8" align="center"><input type="submit" value="删除所选附件" /></td>
			</tr>
		</form>
<!--
EOT;
} elseif ($action == 'repair') {
	print <<<EOT
-->
		<form action="admincp.php?job=attachment" method="post">
			<input type="hidden" name="action" value="dorepair" />
			<tr class="tb_header">
				<td>附件修复</td>
			</tr>
			<tr class="tb_row">
				<td><p>本功能清除数据库那存在附件记录而没有附件文件的冗余数据，文章中的附件记录也将同时更新。如果附件较多，过程会比较久，请耐心等候。建议定期执行。</p></td>
			</tr>
			<tr><td><br /></td></tr>
			<tr align="center">
				<td><input type="submit" value="确认"></p></td>
			</tr>
		</form>
<!--
EOT;
} elseif ($action == 'clear') {
	print <<<EOT
-->
		<form action="admincp.php?job=attachment" method="post">
			<input type="hidden" name="action" value="doclear" />
			<tr class="tb_header">
				<td>附件清理</td>
			</tr>
			<tr class="tb_row">
				<td><p>本功能删除数据库中没有记录而实际存在的附件，可有效清理冗余附件。循环处理数量: <input type="text" name="percount" value="500" ></p></td>
			</tr>
			
			<tr><td><br /></td></tr>

			<tr align="center">
				<td><input type="submit" value="Go"></td>
			</tr>
		</form>
<!--
EOT;
}
print <<<EOT
-->
	</table>
</div>
<!--
EOT;
?>
-->
