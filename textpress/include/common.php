<?php

// 文件说明：前后台公用函数

error_reporting(0);
@header("content-Type: text/html; charset=UTF-8");
$mtime = explode(' ', microtime());
$starttime = $mtime[1] + $mtime[0];

define('TXP_ROOT', substr(dirname(__FILE__), 0, -7));
require_once(TXP_ROOT.'include/version.php');

$php_self = addslashes(htmlspecialchars($_SERVER['PHP_SELF'] ? $_SERVER['PHP_SELF'] : $_SERVER['SCRIPT_NAME']));
$timestamp = time();

// 防止 PHP 5.1.x 使用时间函数报错
if(function_exists('date_default_timezone_set')) {
	@date_default_timezone_set('UTC');
}

/* load the website config file */

require_once(TXP_ROOT.'config.php');



/* check the reflash and the proxy IP */

if($attackevasive) {

	if(!defined('TXP_ROOT')) {
		exit('Access Denied');
	}

	if($attackevasive == 1 || $attackevasive == 3) {
		if ($_COOKIE['lastrequest']) {
			list($lastrequest,$lastpath) = explode("\t",$_COOKIE['lastrequest']);
			$onlinetime = $timestamp - $lastrequest;
		} else {
			$lastrequest = $lastpath = '';
		}
		$REQUEST_URI  = $php_self.'?'.$_SERVER['QUERY_STRING'];
		if ($REQUEST_URI == $lastpath && $onlinetime < 2) {
	?>
	<html>
	<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta http-equiv="Refresh" content="2;url=<?php echo $REQUEST_URI;?>">
	<title>Refresh Limitation Enabled</title>
	</head>
	<body style="table-layout:fixed; word-break:break-all">
	<center>
	<div style="margin-top:100px;background-color:#f1f1f1;text-align:center;width:600px;padding:20px;margin-right: auto;margin-left: auto;font-family: Verdana, Tahoma; color: #666666; font-size: 12px">
	  <p><b>Refresh Limitation Enabled</b></p>
	  <p>The time between your two requests is smaller than 2 seconds, please do NOT refresh and wait for automatical forwarding ...</p>
	</div>
	</center>
	</body>
	</html>
	<?
			exit;
		}
		setcookie('lastrequest',$timestamp."\t".$REQUEST_URI);
	}

	if(($attackevasive == 2 || $attackevasive == 3) && ($_SERVER['HTTP_X_FORWARDED_FOR'] || $_SERVER['HTTP_VIA'] || $_SERVER['HTTP_PROXY_CONNECTION'] || $_SERVER['HTTP_USER_AGENT_VIA'])) {
	?>
	<html>
	<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<title>Proxy Connection Denied</title>
	</head>
	<body style="table-layout:fixed; word-break:break-all">
	<center>
	<div style="margin-top:100px;background-color:#f1f1f1;text-align:center;width:600px;padding:20px;margin-right: auto;margin-left: auto;font-family: Verdana, Tahoma; color: #666666; font-size: 12px">
	  <p><b>Proxy Connection Denied</b></p>
	  <p>Your request was forbidden due to the administrator has set to deny all proxy connection.</p>
	</div>
	</center>
	</body>
	</html>
	<?
		exit;
	}
}

/* load database class */
require_once(TXP_ROOT.'include/db.php');


/* initialize the database class */
$DB = new DB_MySQL;
$DB->connect($servername, $dbusername, $dbpassword, $dbname, $usepconnect);
unset($servername, $dbusername, $dbpassword, $dbname, $usepconnect);

/* get the IP address */
if(getenv('HTTP_CLIENT_IP') && strcasecmp(getenv('HTTP_CLIENT_IP'), 'unknown')) {
	$onlineip = getenv('HTTP_CLIENT_IP');
} elseif(getenv('HTTP_X_FORWARDED_FOR') && strcasecmp(getenv('HTTP_X_FORWARDED_FOR'), 'unknown')) {
	$onlineip = getenv('HTTP_X_FORWARDED_FOR');
} elseif(getenv('REMOTE_ADDR') && strcasecmp(getenv('REMOTE_ADDR'), 'unknown')) {
	$onlineip = getenv('REMOTE_ADDR');
} elseif(isset($_SERVER['REMOTE_ADDR']) && $_SERVER['REMOTE_ADDR'] && strcasecmp($_SERVER['REMOTE_ADDR'], 'unknown')) {
	$onlineip = $_SERVER['REMOTE_ADDR'];
}
$onlineip = addslashes($onlineip);
@preg_match("/[\d\.]{7,15}/", $onlineip, $onlineipmatches);
$onlineip = $onlineipmatches[0] ? $onlineipmatches[0] : 'unknown';
unset($onlineipmatches);

