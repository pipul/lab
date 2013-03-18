<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<div class="cp_database">
<!--
EOT;

if ($action == 'mysqltools') {
	print <<<EOT
-->
	<form action="admincp.php?job=database" enctype="multipart/form-data" method="POST" name="form">
		<input type="hidden" name="action" value="dobackup">
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td colspan="10">数据库备份</td>
			</tr>
			<tr class="tb_row">
				<td>版本</td>
				<td><select name="sqlcompat">
					<option value="" selected>默认</option>
					<option value="MYSQL40">MySQL 4.0.x</option>
					<option value="MYSQL41">MySQL 4.1.x/5.x</option>
				</select></td>
				<td>方式</td>
				<td><select name="extendins">
					<option value="1">扩展</option>
					<option value="0" selected>默认</option>
				</select></td>
				<td>限定字符集</td>
				<td><select name="addsetnames">
					<option value="1">是</option>
					<option value="0" selected>否</option>
				</select></td>
				<td>单卷配额</td>
				<td><input type="text" name="sizelimit" size="5" maxlength="20" value="2048"> KB</td>
				<td>备份名:</td>
				<td><input type="text" name="filename" maxlength="40" size="30"  value="$backuppath.sql"></td>
			</tr>

			<tr><td><br /></td></tr>

			<tr align="right">
				<td colspan="10"><input type="submit" value="执行备份" /></td>
			</tr>

			<tr><td><br /></td></tr>

		</table>
	</form>

	<form action="admincp.php?job=database" enctype="multipart/form-data" method="POST" name="form">
		<input type="hidden" name="action" value="dotools">
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td colspan="8">数据库优化</td>
			</tr>
			<tr class="tb_row">
				<td>检查表</td>
				<td><input type="checkbox" name="do[]" value="check" checked /></td>
				<td>修复表</td>
				<td><input type="checkbox" name="do[]" value="repair" checked /></td>
				<td>分析表</td>
				<td><input type="checkbox" name="do[]" value="analyze" checked /></td>
				<td>优化表</td>
				<td><input type="checkbox" name="do[]" value="optimize" checked /></td>
			</tr>

			<tr><td><br /></td></tr>

			<tr align="right">
				<td colspan="8"><input type="submit" value="执行优化" /></td>
			</tr>

			<tr><td><br /></td></tr>

		</table>
	</form>

	<form action="admincp.php?job=database" enctype="multipart/form-data" method="POST" name="form">
		<input type="hidden" name="action" value="deldbfile">
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td colspan="8">数据库恢复</td>
			</tr>
			<tr class="tb_row">
				<td colspan="8">注意数备份文件与当前数据库前缀是否相同且恢复操作只允许从卷号1的文件开始恢复数据.</td>
			</tr>
    
			<tr class="tb_header">
				<td>备份名</td>
				<td>创建时间</td>
				<td>修改时间</td>
				<td>程序版本</td>
				<td>分卷号</td>
				<td>文件大小</td>
				<td>操作</td>
				<td><input name="chkall" value="on" type="checkbox" onclick="checkall(this.form)"></td>
			</tr>
<!--
EOT;
	if ($noexists) {
		print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="8">目录不存在或无法访问, 请检查 $backupdir 目录是否具有写入权限</td>
			</tr>
<!--
EOT;
	} else {
		if($dbfiles) {
			foreach($dbfiles as $key => $dbfile){
				print <<<EOT
-->
			<tr class="tb_row">
				<td><a href="$backupdir/$dbfile[filename]">$dbfile[filename]</a></td>
				<td>$dbfile[bktime]</td>
				<td>$dbfile[mtime]</td>
				<td>$dbfile[version]</td>
				<td>$dbfile[volume]</td>
				<td>$dbfile[filesize]</td>
<!--
EOT;
				if ($dbfile['volume'] == '1') {
					print <<<EOT
-->
				<td><a href="admincp.php?job=database&action=checkresume&sqlfile=$dbfile[filepath]">导入</a></td>
<!--
EOT;
				} else {
					print <<<EOT
-->
				<td>无</td>
<!--
EOT;
				}
				print <<<EOT
-->
				<td><input type="checkbox" name="sqlfile[$backupdir/$dbfile[filename]]" value="1"></td>
			</tr>
<!--
EOT;
			}
		} else {
			print <<<EOT
-->
			<tr class="tb_row">
				<td colspan="8">没有找到相关的备份文件</td>
			</tr>

<!--
EOT;
		}
	}
	print <<<EOT
-->
			<tr class="tb_header">
				<td colspan="8">{$file_i} 个备份文件</td>
			</tr>

			<tr><td><br /></td></tr>

			<tr align="right">
				<td colspan="8"><input type="submit" value="删除备份" /></td>
			</tr>

			<tr><td><br /></td></tr>

		</table>
	</form>
<!--
EOT;
} elseif($action == 'dotools') {
	foreach ($dodb AS $do) {
		print <<<EOT
-->
	<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
		<tr class="tb_header">
			<td colspan="2">$do[name]</td>
		</tr>
<!--
EOT;
		foreach($tabledb as $table){
			if ($table['do'] == $do['do']) {
				print <<<EOT
-->
		<tr class="tb_row">
			<td>$table[table]</td>
			<td align="right">$table[result]</td>
		</tr>
<!--
EOT;
			}
		}
		print <<<EOT
-->

	</table>
<!--
EOT;

	}
} elseif ($action == 'checkresume') {
	print <<<EOT
-->
	<form action="admincp.php?job=database" enctype="multipart/form-data" method="POST" name="form">
		<input type="hidden" name="action" value="resume">
		<input type="hidden" name="sqlfile" value="$sqlfile">
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td>数据导入提示</td>
			</tr>
			<tr class="tb_row">
				<td>$sqlfile 文件正在准备导入. 注意:恢复功能将覆盖原来的数据,您确认要导入备份数据吗?</td>
			</tr>

			<tr><td><br /></td></tr>

			<tr>
				<td><input type="submit" value="确认"></td>
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
