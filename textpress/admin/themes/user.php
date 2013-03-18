<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<div class="cp_users">
	<form action="admincp.php?job=user" method="POST">
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
		
			<tr class="tb_header">
				<td colspan="2">必填信息</td>
			</tr>
			<tr class="tb_row">
				<td>用户名</td>
				<td><input type="text" name="username" value="$info[username]" /></td>
			</tr>
			<tr class="tb_row">
				<td>新密码:</td>
				<td><input type="password" name="newpassword" value="" /></td>
			</tr>
			<tr class="tb_row">
				<td>确认新密码:</td>
				<td><input type="password" name="comfirpassword" value="" /></td>
			</tr>
			<tr class="tb_row">
				<td>用户组:</td>
				<td><select name="groupid">
					<option value="1" $groupselect[1]>管理员</option>
					<option value="2" $groupselect[2]>撰写组</option>
				</select></td>
			</tr>
			<tr><td><br /></td></tr>
			<tr class="tb_header">
				<td colspan="2">选填信息:</td>
			</tr>
			<tr class="tb_row">
				<td>电子邮件或主页:</td>
				<td><input type="text" name="url" value="$info[url]" /></td>
			</tr>

			<input type="hidden" name="userid" value="$info[userid]">
<!--
EOT;
if ($action == 'mod') {
	print <<<EOT
-->
			<input type="hidden" name="action" value="moduser">
<!--
EOT;
} else {
	print <<<EOT
-->
			<input type="hidden" name="action" value="adduser">
<!--
EOT;
}
print <<<EOT
-->
			<tr><td><br /></td></tr>
			<tr align="center">
				<td colspan="2"><input type="submit" value="添加用户" /></td>
			</tr>
		</table>
	</form>
	
	<br />
<!--

EOT;
if ($action == 'list') {
	print <<<EOT
-->
	<form action="admincp.php?job=user" method="POST">
		<table class="cp_center" border="0" cellspacing="0" cellpadding="0">
			<tr class="tb_header">
				<td><b>用户名</b></td>
				<td><b>用户组</b></td>
				<td><b>联系方法</b></td>
				<td><b>注册时间</b></td>
				<td><b>登陆次数</b></td>
				<td><b>上次登陆IP</b></td>
				<td><b>上次登陆时间</b></td>
				<td><b>最后发表评论时间</b></td>
				<td><input name="chkall" value="on" type="checkbox" onclick="checkall(this.form)" /></td>
			</tr>
<!--
EOT;
	foreach($userdb as $key => $user){
		print <<<EOT
-->
			<tr class="tb_row">
				<td><a href="admincp.php?job=user&action=mod&userid=$user[userid]">$user[username]</a></td>
				<td>$user[group]</td>
				<td>$user[url]</td>
				<td>$user[regdateline]</td>
				<td>$user[logincount]</td>
				<td><a href="admincp.php?job=user&action=list&ip=$user[loginip]">$user[loginip]</a></td>
				<td>$user[logintime]</td>
				<td>$user[lastpost]</td>
				<td><input type="checkbox" name="user[]" value="$user[userid]" $user[disabled] /></td>
			</tr>
<!--
EOT;
	}
	print <<<EOT
-->
			<tr><td><br /></td></tr>

			<tr class="tb_header">
				<td colspan="2">记录:$tatol</td>
				<td colspan="7">$multipage</td>
			</tr>

			<tr><td><br /></td></tr>

			<tr>
				<td colspan="9" align="center"><input type="hidden" name="action" value="delusers" /><input type="submit" value="删除所选用户" /></td>
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
