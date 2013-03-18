<?php

// 文件说明：加载后台函数集合

require_once('global.php');


if ($txp_group == 1) {
	$adminitem = array(
		'Console' => array(
			'Dashboard' => 'admincp.php',
			'User' => 'admincp.php?job=user',
			'Themes' => 'admincp.php?job=template',
			'Links' => 'admincp.php?job=link',
		),
		'Post' => array(
			'Articles' => 'admincp.php?job=article',
			'Category' => 'admincp.php?job=category',
			'Attachment' => 'admincp.php?job=attachment',
			'Comments' => 'admincp.php?job=comment',
		),
		'Settings' => array(
			'Database' => 'admincp.php?job=database',
			'Configurate' => 'admincp.php?job=configurate',
		),
		'Navigate' => array(
			'View Site' => '../',
			'Logout' => 'admincp.php?action=logout',
		),
	);
} else {
	// 撰写组不显示菜单
	$job = in_array($job, array('article','main')) ? $job : 'article';
}

if (!$job) {
	$job = 'main';
} else {
	if (strlen($job) > 20) {
		$job = 'main';
	}
	$job = str_replace(array('.','/','\\',"'",':','%'),'',$job);
	$job = basename($job);
	$job = in_array($job, array('configurate','article','comment','attachment','category','user','link','template','database')) ? $job : 'main';
}

if (file_exists($job.'.php')) {
	include ($job.'.php');
} else {
	include ('main.php');
}

cpfooter();

?>
