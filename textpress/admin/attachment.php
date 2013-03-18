<?php

// 文件说明：附件管理


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

// 权限检查
permission();

// 加载附件相关函数

require_once(TXP_ROOT.'include/attachment.php');


// 添加附件到指定文章
if($action == 'addattachtoarticle') {
	$articleid = intval($articleid);
	$article = $DB->fetch_one_array("SELECT title,attachments,visible FROM {$db_prefix}articles WHERE articleid='$articleid'");
	if(!$article) {
		message('文章不存在', './');
	}
	// 修改附件
	$oldattach = array();	
	if ($article['attachments']){
		$oldattach = unserialize(stripslashes_array($article['attachments']));
	}
	$searcharray = array();
	$replacearray = array();
	require_once(TXP_ROOT.'admin/uploadfiles.php');
	if ($attachs){
		$attachs=unserialize(stripslashes_array($attachs));
		foreach ($attachs as $key => $value){
			$oldattach[$key] = $value;
		}
	}
	if ($oldattach){
		$oldattach = addslashes(serialize($oldattach));
	} else {
		$oldattach = '';
	}
	if($attachmentids){
		$DB->unbuffered_query("UPDATE {$db_prefix}attachments SET articleid='$articleid' WHERE attachmentid IN($attachmentids)");
	}
	// 修改附件结束
	$DB->unbuffered_query("UPDATE {$db_prefix}articles SET attachments='$oldattach' WHERE articleid='$articleid'");

    redirect('成功上传了'.$attachment_count.'个附件到《'.$article['title'].'》', 'admincp.php?job=attachment&action=list&articleid='.$articleid,'10');
}


// 批量删除附件

if($action == 'delattachments') {
	if ($attachmentids = implode_ids($_POST['attachment'])) {
		$nokeep = array();
		$query  = $DB->query("SELECT attachmentid,filepath,thumb_filepath FROM {$db_prefix}attachments WHERE attachmentid IN ($attachmentids)");
		while($attach = $DB->fetch_array($query)) {
			$nokeep[$attach['attachmentid']] = $attach;
		}
		removeattachment($nokeep);

		$articleid = intval($_POST['articleid']);
		redirect('成功删除所选附件,如果删除数量很多.建议执行一次附件修复操作,更新文章中的附件信息以提高访问速度.', 'admincp.php?job=attachment&action=list&articleid='.$articleid,'10');
	} else {
        redirect('未选择任何附件');
	}

}

// 附件修复
if($action == 'dorepair') {
	$query = $DB->query("SELECT attachmentid,filepath FROM {$db_prefix}attachments");
	while ($attach = $DB->fetch_array($query)) {
		if(!file_exists(TXP_ROOT.$options['attachments_dir'].'/'.$attach['filepath'])){
			$DB->unbuffered_query("DELETE FROM {$db_prefix}attachments WHERE attachmentid='".$attach['attachmentid']."'");
		}
	}
	unset($attach,$query);
	$count = 0;
	$query = $DB->query("SELECT articleid,attachments FROM {$db_prefix}articles WHERE attachments <> ''");
	while ($aids = $DB->fetch_array($query)) {
		$attachs = unserialize(stripslashes_array($aids['attachments']));
		if (is_array($attachs)) {
			$update=0;
			foreach ($attachs as $key=>$attach) {
				if($attach['filepath'] && file_exists(TXP_ROOT.$options['attachments_dir'].'/'.$attach['filepath'])){
					$check = $DB->fetch_one_array("SELECT attachmentid FROM {$db_prefix}attachments WHERE attachmentid='".$attach['attachmentid']."'");	
					if (!$check) {
						$count++;
						$attach['filename'] = addslashes($attach['filename']);
						$DB->query("INSERT INTO {$db_prefix}attachments (filename, filesize, filetype, filepath, dateline, downloads, isimage, thumb_filepath, thumb_width, thumb_height) VALUES ('".addslashes($attach['filename'])."', '".addslashes($attach['filesize'])."', '".addslashes($attach['filetype'])."', '".addslashes($attach['filepath'])."', '".addslashes($attach['dateline'])."', '".addslashes($attach['downloads'])."', '".addslashes($attach['isimage'])."', '".addslashes($attach['thumb_filepath'])."', '".addslashes($attach['thumb_width'])."', '".addslashes($attach['thumb_height'])."')");
					}
				} else{
					$count++;
					$check = $DB->fetch_one_array("SELECT attachmentid FROM {$db_prefix}attachments WHERE attachmentid='".$attach['attachmentid']."'");
					if($check){
						$DB->unbuffered_query("DELETE FROM {$db_prefix}attachments WHERE attachmentid='".$attach['attachmentid']."'");
					}
					$update=1;
					unset($attachs[$key]);
				}
			}
			if($update){
				$attachs = $attachs ? addslashes(serialize($attachs)) : '';
				$DB->unbuffered_query("UPDATE {$db_prefix}articles SET attachments='$attachs' WHERE articleid='".$aids['articleid']."'");
			}
		} else{
			$count++;
			$DB->unbuffered_query("UPDATE {$db_prefix}articles SET attachments='' WHERE articleid='".$aids['articleid']."'");
		}
	}

    redirect('成功修复'.$count.'个附件记录', 'admincp.php?job=attachment&action=list');
}

