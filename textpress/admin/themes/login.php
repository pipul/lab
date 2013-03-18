<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<style type="text/css">
	.tb_login td {
		padding: 5px 10px 5px 10px;
	}
</style>
<div class="cp_login">
	<table class="cp_center" cellspacing="0" cellpadding="0" width="400px">
		<tr class="tb_header">
			<td>Login to TextPress</td>
		</tr>

		<tr><td><br /></td></tr>

<!--
EOT;
if ($txp_uid && ($txp_group == 1 || $txp_group == 2)) {
	print <<<EOT
-->
		<form method="post" action="admincp.php">
			<input type="hidden" name="action" value="login" />
			<tr class="tb_login">
				<td>Wellcome $txp_user, Please input the...  Password:</td>
			</tr>

			<tr><td><br /></td></tr>

			<tr class="tb_login">
				<td><input type="password" name="password" value="" style="width:390px;"/></td>
			</tr>

			<tr><td><br /></td></tr>

			<tr class="tb_login" align="right">
				<td><input type="submit" value="Go" /></td>
			</tr>
		</form>
<!--
EOT;
} else {
	print <<<EOT
-->
		<tr class="tb_login">
			<td>Warning:Permission denied</td>
		</tr>
<!--
EOT;
}
print <<<EOT
-->
	</table>
</div>
<!--
EOT;
?>-->
