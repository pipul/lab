<?php

// 文件说明：评论管理


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

// 权限检查
permission();

if (!$action) {
    $action = 'cmlist';
}

$articleid = intval($_GET['articleid'] ? $_GET['articleid'] : $_POST['articleid']);
$commentid = intval($_GET['commentid'] ? $_GET['commentid'] : $_POST['commentid']);
$do = in_array($do,array('hidden','display','del')) ? $do : '';

// 设置状态
if($action == 'cmvisible') {
	if ($commentid) {
		$comment = $DB->fetch_one_array("SELECT visible,articleid FROM {$db_prefix}comments WHERE commentid='$commentid'");
		if ($comment['visible']) {
			$visible = '0';
			$query = '-';
			$state = '隐藏';
		} else {
			$visible = '1';
			$query = '+';
			$state = '显示';
		}
		$DB->unbuffered_query("UPDATE {$db_prefix}articles SET comments=comments".$query."1 WHERE articleid='".$comment['articleid']."'");
		$DB->unbuffered_query("UPDATE {$db_prefix}comments SET visible='$visible' WHERE commentid='$commentid'");

		redirect('已经成功把该评论设置为 '.$state.' 状态', 'admincp.php?job=comment&action=cmlist&articleid='.$articleid);
	} else {
		redirect('缺少参数', 'admincp.php?job=comment&action=cmlist&articleid='.$articleid);
	}
}

// 修改评论

if($action == 'domodcm') {
	$author = trim($_POST['author']);
	$url = trim($_POST['url']);
	if(!$author || strlen($author) > 30) {
		redirect('用户名为空或用户名太长');
	}
	$name_key = array("\\",'&',' ',"'",'"','/','*',',','<','>',"\r","\t","\n",'#','$','(',')','%','@','+','?',';','^');
	foreach($name_key as $value){
		if (strpos($author,$value) !== false){ 
			redirect('此用户名包含不可接受字符或被管理员屏蔽,请选择其它用户名');
		}
	}
	$author = char_cv($author);
	if ($url) {
		if (!isemail($url)) {
			if (!preg_match("#^(http|news|https|ftp|ed2k|rtsp|mms)://#", $url)) {
				redirect('网站URL错误');
			}
			$key = array("\\",' ',"'",'"','*',',','<','>',"\r","\t","\n",'(',')','+',';');
			foreach($key as $value){
				if (strpos($url,$value) !== false){
					redirect('网站URL错误');
				}
			}
		}
		$url = char_cv($url);
	}

    $DB->unbuffered_query("UPDATE {$db_prefix}comments SET author='$author', url='$url', content='".addslashes($_POST['content'])."' WHERE commentid='$commentid'");

    redirect('修改评论成功', 'admincp.php?job=comment&action=cmlist&articleid='.$articleid);

}

// 批量处理评论状态

if($action == 'domorecmlist') {

	if ($do == 'display') {

		$visible = '1';

		$msg     = '所选评论已显示';
		$del     = false;

	} elseif ($do == 'hidden') {
		$visible = '0';
		$msg     = '所选评论已隐藏';
		$del     = false;
	} elseif ($do == 'del') {
		$msg     = '所选评论已删除';
		$del     = true;
	} else {

		redirect('未选择任何操作');

	}
	if ($cids = implode_ids($_POST['comment'])) {
		if ($del) {
			$DB->unbuffered_query("DELETE FROM {$db_prefix}comments WHERE commentid IN ($cids)");
		} else {
			$DB->unbuffered_query("UPDATE {$db_prefix}comments SET visible='$visible' WHERE commentid IN ($cids)");
		}

		$query = $DB->query("SELECT articleid FROM {$db_prefix}articles");
		while ($article = $DB->fetch_array($query)) {
			// 更新所有文章的评论数
			$tatol = $DB->num_rows($DB->query("SELECT commentid FROM {$db_prefix}comments WHERE articleid='".$article['articleid']."' AND visible='1'"));
			$DB->unbuffered_query("UPDATE {$db_prefix}articles SET comments='$tatol' WHERE articleid='".$article['articleid']."'");
		}

		redirect($msg, 'admincp.php?job=comment&action=cmlist&articleid='.$articleid);
	} else {		
		redirect('未选择任何评论');
	}

}

if ($action == 'cmlist') {
	$sql_query = ' WHERE 1=1 ';
	$kind = in_array($_GET['kind'],array('display','hidden')) ? $_GET['kind'] : '';
	if ($kind == 'display') {
		$sql_query .= " AND visible='1'";
		$pagelink   = '&kind=display';
	}
	if ($kind == 'hidden') {
		$sql_query .= " AND visible='0'";
		$pagelink   = '&kind=hidden';
	}
	if ($articleid) {
		$article = $DB->fetch_one_array("SELECT title FROM {$db_prefix}articles WHERE articleid='$articleid'");
		$sql_query .= " AND articleid='$articleid'";
		$pagelink   = '&articleid='.$articleid;
	}
	$ip = char_cv($_GET['ip']);
	if ($ip) {
		$frontlen = strrpos($ip, '.');
		$ipc = substr($ip, 0, $frontlen);
		$sql_query .= " AND (ipaddress LIKE '%".$ipc."%')";
		$pagelink   = '&ip='.$ip;
	}
	if($page) {
		$start_limit = ($page - 1) * 30;
	} else {
		$start_limit = 0;
		$page = 1;
	}
	$tatol     = $DB->num_rows($DB->query("SELECT commentid FROM {$db_prefix}comments $sql_query"));
	$multipage = multi($tatol, 30, $page, 'admincp.php?job=comment&action=cmlist'.$pagelink);



	$query  = $DB->query("SELECT * FROM {$db_prefix}comments $sql_query ORDER BY commentid DESC LIMIT $start_limit, 30");
	$commentdb = array();

    while ($comment = $DB->fetch_array($query)) {
		$comment['visible'] = $comment['visible'] ? '<span class="yes">显示</span>' : '<span class="no">隐藏</span>';
		$comment['url'] = $comment['url'] ? (isemail($comment['url']) ? '<a href="mailto:'.$comment['url'].'" target="_blank">发送邮件</a>' : '<a href="'.$comment['url'].'" target="_blank">访问主页</a>') : '<font color="#FF0000">Null</font>';
		$comment['dateline'] = sadate('Y-m-d H:i',$comment['dateline']);
		$comment['content'] = trimmed_title(htmlspecialchars($comment['content']));
		$commentdb[] = $comment;
	}
	unset($comment);
	$DB->free_result($query);
}

// 修改评论

if ($action == 'modcm') {
	$comment = $DB->fetch_one_array("SELECT c.articleid,c.commentid,c.author,c.url,c.dateline,c.content, a.title FROM {$db_prefix}comments c LEFT JOIN {$db_prefix}articles a ON (a.articleid=c.articleid) WHERE commentid='$commentid'");
	$comment['content'] = htmlspecialchars($comment['content']);
}

cpheader();
include PrintEot('comment');

?>
