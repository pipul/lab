<?php

// 文件说明：后台管理公共函数库

// 加载公用函数
require_once('../include/common.php');

// 加载后台常用函数
require_once(TXP_ROOT.'admin/adminfunctions.php');

if (!$txp_uid || !$txp_pw || ($txp_group != 1 && $txp_group != 2)) {
	loginpage();
}

// 检查安装文件是否存在

if (file_exists('../install')) {

	exit('Installation directory: install/ is still on your server. Please DELETE it or RENAME it now.');

}


$action = $_GET['action'] ? $_GET['action'] : $_POST['action'];
$job    = $_GET['job'] ? $_GET['job'] : $_POST['job'];


// 登陆验证

if ($action == 'login') {

	$password = md5($_POST['password']);
	$userinfo = $DB->fetch_one_array("SELECT username,password,logincount,groupid FROM {$db_prefix}users WHERE userid='$txp_uid'");
	if ($userinfo['password'] == $password && $userinfo['logincount'] == $logincount && $userinfo['username'] == $txp_user && ($userinfo['groupid'] == 1 || $userinfo['groupid'] == 2)) {
		$adminhash = getadminhash($txp_uid,$txp_user,$password,$logincount);
		$admininfo = getadmininfo($password);
		setcookie('txp_admin', authcode("$txp_uid\t$adminhash\t$admininfo\t$onlineip"));
		$DB->query("DELETE FROM {$db_prefix}sessions WHERE uid='$txp_uid' OR lastactivity+1800<'$timestamp' OR hash='$adminhash'");
		$DB->query("INSERT INTO {$db_prefix}sessions (hash,uid,groupid,ipaddress,lastactivity) VALUES ('$adminhash', '$txp_uid', '$txp_group', '$onlineip', '$timestamp')");
		
		if ($_SERVER['QUERY_STRING']) {
			redirect('登陆成功,请稍候...', 'admincp.php'.$_SERVER['QUERY_STRING']);
		}

        redirect('登陆成功,请稍候...', 'admincp.php');

	} else {

		loginpage();

	}

}


// 验证用户是否处于登陆状态
list($admin_id, $admin_hash, $admin_info, $admin_ip) = $_COOKIE['txp_admin'] ? explode("\t", authcode($_COOKIE['txp_admin'], 'DECODE')) : array('', '', '', '');
$admin_id = intval($admin_id);
$admin_hash = addslashes($admin_hash);
$admin_info = addslashes($admin_info);
$admin_ip = addslashes($admin_ip);
if ($admin_id && $admin_hash && $admin_hash && $admin_ip) {
	$session = $DB->fetch_one_array("SELECT * FROM {$db_prefix}sessions WHERE uid='$admin_id' AND groupid='$txp_group' AND hash='$admin_hash' AND lastactivity+1800>'$timestamp' AND ipaddress='$admin_ip'");
	if (!$session) {
		$DB->query("DELETE FROM {$db_prefix}sessions WHERE uid='$admin_id' OR hash='$admin_hash'");
		loginpage();
	}
	$userinfo = $DB->fetch_one_array("SELECT userid,username,password,logincount,groupid FROM {$db_prefix}users WHERE userid='".$session['uid']."'");
	if (!$userinfo) {
		loginpage();
	}
	$adminhash = getadminhash($userinfo['userid'],$userinfo['username'],$userinfo['password'],$userinfo['logincount']);
	$admininfo = getadmininfo($userinfo['password']);
	if ($admin_hash != $adminhash || $admin_info != $admininfo || $admin_ip != $session['ipaddress']) {
		loginpage();
	}
	$DB->query("UPDATE {$db_prefix}sessions SET lastactivity='$timestamp' WHERE uid='$admin_id' AND hash='$admin_hash'");
} else {
	loginpage();

}
// 验证登陆状态结束

if ($action == 'logout') {
	$DB->query("DELETE FROM {$db_prefix}sessions WHERE uid='$admin_id' OR hash='$admin_hash'");
	setcookie('txp_admin', '');
	redirect('<b>注销成功, 请稍后...</b>', '../');
}

?>
