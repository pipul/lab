<?php

// 文件说明：分类管理


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

// 权限检查
permission();

// 检查分类名是否符合逻辑
function checkname($name) {
	if(!$name || strlen($name) > 30) {
		$result = '分类名不能为空并且不能超过30个字符<br />';
		return $result;
	}
}

// 添加分类

if($action == 'doaddcate') {
	$name   = trim($_POST['name']);
	$result = checkname($name);
	if($result) {
		redirect($result);
	}
	$name = char_cv($name);
    $rs = $DB->fetch_one_array("SELECT count(*) AS categories FROM {$db_prefix}categories WHERE name='$name'");
    if($rs['categories']) {
		redirect('该分类名在数据库中已存在');
    }

    $DB->query("INSERT INTO {$db_prefix}categories (name) VALUES ('$name')");
    redirect('添加新分类成功', 'admincp.php?job=category');
}


// 修改分类

if($action == 'domodcate') {
	$name   = trim($_POST['name']);
	$cid    = intval($_POST['cid']);

	$result = checkname($name);

	if($result)	{
		redirect($result);
	}
	$name = char_cv($name);
	$rs = $DB->fetch_one_array("SELECT count(*) AS categories FROM {$db_prefix}categories WHERE cid!='$cid' AND name='$name'");
    if($rs['categories']) {
		redirect('已经有其他分类使用【'.$name.'】这个名称');
    }

	// 更新分类
    $DB->unbuffered_query("UPDATE {$db_prefix}categories SET name='$name' WHERE cid='$cid'");
    redirect('修改分类成功', 'admincp.php?job=category');

}


// 删除分类

if($action == 'dodelcate') {
	$cid = intval($_POST['cid']);
	$aids = 0;

	// 删除分类

	$DB->query("DELETE FROM {$db_prefix}categories WHERE cid='$cid'");

	// 加载附件相关函数
	require_once(TXP_ROOT.'include/attachment.php');

	$query = $DB->query("SELECT articleid, uid, visible FROM {$db_prefix}articles WHERE cid='$cid' ORDER BY articleid");

	while ($article = $DB->fetch_array($query)) {
		$aids .= ','.$article['articleid'];
		if ($article['visible']) {
			$DB->query("UPDATE {$db_prefix}users SET articles=articles-1 WHERE userid='".$article['uid']."'");
		}
	}

	// 删除该分类下文章中的附件
	$query  = $DB->query("SELECT attachmentid,filepath,thumb_filepath FROM {$db_prefix}attachments WHERE articleid IN ($aids)");
	$nokeep = array();
	while($attach = $DB->fetch_array($query)) {
		$nokeep[$attach['attachmentid']] = $attach;
	}
	removeattachment($nokeep);

	$DB->unbuffered_query("DELETE FROM {$db_prefix}comments WHERE articleid IN ($aids)");

	// 删除分类下的文章

    $DB->unbuffered_query("DELETE FROM {$db_prefix}articles WHERE cid='$cid'");

    redirect('成功删除分类和该分类下所有文章以及相关评论', 'admincp.php?job=category');

}

if(!$action) {
	$action = 'catelist';
}

// 分类列表

if ($action == 'catelist') {
	$catedb = array();
	$query = $DB->query("SELECT * FROM {$db_prefix}categories");
	$tatol = $DB->num_rows($query);
	while($cate = $DB->fetch_array($query)){
		$catedb[] = $cate;
	}
	unset($cate);
	$DB->free_result($query);
}

// 分类操作
if (in_array($action, array('addcate', 'modcate', 'delcate'))) {
	if ($action != 'addcate') {
		$cate = $DB->fetch_one_array("SELECT * FROM {$db_prefix}categories WHERE cid='".intval($_GET['cid'])."'");
	}
}

cpheader();
include PrintEot('category');

?>
