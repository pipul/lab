<?php

// 文件说明：伪静态模式下模板的链接处理


if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}

// 转换分类的连接
function rewrite_category($head, $cid, $page = 1, $class = '') {
	global $options;
	!$page && $page = 1;
	return '<a href="'.$head.'cat-'.$cid.'-'.$page.'.'.$options['rewrite_ext'].'"'.($class ? ' class="'.$class.'"' : '').'>';
}

// 转换归档的连接
function rewrite_archives($head, $date, $page = 1, $class = '') {
	global $options;
	!$page && $page = 1;
	return '<a href="'.$head.'archives-'.$date.'-'.$page.'.'.$options['rewrite_ext'].'"'.($class ? ' class="'.$class.'"' : '').'>';
}

// 转换文章的连接
function rewrite_post($head, $id, $page = 1, $extra = '', $class = '') {
	global $options;
	!$page && $page = 1;
	return '<a href="'.$head.'show-'.$id.'-'.$page.'.'.$options['rewrite_ext'].($extra ? '#'.$extra : '').'"'.($class ? ' class="'.$class.'"' : '').'>';
}

// 转换其他页面的连接
function rewrite_page($head, $action, $page = 1, $class = '') {
	global $options;
	!$page && $page = 1;
	if (in_array($action,array('search','archives','reg','login','links'))) {
		$pagelink = '';
	} else {
		$pagelink = '-'.$page;
	}
	return '<a href="'.$head.$action.$pagelink.'.'.$options['rewrite_ext'].'"'.($class ? ' class="'.$class.'"' : '').'>';
}

?>
