<?php

// 文件说明：编辑器

if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

include('fckeditor/fckeditor.php') ;


$oFCKeditor = new FCKeditor('description');
$oFCKeditor->Value = $article['description'];
$oFCKeditor->Height = '200';
$oFCKeditor->ToolbarSet = 'Basic';

$descriptionarea = $oFCKeditor->CreateHtml();

$oFCKeditor = new FCKeditor('content');
$oFCKeditor->Value = $article['content'];

$contentarea = $oFCKeditor->CreateHtml();

?>
