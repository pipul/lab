<?php

// 文件说明：模板管理


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

// 权限检查
permission();

if(!$action) {
	$action = 'template';
}

// 读取模板套系(目录)
$template_dir = '../content/templates/';
$path = $_GET['path'] ? $_GET['path'] : $_POST['path'];
$file = $_GET['file'] ? $_GET['file'] : $_POST['file'];
$ext = $_GET['ext'] ? $_GET['ext'] : $_POST['ext'];

$opened = @opendir($template_dir);
$dirdb = array();
while($dir = @readdir($opened)){
	if(($dir != '.') && ($dir != '..')) {
		if (@is_dir($template_dir.$dir)){
			$dirdb[] = $dir;
		}
	}
}
asort($dirdb);
unset($dir);
@closedir($opened);
$path = in_array($path,$dirdb) ? $path : 'default';
if (strstr($file,'.') || strstr($path,'.')) {
	redirect('模板无效', 'admincp.php?job=template&action=filelist');
}

// 获取模板信息
function get_template_info($infofile) {
	global $template_dir;
	$infofile = str_replace(array('..',':/'),array('',''),$infofile);
	$template_info = @file($template_dir.$infofile);
	if ($template_info) {
		$cssdata = array();
		foreach ($template_info AS $data) {
			$data = str_replace('://','=//',$data);
			$info = explode(':', $data);
			$info[1] = trim(str_replace('=//','://',$info[1]));
			$cssdata[] = $info[1];
		}
		// 判断制作者是否有网站
		if ($cssdata[4]) {
			$cssdata[3] = '<a href="'.trim($cssdata[4]).'" title="访问模板作者的网站" target="_blank">'.trim($cssdata[3]).'</a>';
		}
		// 判断缩略图是否存在
		$templatedir = dirname($template_dir.$infofile);
		if (file_exists($templatedir.'/screenshot.png')) {
			$screenshot = $templatedir.'/screenshot.png';
		} else {
			$screenshot = dirname($template_dir.dirname($infofile)).'/no.png';
		}
		$info = array(
			'name' => $cssdata[0],
			'dirurl' => urlencode(dirname($infofile)),
			'version' => $cssdata[1],
			'description' => $cssdata[2],
			'author' => $cssdata[3],
			'templatedir' => $templatedir,
			'screenshot' => $screenshot
		);
		return $info;
	} else {
		return false;
	}
}

// 复制目录
function copydir($source, $target) {
	if (substr($source, -1) != '/') {
		$source = $source.'/';
	}
	if (substr($target, -1) != '/') {
		$target = $target.'/';
	}
	if (!@mkdir($target, 0777)) {
		return false;
	} else {
		@chmod($target, 0777);
	}
	$result = true;
	$handle = @opendir($source);
	while(($file = @readdir($handle)) !== false) {
		if($file != '.' && $file != '..') {
			if(@is_dir($source.$file)) {
				copydir($source.$file, $target.$file);
			} else {
				if(!@copy($source.$file, $target.$file)) {
					$result = false;
					break;
				}
			}
		}
	}
	@closedir($handle);
	return $result;
}

// 删除目录
function removedir($dirname){
	$result = false;
	if (substr($dirname, -1) != '/') {
		$dirname = $dirname.'/';
	}
	$handle = @opendir($dirname);
	while(($file = @readdir($handle)) !== false) {
		$delfile = $dirname.$file;
		if ($file != '.' && $file != '..') {
			if(@is_dir($delfile)) { 
				@chmod($delfile,0777);
				removedir($delfile);
			} else {
				@chmod($delfile,0777);
				@unlink($delfile);
			}
		}
	}
	@closedir($handle);
	@chmod($dirname,0777);
	@rmdir($dirname);
}

// 设置模板
if($action == 'settemplate') {
	$name = $_GET['name'];
	if (file_exists($template_dir.$name) && strpos($name,'..')===false) {
		$DB->query("REPLACE INTO {$db_prefix}settings VALUES ('templatename', '".addslashes($name)."')");
		redirect('模板已经更新', 'admincp.php?job=template&action=template');
	} else {
		redirect('模板不存在', 'admincp.php?job=template&action=template');
	}
}

// 保存文件
if($action == 'savefile'){
	$ext = in_array($ext,array('php','css')) ? $ext : 'php';
	$filepath = $template_dir.$path.'/'.$file.'.'.$ext;
	if (file_exists($filepath)) {
		$content = stripslashes_array(trim($_POST['content']));
		$fp = @fopen($filepath,'wb');
		@fwrite($fp,$content);
		@fclose($fp);
		redirect('模板修改成功', 'admincp.php?job=template&action=filelist&path='.$path);
	} else {
		redirect('模板文件不存在', 'admincp.php?job=template&action=filelist&path='.$path);
	}
}