// 读取系统配置
$options = array();
$query = $DB->query("SELECT title, value FROM {$db_prefix}settings");
while ($setting = $DB->fetch_array($query)) {
	$options[$setting['title']] = $setting['value'];
}
unset($setting);
$DB->free_result($query);


if ($options['gzipcompress'] && function_exists('ob_gzhandler')) {
	@ob_start('ob_gzhandler');
} else {
	$options['gzipcompress'] = 0;
	ob_start();
}
!$options['templatename'] && $options['templatename'] = 'default';
$options['title'] = $options['name'];
$timeoffset = (!$options['server_timezone'] || $options['server_timezone'] == '111') ? 0 : $options['server_timezone'];

// 检查主机是否支持mod_rewrite
if (function_exists('apache_get_modules')) {
	$apache_mod = apache_get_modules();
	if (!in_array('mod_rewrite',$apache_mod)) {
		$options['rewrite_enable'] = 0;
	}
}

$options = stripslashes_array($options);
$page = intval($_GET['page']);

// 允许程序在 register_globals = off 的环境下工作
$onoff = function_exists('ini_get') ? ini_get('register_globals') : get_cfg_var('register_globals');
if ($onoff != 1) {
	@extract($_POST, EXTR_SKIP);
	@extract($_GET, EXTR_SKIP);
	@extract($_COOKIE, EXTR_SKIP);
}

// 去除转义字符
function stripslashes_array(&$array) {
	if (is_array($array)) {
		foreach ($array as $k => $v) {
			$array[$k] = stripslashes_array($v);
		}
	} else if (is_string($array)) {
		$array = stripslashes($array);
	}
	return $array;
}

@set_magic_quotes_runtime(0);
// 判断 magic_quotes_gpc 状态
if (@get_magic_quotes_gpc()) {
    $_GET = stripslashes_array($_GET);
    $_POST = stripslashes_array($_POST);
    $_COOKIE = stripslashes_array($_COOKIE);
}

// 系统URL
if (!$options['url']) {
	//HTTP_HOST已经包含端口信息,不必加SERVER_PORT了.
	$options['url'] = 'http://'.$_SERVER['HTTP_HOST'].dirname($php_self).'/';
} else {
	$options['url'] = str_replace(array('{host}','index.php'), array($_SERVER['HTTP_HOST'],''), $options['url']);
	if (substr($options['url'], -1) != '/') {
		$options['url'] = $options['url'].'/';
	}
}

// 文章排列依据
$article_order = in_array($options['article_order'], array('dateline', 'articleid')) ? $options['article_order'] : 'dateline';

// 前台身份验证
list($txp_uid, $txp_pw, $logincount) = $_COOKIE['txp_auth'] ? explode("\t", authcode($_COOKIE['txp_auth'], 'DECODE')) : array('', '', '');
$txp_uid = intval($txp_uid);
$txp_pw = addslashes($txp_pw);
$txp_group = 4;
if (!$txp_uid || !$txp_pw) {
	$txp_uid = 0;
} else {
	$user = $DB->fetch_one_array("SELECT username,password,logincount,groupid FROM {$db_prefix}users WHERE userid='$txp_uid'");
	if ($user['password'] == $txp_pw && $user['logincount'] == $logincount) {
		$txp_user = $user['username'];
		$txp_group = $user['groupid'];
	} else {
		$txp_uid = 0;
		$txp_pw = '';
		setcookie('txp_auth', '');
	}
}
if($txp_group == 1) {
	error_reporting(E_ERROR | E_WARNING | E_PARSE);
}
$formhash = formhash();


// 格式化时间
function sadate($format,$timestamp=''){
	global $options,$timeoffset;
	!$timestamp && $timestamp = time();
	return gmdate($format,$timestamp+$timeoffset*3600);
}

// 获得散列
function formhash() {
	global $txp_user, $txp_uid, $txp_pw, $timestamp;
	return substr(md5(substr($timestamp, 0, -7).$txp_user.$txp_uid.$txp_pw), 8, 8);
}