if ($action == 'doclear'){
	if (!$start){
		$start=0;
		$deltotal=0;
	}
	$num	= 0;
	$delnum	= 0;
	!$percount && $percount = 500;
	$attachdir = TXP_ROOT.$options['attachments_dir'];
	$dir1 = @opendir($attachdir);
	while($file1 = @readdir($dir1)){
		if ($file1 != '' && $file1 != '.' && $file1 != 'index.php' && $file1 != '..' && $file1 != 'index.htm'){
			if (@is_dir($attachdir.'/'.$file1)){
				$dir2 = @opendir($attachdir.'/'.$file1);
				while($file2 = @readdir($dir2)){
					if (@is_file($attachdir.'/'.$file1.'/'.$file2) && $file2 != '' && $file1 != 'index.php' && $file2 != '.' && $file2 != '..' && $file2 != 'index.htm'){
						$num++;
						if ($num > $start){
							$r = $DB->fetch_one_array("SELECT attachmentid FROM {$db_prefix}attachments WHERE filepath='/$file1/$file2' OR  thumb_filepath='/$file1/$file2'");
							if(!$r){
								$delnum++;
								$deltotal++;
								@unlink($attachdir.'/'.$file1.'/'.$file2);
							}
							if ($num-$start >= $percount){
								$start = $num-$delnum;
								$jumpurl="admincp.php?job=attachment&action=doclear&start=$start&percount=$percount&deltotal=$deltotal";
								redirect('正在清理冗余附件，已经删除 '.$deltotal.' 个附件,程序将自动完成整个过程.页面跳转中..',$jumpurl);
							}
						}
					}
				}
			} elseif (is_file($attachdir.'/'.$file1)){
				$num++;
				if ($num > $start){
					$rt = $DB->fetch_one_array("SELECT attachmentid FROM {$db_prefix}attachments WHERE filepath='/$file1' OR  thumb_filepath='/$file1'");
					if(!$rt){
						$delnum++;
						$deltotal++;
						@unlink($attachdir.'/'.$file1);
					}
					if ($num-$start >= $percount){
						$start = $num-$delnum;

						$jumpurl = "admincp.php?job=attachment&action=doclear&start=$start&percount=$percount&deltotal=$deltotal";
						redirect('正在清理冗余附件，已经删除 '.$deltotal.' 个附件,程序将自动完成整个过程.页面跳转中..',$jumpurl);
					}
				}
			}
		}
	}
	redirect('附件清理结束,共删除了'.$deltotal.'个冗余附件.', 'admincp.php?job=attachment&action=list');
}


// 操作结束



if (!$action) {
	$action = 'list';
}

if ($action == 'list') {
	if($page) {
		$start_limit = ($page - 1) * 30;
	} else {
		$start_limit = 0;
		$page = 1;
	}
	$sql = 'WHERE 1';

	if ($view == 'image') {
		$sql .= " AND (a.filetype LIKE '%image/%')";
	} elseif ($view == 'file') {
		$sql .= " AND !(a.filetype LIKE '%image/%')";
	}
	$articleid = intval($_GET['articleid']);
	if ($articleid) {
		$article = $DB->fetch_one_array("SELECT title FROM {$db_prefix}articles WHERE articleid='$articleid'");
		$sql .= " AND a.articleid='$articleid'";
	} else {
		$warning = '';
		$a_dir = TXP_ROOT.$options['attachments_dir'];
		$attachsize = dirsize($a_dir);
		$dircount = dircount($a_dir);
		$realattachsize = (is_numeric($attachsize)) ? sizecount($attachsize) : '不详';
		$stats = $DB->fetch_one_array("SELECT count(*) as count, sum(filesize) as sum FROM {$db_prefix}attachments");
		$stats['count'] = ($stats['count'] != 0) ? $stats['count'] : 0;
		$stats['sum'] = ($stats['count'] == 0) ? '0 KB' : sizecount($stats['sum']);
		if (!@is_dir($a_dir)) {
			$warning = ' <font color="#FF0000"><strong>(文件夹无效,请重新设定上传文件夹!)</strong></font>';
		}
	}
	$view = in_array($_GET['view'], array('image', 'file')) ? $_GET['view'] : '';

	$tatol = $DB->num_rows($DB->query("SELECT attachmentid FROM {$db_prefix}attachments a ".$sql));
	$multipage = multi($tatol, 30, $page, 'admincp.php?job=attachment&action=list&view='.$view.'&articleid='.$articleid);

    $query = $DB->query("SELECT a.*,ar.title as article FROM {$db_prefix}attachments a LEFT JOIN {$db_prefix}articles ar ON (ar.articleid=a.articleid) $sql ORDER BY a.attachmentid DESC LIMIT $start_limit, 30");

	$attachdb = array();
	while ($attach = $DB->fetch_array($query)) {
		$attach['thumb'] = $attach['thumb_filepath'] ? '<a href="../'.$options['attachments_dir'].$attach['thumb_filepath'].'" target="_blank"><font color=#FF0000>有</font></a>' : '无';
		$attach['filename'] = htmlspecialchars($attach['filename']);
		$attach['filepath'] = htmlspecialchars($attach['filepath']);
		$attach['filesize'] = sizecount($attach['filesize']);
		$attach['filetype'] = htmlspecialchars($attach['filetype']);
		$attach['name'] = htmlspecialchars($attach['name']);
		$attach['dateline'] = date("Y-m-d H:i",$attach['dateline']);
		$pathdata = split('/',$attach['filepath']);
		if (count($pathdata) == 2) {
			$attach['subdir'] = '根目录';
		} else {
			$attach['subdir'] = $pathdata[1];
		}
		$attachdb[] = $attach;
	}
	unset($attach);
	$DB->free_result($query);
}

cpheader();
include PrintEot('attachment');

?> 
