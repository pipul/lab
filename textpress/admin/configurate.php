<?php

// 文件说明：系统设置


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

// 权限检查
permission();

// 更新配置以及配置文件
if($_POST['action'] == 'updatesetting') {
	// $DB->query("TRUNCATE TABLE {$db_prefix}settings");
	foreach($_POST['setting'] AS $key => $val) {
		$DB->query("REPLACE INTO {$db_prefix}settings VALUES ('".addslashes($key)."', '".addslashes($val)."')");
	}
	redirect('更新系统配置成功', 'admincp.php?job=configurate');
}

$query = $DB->query("SELECT * FROM {$db_prefix}settings");
while($setting = $DB->fetch_array($query)) {
	$settings[$setting['title']] = htmlspecialchars($setting['value']);
}

ifselected($settings['show_debug'],'show_debug');
ifselected($settings['audit_comment'],'audit_comment');
ifselected($settings['seccode'],'seccode');
ifselected($settings['comment_order'],'comment_order');
ifselected($settings['attachments_thumbs'],'attachments_thumbs');
ifselected($settings['display_attach'],'display_attach');
ifselected($settings['remote_open'],'remote_open');
ifselected($settings['close'],'close');
ifselected($settings['seccode_enable'],'seccode_enable');
ifselected($settings['gzipcompress'],'gzipcompress');
ifselected($settings['showmsg'],'showmsg');
ifselected($settings['watermark'],'watermark');
ifselected($settings['rewrite_enable'],'rewrite_enable');
ifselected($settings['banip_enable'],'banip_enable');
ifselected($settings['spam_enable'],'spam_enable');
ifselected($settings['rss_enable'],'rss_enable');

$article_order = '';
$article_order[$settings['article_order']] = 'selected';
$attack_reject[$settings['attack_reject']] = 'selected';
$attachments_save_dir[$settings['attachments_save_dir']] = 'selected';
$attachments_display[$settings['attachments_display']] = 'selected';
$settings['server_timezone'] < 0 ? ${'zone_0'.str_replace('.','_',abs($settings['server_timezone']))}='selected' : ${'zone_'.str_replace('.','_',$settings['server_timezone'])}='selected';
$waterpos[$settings['waterpos']] = 'selected';

$gd_version = gd_version();
$gd_version = $gd_version ? '服务器GD版本:'.$gd_version : '服务器不支持GD,因此该功能无法正常使用.';

cpheader();
include PrintEot('configurate');
?>