// 获得某年某月的时间戳
function gettimestamp($year, $month) {
/*	
	$start = mktime(0,0,0,$month,1,$year);
	if ($month == 12) {
		$nextyear = $year + 1;
		$nextmonth = $month;
	} else {
		$nextmonth = $month + 1;
		$nextyear = $year;
	}
	$end = mktime(0,0,0,$nextmonth,1,$nextyear);
*/
	$start = strtotime($year.'-'.$month.'-1');
	if ($month == 12) {
		$endyear  = $year + 1;
		$endmonth = 1;
	} else {
		$endyear  = $year;
		$endmonth = $month+1;
	}
	$end = strtotime($endyear.'-'.$endmonth.'-1');
	return $start.'-'.$end;
}

function correcttime($timestamp) {
	global $timeoffset;
	$z = date('Z');
	if ($z != '0') {
		$timestamp = $timestamp - ($z - $timeoffset * 3600);
	} else {
		$timestamp = $timestamp - $timeoffset * 3600;
	}
	return $timestamp;
}

// 截取字数
function trimmed_title($text, $limit=12) {
	if ($limit) {
		$val = csubstr($text, 0, $limit);
		return $val[1] ? $val[0]."..." : $val[0];
	} else {
		return $text;
	}
}

// 判断是否为邮件地址
function isemail($email) {
	return strlen($email) > 6 && preg_match("/^[\w\-\.]+@[\w\-\.]+(\.\w+)+$/", $email);
}

function csubstr($text, $start=0, $limit=12) {
	if (function_exists('mb_substr')) {
		$more = (mb_strlen($text, 'UTF-8') > $limit) ? TRUE : FALSE;
		$text = mb_substr($text, 0, $limit, 'UTF-8');
		return array($text, $more);
	} elseif (function_exists('iconv_substr')) {
		$more = (iconv_strlen($text) > $limit) ? TRUE : FALSE;
		$text = iconv_substr($text, 0, $limit, 'UTF-8');
		return array($text, $more);
	} else {
		preg_match_all("/[\x01-\x7f]|[\xc2-\xdf][\x80-\xbf]|\xe0[\xa0-\xbf][\x80-\xbf]|[\xe1-\xef][\x80-\xbf][\x80-\xbf]|\xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]|[\xf1-\xf7][\x80-\xbf][\x80-\xbf][\x80-\xbf]/", $text, $ar);   
		if(func_num_args() >= 3) {   
			if (count($ar[0])>$limit) {
				$more = TRUE;
				$text = join("",array_slice($ar[0],0,$limit))."..."; 
			} else {
				$more = FALSE;
				$text = join("",array_slice($ar[0],0,$limit)); 
			}
		} else {
			$more = FALSE;
			$text =  join("",array_slice($ar[0],0)); 
		}
		return array($text, $more);
	} 
}

// 转换字符
function char_cv($string) {
	$string = htmlspecialchars(addslashes($string));
	return $string;
}

// 页面输出
function PageEnd() {
	global $options;
	$output = str_replace(array('<!--<!---->','<!---->'),array('',''),ob_get_contents());
	ob_end_clean();
	$options['gzipcompress'] ? ob_start('ob_gzhandler') : ob_start();
	if ($options['rewrite_enable']) {
		require_once(TXP_ROOT.'include/rewrite.php');
		$searcharray = array (
			"/\<a href\=\"(\.*\/*)\?action\=index&amp;cid\=(\d+)(&amp;page\=(\d+))?\"( class\=\"(.+?)\")?\>/e",
			"/\<a href\=\"(\.*\/*)\?action\=index&amp;setdate\=(\d+)(&amp;page\=(\d+))?\"( class\=\"(.+?)\")?\>/e",
			"/\<a href\=\"(\.*\/*)\?action\=show&amp;id\=(\d+)(&amp;page\=(\d+))?(#(\w+))?\"( class\=\"(.+?)\")?\>/e",
			"/\<a href\=\"(\.*\/*)\?action\=(search|reg|login|archives|comments|index|links)(&amp;page\=(\d+))?\"( class\=\"(.+?)\")?\>/e",
		);
		$replacearray = array (
			"rewrite_category('\\1', '\\2', '\\4', '\\6')",
			"rewrite_archives('\\1', '\\2', '\\4', '\\6')",
			"rewrite_post('\\1', '\\2', '\\4', '\\6', '\\8')",
			"rewrite_page('\\1', '\\2', '\\4', '\\6')",
		);
		/*
		//正则真他妈烦人
		*/
		$output = preg_replace($searcharray, $replacearray, $output);
	}
	echo $output;
	exit;
}

// base64编码函数
function authcode($string, $operation = 'ENCODE') {
	$string = $operation == 'DECODE' ? base64_decode($string) : base64_encode($string);
	return $string;
}

?>