// 删除文件
if($action == 'delfile'){
	$ext = in_array($ext,array('php','css')) ? $ext : 'php';
	$filepath = $template_dir.$path.'/'.$file.'.'.$ext;
	if (file_exists($filepath)) {
		@chmod ($filepath, 0777);
		if (@unlink($filepath)) {
			redirect('模板删除成功', 'admincp.php?job=template&action=filelist&path='.$path);
		} else {
			@chmod ($filepath, 0777);
			redirect('模板删除失败', 'admincp.php?job=template&action=filelist&path='.$path);
		}
	} else {
		redirect('模板文件不存在', 'admincp.php?job=template&action=filelist&path='.$path);
	}
}


// 新建模板
if($action == 'donewtemplate') {
	$tplname = addslashes($_POST['newtemplatename']);
	if(!preg_match("/^[a-z0-9A-Z\-_.+]+$/i", $tplname) || strlen($tplname) > 50) {
		redirect('模板名称只能用英文和数字并且不能超过50个字节', 'admincp.php?job=template');
	}
	// 原始模板default
	$stpl = $template_dir.'default/';
	// 新建模板目录名
	$ttpl = $template_dir.$tplname.'/';
	if(!is_dir($stpl)) {
		redirect('读取原始模板失败.无法创建新模板.','admincp.php?job=template');
	}
	if(!is_dir($ttpl)) {
		if (copydir($stpl,$ttpl)) {
			redirect('成功新建'.$tplname.'模板','admincp.php?job=template&action=filelist&path='.$tplname);
		} else {
			removedir($ttpl);
			redirect('新建'.$tplname.'模板失败','admincp.php?job=template');
		}
	} else {
		redirect('模板已经存在,请换一个名字.','admincp.php?job=template');
	}
}

// 选择模板
if($action == 'template') {
	$current_infofile = $options['templatename'].'/info.txt';
	if (file_exists($template_dir.$current_infofile)) {
		$current_template_info = get_template_info($current_infofile);
	} else {
		$current_template_info = '';
	}	
	if (!file_exists($template_dir.$options['templatename'].'/screenshot.png')) {
		$current_template_info['screenshot'] = $template_dir.'no.png';
	} else {
		$current_template_info['screenshot'] = $template_dir.$options['templatename'].'/screenshot.png';
	}

	$dir1 = opendir($template_dir);
	$available_template_db = array();
	while($file1 = readdir($dir1)){
		if ($file1 != '' && $file1 != '.' && $file1 != '..' && $file1 != 'admin' && $file1 != $options['templatename']){
			if (is_dir($template_dir.'/'.$file1)){
				$dir2 = opendir($template_dir.'/'.$file1);
				while($file2 = readdir($dir2)){
					if (is_file($template_dir.'/'.$file1.'/'.$file2) && $file2 == 'info.txt'){
						$available_template_db[] = get_template_info($file1.'/'.$file2);
					}
				}
				closedir($dir2);
			}
		}
	}
	closedir($dir1);
	unset($file1);
}

// 修改模板文件
if($action == 'mod'){
	$ext = in_array($ext,array('php','css')) ? $ext : 'php';
	$filepath = $template_dir.$path.'/'.$file.'.'.$ext;
	if (file_exists($filepath)) {
		$writeable = false;
		if(is_writeable($filepath)) {
			$writeable = true;
		}
		$fp = @fopen($filepath,'r');
		$contents = @fread($fp, filesize($filepath));
		@fclose($fp);
		$contents = htmlspecialchars($contents);
	} else {
		redirect('模板文件不存在', 'admincp.php?job=template&action=filelist&path='.$path);
	}
}

// 模板套系中的文件列表
if($action == 'filelist') {
	require_once(TXP_ROOT.'include/attachment.php');
	$dir = $template_dir.$path;
	$fp = opendir($dir);
	$tp_count = 0;
	$filedb = array();
	while ($file = readdir($fp)) {
		if ($file != '.' && $file != '..') {		
			$extension = getextension($file);
			if ($extension == 'php' || $extension == 'css') {
				$tp_count++;
				$filedb[$tp_count]['filename'] = str_replace(array('.php','.css'), '', $file);
				$filedb[$tp_count]['extension'] = $extension;
			}
		}
	}
	closedir($fp);
	asort($filedb);
	unset($file);
}

cpheader();
include PrintEot('template');
?>
