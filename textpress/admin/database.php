<?php

// 文件说明：数据库管理


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

// 权限检查
permission();

$backupdir = 'backupdata';

$tables = array(
	$db_prefix.'articles',
	$db_prefix.'attachments',
	$db_prefix.'categories',
	$db_prefix.'comments',
	$db_prefix.'links',
	$db_prefix.'settings',
	$db_prefix.'sessions',
	$db_prefix.'users'
);

// 恢复数据库文件
if ($action == 'resume') {
	$sqlfile = $_GET['sqlfile'] ? $_GET['sqlfile'] : $_POST['sqlfile'];
	$file = $backupdir.'/'.$sqlfile;
	$path_parts = pathinfo($file);
	if (strtolower($path_parts['extension']) != 'sql') {
		redirect('只能恢复SQL文件!','admincp.php?job=database&action=mysqltools');
	}
	if(@$fp = fopen($file, 'rb')) {
		$sqldump = fgets($fp, 256);
		$identify = explode(',', base64_decode(preg_replace("/^# Identify:\s*(\w+).*/s", "\\1", $sqldump)));
		$sqldump .= fread($fp, filesize($file));
		fclose($fp);
	} else {
		if($autoimport) {
			restats();
			redirect('分卷数据成功导入数据库','admincp.php?job=database&action=mysqltools');
		} else {
			redirect('分卷数据导入数据库失败','admincp.php?job=database&action=mysqltools');
		}
	}

	if($identify[0] && $identify[1] == $TXP_VERSION && $identify[2]) {
		$sqlquery = splitsql($sqldump);
		unset($sqldump);
		foreach($sqlquery as $sql) {
			if(trim($sql) != '') {
				$DB->query($sql, 'SILENT');
			}
		}

		$file_next = basename(preg_replace("/_($identify[2])(\..+)$/", "_".($identify[2] + 1)."\\2", $file));

		if($identify[2] == 1) {
			redirect('分卷数据成功导入数据库,程序将自动导入本次其他的备份.','admincp.php?job=database&action=resume&sqlfile='.rawurlencode($file_next).'&autoimport=yes');
		} elseif($autoimport) {
			redirect('数据文件卷号 '.$identify[2].' 成功导入，程序将自动继续。', 'admincp.php?job=database&action=resume&sqlfile='.rawurlencode($file_next).'&autoimport=yes');
		} else {
			restats();
			redirect('数据成功导入','admincp.php?job=database&action=mysqltools');
		}
	} else {
		redirect('数据文件非 TextPress 格式或与程序当前版本信息不符,无法导入.','admincp.php?job=database&action=mysqltools',5);
	}
}

// 备份操作
if ($action == 'dobackup') {
	$volume = intval($volume) + 1;
	$sqlfilename = 'backupdata/'.$filename.'_'.$volume.'.sql';

	if(!$sqlfilename || preg_match("/(\.)(exe|jsp|asp|asa|htr|stm|shtml|php3|aspx|cgi|fcgi|pl|php|bat)(\.|$)/i", $sqlfilename)) {
		redirect('您没有输入备份文件名或文件名中使用了敏感的扩展名.', 'admincp.php?job=database&action=mysqltools',5);
	}

	$idstring = '# Identify: '.base64_encode("$timestamp,$TXP_VERSION,$volume")."\n";

	$sqlcompat = in_array($sqlcompat, array('MYSQL40', 'MYSQL41')) ? $sqlcompat : '';
	$dumpcharset = str_replace('-', '', $charset);
	$setnames = intval($addsetnames) || ($DB->version() > '4.1' && (!$sqlcompat || $sqlcompat == 'MYSQL41')) ? "SET character_set_connection=".$dumpcharset.", character_set_results=".$dumpcharset.", character_set_client=binary;\n\n" : '';

	if($DB->version() > '4.1') {
		$DB->query("SET character_set_connection=$dumpcharset, character_set_results=$dumpcharset, character_set_client=binary;");
		if($sqlcompat == 'MYSQL40') {
			$DB->query("SET SQL_MODE='MYSQL40'");
		}
	}
		
	$sqldump = '';
	$tableid = $tableid ? $tableid - 1 : 0;
	$startfrom = intval($startfrom);
	for($i = $tableid; $i < count($tables) && strlen($sqldump) < $sizelimit * 1000; $i++) {
		$sqldump .= sqldumptable($tables[$i], $startfrom, strlen($sqldump));
		$startfrom = 0;
	}
	$tableid = $i;
	if(trim($sqldump)) {
		$sqldump = "$idstring".
			"# <?exit();?>\n".
			"# TextPress bakfile Multi-Volume Data Dump Vol.$volume\n".
			"# Version: $TXP_VERSION\n".
			"# Time: ".sadate('Y-m-d H:i',$timestamp)."\n".
			"# TextPress: http://pipul.org\n".
			"# --------------------------------------------------------\n\n\n".$setnames.$sqldump;

		@$fp = fopen($sqlfilename, 'wb');
		@flock($fp, 2);
		if(@!fwrite($fp, $sqldump)) {
			@fclose($fp);
			redirect('数据文件无法备份到服务器, 请检查目录属性.', 'admincp.php?job=database&action=mysqltools',5);
		} else {
			redirect('分卷备份:数据文件 '.$volume.' 成功创建,程序将自动继续.', "admincp.php?job=database&action=dobackup&filename=".rawurlencode($filename)."&sizelimit=".rawurlencode($sizelimit)."&volume=".rawurlencode($volume)."&tableid=".rawurlencode($tableid)."&startfrom=".rawurlencode($startrow)."&extendins=".rawurlencode($extendins)."&sqlcompat=".rawurlencode($sqlcompat));
		}
	} else {
		redirect('数据成功备份至服务器指定文件中', 'admincp.php?job=database&action=mysqltools');
	}

}

