<?php

// 文件说明：后台首页信息


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

function getphpcfg($varname) {
	switch($result = get_cfg_var($varname)) {
		case 0:
			return '关闭';
			break;
		case 1:
			return '打开';
			break;
		default:
			return $result;
			break;
	}
}

function getfun($funName) {
	return (function_exists($funName)) ? '支持' : '不支持';
}

if (@ini_get('file_uploads')) {
	$fileupload = '允许 '.ini_get('upload_max_filesize');
} else {
	$fileupload = '<font color="red">禁止</font>';
}

$globals  = getphpcfg('register_globals');
$safemode = getphpcfg('safe_mode');
$gd_version = gd_version();
$gd_version = $gd_version ? '版本:'.$gd_version : '不支持';

$server['datetime'] = sadate('Y-m-d　H:i:s');
$server['software'] = $_SERVER['SERVER_SOFTWARE'];
if (function_exists('memory_get_usage')) {
	$server['memory_info'] = get_real_size(memory_get_usage());
}

$onlines = array();
$query = $DB->query("SELECT s.uid,s.ipaddress,s.lastactivity,s.groupid,u.username FROM {$db_prefix}sessions s LEFT JOIN {$db_prefix}users u ON (s.uid=u.userid) ORDER BY s.lastactivity");
while($online = $DB->fetch_array($query)) {
	$online['lastactivity'] = sadate('Y-m-d H:i:s', $online['lastactivity']);
	if ($online['groupid'] == 1 || $online['groupid'] == 2) {
		$onlines[] = $online;
	}
}
unset($online);
$DB->free_result($query);

$now_version = rawurlencode($TXP_VERSION);
$now_release = rawurlencode($TXP_RELEASE);
$now_hostname = rawurlencode($_SERVER['HTTP_HOST']);

cpheader();
include PrintEot('main');
?>
