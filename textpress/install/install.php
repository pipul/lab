<?php
error_reporting(0);

define('TXP_ROOT', TRUE);
ob_start();
define('TXP_VERSION', '1.6');

// 允许程序在 register_globals = off 的环境下工作
$onoff = function_exists('ini_get') ? ini_get('register_globals') : get_cfg_var('register_globals');
if ($onoff != 1) {
	@extract($_POST, EXTR_SKIP);
	@extract($_GET, EXTR_SKIP);
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

// 判断 magic_quotes_gpc 状态
if (get_magic_quotes_gpc()) {
    $_GET = stripslashes_array($_GET);
    $_POST = stripslashes_array($_POST);
}

set_magic_quotes_runtime(0);

$step = $_GET['step'] ? $_GET['step'] : $_POST['step'];
$php_self = $_SERVER['PHP_SELF'] ? $_SERVER['PHP_SELF'] : $_SERVER['SCRIPT_NAME'];
$dbcharset = 'utf8';
$configfile = '../config.php';


$sqlfile = 'install.sql';
if(!is_readable($sqlfile)) {
	exit('Database file does not exists.');
}
$fp = fopen($sqlfile, 'rb');
$sql = fread($fp, 2048000);
fclose($fp);

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>TextPress安装脚本</title>
<link href="install.css" rel="stylesheet" type="text/css" />
</head>
<body>


<div id="main">
  <form method="post" action="<?php echo $php_self;?>">
<?php
if (!$step || $step == '1') {
?>
    <p>Welcome to TextPress. Before getting started, we need some information on the database. You will need to know the following items before proceeding.</p>
<ol>
	<li>Database name</li>
	<li>Database username</li>
	<li>Database password</li>
	<li>Database host</li>
	<li>Table prefix (if you want to run more than one TextPress in a single database) </li>
</ol>
	<p><strong>If for any reason this automatic file creation doesn't work, don't worry. All this does is fill in the database information to a configuration file. You may also simply open config.php in a text editor, fill in your information, and save it. </strong></p>

	<p>In all likelihood, these items were supplied to you by your Web Host. If you do not have this information, then you will need to contact them before you can continue.</p>
    <p>
      <input type="hidden" name="step" value="2" />
      <input type="submit" value="Let's Go" />
    </p>
    <?php
} elseif ($step == '2') {
	
	$exist_error = FALSE;
	$write_error = FALSE;
	if (file_exists($configfile)) {
		$fileexists = result(1, 0);
	} else {
		$fileexists = result(0, 0);
		$exist_error = TRUE;
	}
	if (is_writeable($configfile)) {
		$filewriteable = result(1, 0);
	} else {
		$filewriteable = result(0, 0);
		$write_error = TRUE;
	}
	if ($exist_error) {
		$config_info = 'Warning:config.php not fount !';
	} elseif($write_error) {
		$config_info = 'Warning:config.php can not write, Please check the file permission.';
	}
?>
    <p>Below you should enter your database connection details. If you're not sure about these, contact your host. </p>
    <p>config.php exists <?php echo $fileexists;?></p>
    <p>config.php writeable <?php echo $filewriteable;?></p>
<?php
	if ($config_info) {
?>
    <p><?php echo $config_info;?></p>
    <p>
      <input type="button" value="Back" onclick="history.back(1)" />
    </p>
<?php
	} else {
?>
    <table width="100%" align="center" border="0" cellspacing="0" cellpadding="4">
      <tr>
        <td><b>Database Host:</b></td>
        <td align="right"><input type="text" value="" name="servername" /></td>
      </tr>
      <tr>
        <td><b>Database Name:</b></td>
        <td align="right"><input type="text" value="" name="dbname" /></td>
      </tr>
      <tr>
        <td><b>UserName:</b></td>
        <td align="right"><input type="text" value="" name="dbusername" /></td>
      </tr>
      <tr>
        <td><b>PassWord:</b></td>
        <td align="right"><input type="password" value="" name="dbpassword" /></td>
      </tr>
      <tr>
        <td><b>TablePrefix:</b></td>
        <td align="right"><input type="text" value="txp_" name="db_prefix" /></td>
      </tr>
    </table>

    <p align="right">
      <input type="hidden" name="step" value="3" />
      <input type="submit" value="Let's Go" />
    </p>
    <?php
	}
} elseif ($step == '3') {
	if(trim($_POST['dbname']) == "" || trim($_POST['servername']) == "" || trim($_POST['dbusername']) == ""){

?>
    <p>This either means that the informations what you input is incorrect or we can't contact the database server. This could mean your host's database server is down..</p>
    <p>
      <input type="button" value="Back" onclick="history.back(1)" />
    </p>
<?php
	} else {

		if(is_writeable($configfile)) {

			$servername = trim($_POST['servername']);
			$dbusername = trim($_POST['dbusername']);
			$dbpassword = trim($_POST['dbpassword']);
			$dbname = trim($_POST['dbname']);
			$db_prefix = trim($_POST['db_prefix']);
			$db_prefix = $db_prefix ? $db_prefix : 'txp_';

			$fp = @fopen($configfile, 'r');
			$filecontent = @fread($fp, @filesize($configfile));
			@fclose($fp);

			$filecontent = preg_replace("/[$]servername\s*\=\s*[\"'].*?[\"']/is", "\$servername = '$servername'", $filecontent);
			$filecontent = preg_replace("/[$]dbusername\s*\=\s*[\"'].*?[\"']/is", "\$dbusername = '$dbusername'", $filecontent);
			$filecontent = preg_replace("/[$]dbpassword\s*\=\s*[\"'].*?[\"']/is", "\$dbpassword = '$dbpassword'", $filecontent);
			$filecontent = preg_replace("/[$]dbname\s*\=\s*[\"'].*?[\"']/is", "\$dbname = '$dbname'", $filecontent);
			$filecontent = preg_replace("/[$]db_prefix\s*\=\s*[\"'].*?[\"']/is", "\$db_prefix = '$db_prefix'", $filecontent);

			$fp = @fopen($configfile, 'w');
			@fwrite($fp, trim($filecontent));
			@fclose($fp);

		}

		include ($configfile);
		include ('../include/db.php');
		$DB = new DB_MySQL;

		$DB->connect($servername, $dbusername, $dbpassword, $dbname, $usepconnect);
		unset($servername, $dbusername, $dbpassword, $usepconnect);

		$msg = '';
		$quit = FALSE;
		$curr_os = PHP_OS;
		$curr_php_version = PHP_VERSION;
		if($curr_php_version < '4.0.6') {
			$msg .= "<font color=\"#FF0000\">您的PHP版本低于4.0.6, 无法使用 TextPress</font><br />";
			$quit = TRUE;
		}

		$query = $DB->query("SELECT VERSION()");
		$curr_mysql_version = $DB->result($query, 0);
		if($curr_mysql_version < '3.23') {
			$msg .= "<font color=\"#FF0000\">您的MySQL版本低于3.23, 由于程序没有经过此平台的测试, 建议您换 MySQL4 的数据库服务器.</font><br />";
			$quit = TRUE;
		}

		if(strstr($db_prefix, '.')) {
			$msg .= "<font color=\"#FF0000\">The TablePrefix is incorrect, Please go back.</font><br />";
			$quit = TRUE;
		}

		$DB->select_db($dbname);
		if($DB->geterrdesc()) {
			if(mysql_get_server_info() > '4.1') {
				$DB->query("CREATE DATABASE $dbname DEFAULT CHARACTER SET $dbcharset");
			} else {
				$DB->query("CREATE DATABASE $dbname");
			}
			if($DB->geterrdesc()) {
				$msg .= "<font color=\"#FF0000\">Specified database does not exist and the system can not automatically to create database, install TextPress.</font><br />";
				$quit = TRUE;
			} else {
				$DB->select_db($dbname);
				$msg .= "Create database successfully. ";
			}
		}

		$query - $DB->query("SELECT COUNT(*) FROM {$db_prefix}settings", 'SILENT');
		if(!$DB->geterrdesc()) {
			$msg .= "<font color=\"#FF0000\">TextPress has been created table, continue the installation will clear the original data.</font><br />";
			$alert = " onclick=\"return confirm('Are you sure clear the original data ?');\"";
		} else {
			$alert = '';
		}

		if($quit) {
			$msg .= "<font color=\"#FF0000\">Because your server's directory configuration is not property or other reason,  TextPress can not continue with the installation.</font>";
		} else {
			$msg .= "Congratulations ! TextPress seems to work , Please go !";
		}
		if ($msg) {
			echo "<p>".$msg."</p>";
		}
		if($quit) {
?>
    <p>
      <input type="button" value="Exit" onclick="javascript: window.close();">
    </p>
<?php
		} else {
?>
    <table width="100%" border="0" cellspacing="0" cellpadding="4">
      <tr>
        <td><b>LoginName:</b></td>
        <td align="right"><input type="text" value="" name="username" /></td>
      </tr>
      <tr>
        <td><b>PassWord:</b></td>
        <td align="right"><input type="password" value="" name="password" /></td>
      </tr>
      <tr>
        <td><b>Confirm Password:</b></td>
        <td align="right"><input type="password" value="" name="comfirpassword" /></td>
      </tr>
    </table>
    <p align="right">
      <input type="hidden" name="step" value="4" />
      <input type="submit" value="Let's Go" <?php echo $alert;?> />
    </p>
<?php
		}
	}
} elseif ($step == '4') {
	$username = addslashes(trim($_POST['username']));
	$password = $_POST['password'];
	$comfirpassword = $_POST['comfirpassword'];

    if ($username == "" || $password == "" || $comfirpassword == "") {
		$msg = "<p>Please go back and enter all the required options.</p>";
		$quit = TRUE;
    } elseif (strlen($_POST['password']) < 8) {
		$msg = "<p>The password can not be less than 8 bit.</p>";
		$quit = TRUE;
	} elseif ($password != $comfirpassword) {
		$msg = "<p>PassWord is wrong !</p>";
		$quit = TRUE;
    } else {
		$msg = "<p>Congratulations ! The infomations seems to work , Please go !</p>";
		$quit = FALSE;
	}
	$name_key = array("\\",'&',' ',"'",'"','/','*',',','<','>',"\r","\t","\n",'#','$','(',')','%','@','+','?',';','^');
	foreach($name_key as $value){
		if (strpos($username,$value) !== false){ 
			$msg = "<p>User name can only contain letters, numbers, underscores.</p>";
			$quit = TRUE;
		}
	}
	if ($quit) {
		echo $msg;
?>
    <p>
      <input type="button" value="Back" onclick="history.back(1)" />
    </p>
<?php
	} else {
		echo $msg;
?>
    <p><b>LoginName:</b> <?php echo $username;?><input type="hidden" name="username" value="<?php echo $username;?>" /></p>
    <p><b>PassWord:</b> <?php echo $password;?><input type="hidden" name="password" value="<?php echo $password;?>" /></p>

    <p>Have any question ? If no,Let's go !</p>

    <p align="right">
      <input type="hidden" name="step" value="5" />
      <input type="submit" value="Let's Go" />
    </p>
<?php
	}
} elseif ($step == '5') {
	$username = addslashes(trim($_POST['username']));
	$password = $_POST['password'];
?>
	<table width="100%" border="0" cellspacing="0" cellpadding="0">
<?php
	include ($configfile);
	include ('../include/db.php');

	$DB = new DB_MySQL;
	$DB->connect($servername, $dbusername, $dbpassword, $dbname, $usepconnect);
	unset($servername, $dbusername, $dbpassword, $usepconnect);

	runquery($sql);

	$DB->query("INSERT INTO {$db_prefix}users (username, password, groupid) VALUES ('$username', '".md5($password)."', '1')");
?>
    </table>
    <p>Tatol create <b><?php echo $tablenum;?></b> table.</p>
    <p>TextPress has been installed. Please remove all the install directory.</p>
    <p><b>LoginName:</b> <?php echo $username;?></p>
    <p><b>PassWord:</b> <?php echo $password;?></p>
    <p align="right"><a href="../">Go ! View your size</a></p>
<?php
}
?>
  </form>
</div>
</body>
</html>
<?php

function result($result = 1, $output = 1) {
	if($result) {
		$text = '... <font color="#0000EE">Yes</font><br />';
		if(!$output) {
			return $text;
		}
		echo $text;
	} else {
		$text = '... <font color="#FF0000">No</font><br />';
		if(!$output) {
			return $text;
		}
		echo $text;
	}
}

function runquery($sql) {
	global $dbcharset, $db_prefix, $DB, $tablenum;

	$sql = str_replace("\r", "\n", str_replace(' txp_', ' '.$db_prefix, $sql));
	$ret = array();
	$num = 0;
	foreach(explode(";\n", trim($sql)) as $query) {
		$queries = explode("\n", trim($query));
		foreach($queries as $query) {
			$ret[$num] .= $query[0] == '#' ? '' : $query;
		}
		$num++;
	}
	unset($sql);

	foreach($ret as $query) {
		$query = trim($query);
		if($query) {
			if(substr($query, 0, 12) == 'CREATE TABLE') {
				$name = preg_replace("/CREATE TABLE ([a-z0-9_]+) .*/is", "\\1", $query);
				echo '<tr><td>Create table '.$name.' ...</td><td align="right"><font color="#0000FF">successfully</font></td></tr>';
				$DB->query(createtable($query, $dbcharset));
				$tablenum++;
			} else {
				$DB->query($query);
			}
		}
	}
}

function createtable($sql, $dbcharset) {
	$type = strtoupper(preg_replace("/^\s*CREATE TABLE\s+.+\s+\(.+?\).*(ENGINE|TYPE)\s*=\s*([a-z]+?).*$/isU", "\\2", $sql));
	$type = in_array($type, array('MYISAM', 'HEAP')) ? $type : 'MYISAM';
	return preg_replace("/^\s*(CREATE TABLE\s+.+\s+\(.+?\)).*$/isU", "\\1", $sql).
		(mysql_get_server_info() > '4.1' ? " ENGINE=$type DEFAULT CHARSET=$dbcharset" : " TYPE=$type");
}

?>