function sqldumptable($table, $startfrom = 0, $currsize = 0) {
	global $DB, $sizelimit, $startrow, $extendins, $sqlcompat, $dumpcharset;

	$offset = 300;
	$tabledump = '';

	if(!$startfrom) {
		$tabledump = "DROP TABLE IF EXISTS $table;\n";
		$createtable = $DB->query("SHOW CREATE TABLE $table");
		$create = $DB->fetch_row($createtable);
		$tabledump .= $create[1];

		if($sqlcompat == 'MYSQL41' && $DB->version() < '4.1') {
			$tabledump = preg_replace("/TYPE\=(.+)/", "ENGINE=\\1 DEFAULT CHARSET=".$dumpcharset, $tabledump);
		}
		if($DB->version() > '4.1' && $dumpcharset) {
			$tabledump = preg_replace("/(DEFAULT)*\s*CHARSET=.+/", "DEFAULT CHARSET=".$dumpcharset, $tabledump);
		}

		$query = $DB->query("SHOW TABLE STATUS LIKE '$table'");
		$tablestatus = $DB->fetch_array($query);
		$tabledump .= ($tablestatus['Auto_increment'] ? " AUTO_INCREMENT=$tablestatus[Auto_increment]" : '').";\n\n";
		if($sqlcompat == 'MYSQL40' && $DB->version() >= '4.1') {
			if($tablestatus['Auto_increment'] <> '') {
				$temppos = strpos($tabledump, ',');
				$tabledump = substr($tabledump, 0, $temppos).' auto_increment'.substr($tabledump, $temppos);
			}
		}
	}

	$tabledumped = 0;
	$numrows = $offset;
	if($extendins == '0') {
		while($currsize + strlen($tabledump) < $sizelimit * 1000 && $numrows == $offset) {
			$tabledumped = 1;
			$rows = $DB->query("SELECT * FROM $table LIMIT $startfrom, $offset");
			$numfields = $DB->num_fields($rows);
			$numrows = $DB->num_rows($rows);
			while($row = $DB->fetch_row($rows)) {
				$comma = '';
				$tabledump .= "INSERT INTO $table VALUES (";
				for($i = 0; $i < $numfields; $i++) {
					$tabledump .= $comma.'\''.mysql_escape_string($row[$i]).'\'';
					$comma = ',';
				}
				$tabledump .= ");\n";
			}
			$startfrom += $offset;
		}
	} else {
		while($currsize + strlen($tabledump) < $sizelimit * 1000 && $numrows == $offset) {
			$tabledumped = 1;
			$rows = $DB->query("SELECT * FROM $table LIMIT $startfrom, $offset");
			$numfields = $DB->num_fields($rows);
			if($numrows = $DB->num_rows($rows)) {
				$tabledump .= "INSERT INTO $table VALUES";
				$commas = '';
				while($row = $DB->fetch_row($rows)) {
					$comma = '';
					$tabledump .= $commas." (";
					for($i = 0; $i < $numfields; $i++) {
						$tabledump .= $comma.'\''.mysql_escape_string($row[$i]).'\'';
						$comma = ',';
					}
					$tabledump .= ')';
					$commas = ',';
				}
				$tabledump .= ";\n";
			}
			$startfrom += $offset;
		}
	}

	$startrow = $startfrom;
	$tabledump .= "\n";
	return $tabledump;
}

// 批量删除备份文件
if($action == 'deldbfile') {
    if (!$sqlfile || !is_array($sqlfile)) {
        redirect('未选择任何文件');
    }
	$selected = count($sqlfile);
	$succ = $fail = 0;
    foreach ($sqlfile AS $file=>$value) {
		if (file_exists($file)) {
			@chmod($file, 0777);
			if (@unlink($file)) {
				$succ++;
			} else {
				$fail++;
			}
		} else {
			redirect(basename($file).' 文件已不存在', 'admincp.php?job=database&action=mysqltools');
		}
    }
    redirect('删除数据文件操作完毕,删除'.$selected.'个,成功'.$succ.'个,失败'.$fail.'个.', 'admincp.php?job=database&action=mysqltools',5);
}

// 数据库维护操作
if($action == 'dotools') {
	$doname = array(
		'check' => '检查',
		'repair' => '修复',
		'analyze' => '分析',
		'optimize' => '优化'
	);
	$dodb = $tabledb = array();
	foreach ($do AS $value) {
		$dodb[] = array('do'=>$value,'name'=>$doname[$value]);
		foreach ($tables AS $table) {
			if ($DB->query($value.' TABLE '.$table)) {
				$result = '<span class="yes">成功</span>';
			} else {
				$result = '<span class="no">失败</span>';
			}
			$tabledb[] = array('do'=>$value,'table'=>$table,'result'=>$result);
		}
	}
}

// 获取文件大小
function sizecount($filesize) {
	if($filesize >= 1073741824) {
		$filesize = round($filesize / 1073741824 * 100) / 100 . ' G';
	} elseif($filesize >= 1048576) {
		$filesize = round($filesize / 1048576 * 100) / 100 . ' M';
	} elseif($filesize >= 1024) {
		$filesize = round($filesize / 1024 * 100) / 100 . ' K';
	} else {
		$filesize = $filesize . ' bytes';
	}
	return $filesize;
}

function splitsql($sql) {
	$sql = str_replace("\r", "\n", $sql);
	$ret = array();
	$num = 0;
	$queriesarray = explode(";\n", trim($sql));
	unset($sql);
	foreach($queriesarray as $query) {
		$queries = explode("\n", trim($query));
		foreach($queries as $query) {
			$ret[$num] .= $query[0] == "#" ? NULL : $query;
		}
		$num++;
	}
	return($ret);
}

function random($length) {
	PHP_VERSION < '4.2.0' && mt_srand((double)microtime() * 1000000);

	$hash = '';
	$chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz';
	$max = strlen($chars) - 1;
	for($i = 0; $i < $length; $i++) {
		$hash .= $chars[mt_rand(0, $max)];
	}
	return $hash;
}

if(!$action) {
	$action = 'mysqltools';
}

if ($action == 'mysqltools') {
	$backuppath = sadate('Y-m-d',$timestamp).'_'.random(8);
	$file_i = 0;

	$catedb = array();
	$query = $DB->query("SELECT cid,name FROM {$db_prefix}categories");
	while ($cate = $DB->fetch_array($query)) {
		$catedb[] = $cate;		
	}
	unset($cate);

	$query = $DB->query("SELECT userid,username FROM {$db_prefix}users WHERE groupid='1' OR groupid='2' ORDER BY userid");
	$userdb = array();
	while ($user = $DB->fetch_array($query)) {
		$userdb[] = $user;
	}
	unset($user);
	$DB->free_result($query);

	if(is_dir($backupdir)) {
		$dirs = dir($backupdir);
		$dbfiles = array();
		$today = @sadate('Y-m-d',$timestamp);
		while ($file = $dirs->read()) {
			$filepath = $backupdir.'/'.$file;
			$pathinfo = pathinfo($file);
			if(is_file($filepath) && $pathinfo['extension'] == 'sql') {
				$fp = fopen($filepath, 'rb');
				$identify = explode(',', base64_decode(preg_replace("/^# Identify:\s*(\w+).*/s", "\\1", fgets($fp, 200))));
				fclose($fp);
				$moday = @sadate('Y-m-d',@filemtime($filepath));
				$mtime = @sadate('Y-m-d H:i',@filemtime($filepath));
				$dbfile = array(
					'filesize' => sizecount(filesize($filepath)),
					'mtime' => ($moday == $today) ? '<font color="#FF0000">'.$mtime.'</font>' : $mtime,
					'bktime' => $identify[0] ? @sadate('Y-m-d H:i',$identify[0]) : '未知',
					'version' => $identify[1] ? $identify[1] : '未知',
					'volume' => $identify[2] ? $identify[2] : '未知',
					'filepath' => urlencode($file),
					'filename' => htmlspecialchars($file),
				);
				$file_i++;
				$dbfiles[] = $dbfile;
			}
		}
		unset($dbfile);
		$dirs->close();
		$noexists = 0;
	} else {
		$noexists = 1;
	}
}

if ($action == 'checkresume') {
	$sqlfile = htmlspecialchars($sqlfile);
	$fp = fopen($backupdir.'/'.$sqlfile, 'rb');
	$identify = explode(',', base64_decode(preg_replace("/^# Identify:\s*(\w+).*/s", "\\1", fgets($fp, 200))));
	fclose($fp);
	if (count($identify) != 3) {
		redirect('SQL文件有可能是当前程序的老版本备份的,为了程序程序正常运作,不允许导入.确实要导入,请通过其他MYSQL管理程序导入.', 'admincp.php?job=database&action=mysqltools',5);
	}
	if ($identify[2] != 1) {
		redirect('为了程序正常运作,只允许导入卷号为1的SQL文件.', 'admincp.php?job=database&action=mysqltools',5);
	}
	if ($identify[1] != $TXP_VERSION) {
		redirect('SQL文件版本信息和当前程序版本不匹配,为了程序程序正常运作,不允许导入.确实要导入,请通过其他MYSQL管理程序导入.', 'admincp.php?job=database&action=mysqltools',5);
	}
}

cpheader();
include PrintEot('database');

?>
