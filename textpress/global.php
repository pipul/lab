<?php

// 文件说明：前台公共函数


// 加载公用函数
require_once('include/common.php');

// 检查模版
$t_dir = TXP_ROOT.'content/templates/'.$options['templatename'].'/';
if (!is_dir($t_dir)) {
	if (is_dir(TXP_ROOT.'content/templates/default')) {
		$options['templatename'] = 'default';
	} else {
		exit('Template Error: '.$t_dir.' is not a directory.');
	}
}

// 状态检查
if ($options['close']) {
	message(html_clean($options['close_note']),'');
}

// 检查浏览模式 (已关闭)

// 获取时间，假如是WIN系统，一定要做范围的限制。否则.....
$setdate = intval($_GET['setdate']);
if ($setdate && strlen($setdate) == 6) {
	$setyear = substr($setdate,0,4);
	if ($setyear >= 2038 || $setyear <= 1970) {
		$setyear = sadate('Y');
		$setmonth = sadate('m');
		$start = $end = 0;
	} else {
		$setmonth = substr($setdate,-2);
		list($start, $end) = explode('-', gettimestamp($setyear,$setmonth));
	}
} else {
	$setyear = sadate('Y');
	$setmonth = sadate('m');
	$start = $end = 0;
}

// 查询按月归档
$monthname = array('','January','February','March','April','May','June','July','August','September','October','November','December');

// 分页函数
function multi($num, $perpage, $curpage, $mpurl) {
	$multipage = '';
	$mpurl .= strpos($mpurl, '?') ? '&amp;' : '?';
	if($num > $perpage) {
		$page = 10;
		$offset = 5;
		$pages = @ceil($num / $perpage);
		if($page > $pages) {
			$from = 1;
			$to = $pages;
		} else {
			$from = $curpage - $offset;
			$to = $curpage + $page - $offset - 1;
			if($from < 1) {
				$to = $curpage + 1 - $from;
				$from = 1;
				if(($to - $from) < $page && ($to - $from) < $pages) {
					$to = $page;
				}
			} elseif($to > $pages) {
				$from = $curpage - $pages + $to;
				$to = $pages;
				if(($to - $from) < $page && ($to - $from) < $pages) {
					$from = $pages - $page + 1;
				}
			}
		}

		$multipage = ($curpage - $offset > 1 && $pages > $page ? '<a href="'.$mpurl.'page=1" class="p_redirect">&laquo;</a>' : '').($curpage > 1 ? '<a href="'.$mpurl.'page='.($curpage - 1).'" class="p_redirect">&#8249;</a>' : '');
		for($i = $from; $i <= $to; $i++) {
			$multipage .= $i == $curpage ? '<span class="p_curpage">'.$i.'</span>' : '<a href="'.$mpurl.'page='.$i.'" class="p_num">'.$i.'</a>';
		}
		$multipage .= ($curpage < $pages ? '<a href="'.$mpurl.'page='.($curpage + 1).'" class="p_redirect">&#8250;</a>' : '').($to < $pages ? '<a href="'.$mpurl.'page='.$pages.'" class="p_redirect">&raquo;</a>' : '');
		$multipage = $multipage ? '<div class="p_bar"><span class="p_info"><a href="./?action=archives">查看全站归档</a>:'.$num.'</span>'.$multipage.'</div>' : '';
	}
	return $multipage;
}

// 清除HTML代码
function html_clean($content) {
	$content = htmlspecialchars($content);
	$content = str_replace("\n", "<br />", $content);
	$content = str_replace("  ", "&nbsp;&nbsp;", $content);
	$content = str_replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;", $content);
	$content = preg_replace("/\[quote=(.*?)\]\s*(.+?)\s*\[\/quote\]/is", "<div style=\"font-weight: bold\">引用 \\1 说过的话:</div><div class=\"quote\">\\2</div>", $content);
	return $content;
}

// 检查操作系统
function getSystem(){
    $sys = $_SERVER['HTTP_USER_AGENT'];
    if(stripos($sys, "NT 6.1"))
       $os = "Windows 7";
    elseif(stripos($sys, "NT 6.0"))
       $os = "Windows Vista";
    elseif(stripos($sys, "NT 5.1"))
       $os = "Windows XP";
    elseif(stripos($sys, "NT 5.2"))
       $os = "Windows Server 2003";
    elseif(stripos($sys, "NT 5"))  
       $os = "Windows 2000";
    elseif(stripos($sys, "NT 4.9"))
       $os = "Windows ME";  
    elseif(stripos($sys, "NT 4"))
       $os = "Windows NT 4.0";
    elseif(stripos($sys, "98"))
       $os = "Windows 98";
    elseif(stripos($sys, "95"))
       $os = "Windows 95";
    elseif(stripos($sys, "Mac"))
       $os = "OK";
    elseif(stripos($sys, "Linux"))
       $os = "OK";
    elseif(stripos($sys, "Unix"))
       $os = "OK";
    elseif(stripos($sys, "FreeBSD"))
       $os = "OK";
    elseif(stripos($sys, "SunOS"))
       $os = "OK";
    elseif(stripos($sys, "BeOS"))
       $os = "OK";
    elseif(stripos($sys, "OS/2"))
       $os = "OK";
    elseif(stripos($sys, "PC"))
       $os = "OK"; // Macintosh
    elseif(stripos($sys, "AIX"))
       $os = "OK";
    else
       $os = "None";
    return $os;
}

// 高亮显示PHP
function phphighlite($code) {
	if (floor(phpversion())<4) {
		$buffer = $code;
	} else {
		$code = preg_replace("/<style .*?<\/style>/is", "", $code);
		$code = preg_replace("/<script .*?<\/script>/is", "", $code);
		$code = preg_replace("/<br\s*\/?>/i", "\n", $code);
		$code = preg_replace("/<\/?p>/i", "\n", $code);
		$code = preg_replace("/<\/?td>/i", "\n", $code);
		$code = preg_replace("/<\/?div>/i", "\n", $code);
		$code = preg_replace("/<\/?blockquote>/i", "\n", $code);
		$code = preg_replace("/<\/?li>/i", "\n", $code);
		$code = strip_tags($code);
		$code = preg_replace("/\&\#.*?\;/i", "", $code);
		$code = str_replace("&nbsp;&nbsp;&nbsp;", "&nbsp;&nbsp;", $code);
		$code = str_replace("&nbsp;&nbsp;", "&nbsp;", $code);
		$code = str_replace("&nbsp;", "\t", $code);
		$code = str_replace("&quot;", '"', $code);
		$code = str_replace("<br>", "", $code);
		$code = str_replace("<br />", "", $code);
		$code = str_replace("&gt;", ">", $code);
		$code = str_replace("&lt;", "<", $code);
		$code = str_replace("&amp;", "&", $code);
		//$code = str_replace('$', '\$', $code);
		if (!strpos($code,"<?\n") and substr($code,0,4)!="<?\n") {
			$code="<?".trim($code)."?>";
			$addedtags=1;
		}
		ob_start();
		$oldlevel=error_reporting(0);
		highlight_string($code);
		error_reporting($oldlevel);
		$buffer = ob_get_contents();
		ob_end_clean();
		if ($addedtags) {
		  $openingpos = strpos($buffer,"&lt;?");
		  $closingpos = strrpos($buffer, "?");
		  $buffer=substr($buffer, 0, $openingpos).substr($buffer, $openingpos+5, $closingpos-($openingpos+5)).substr($buffer, $closingpos+5);
		}
		$buffer = str_replace("&quot;", "\"", $buffer);
	}
	return $buffer;
}

// 获取页面调试信息
function footer() {
	global $DB, $starttime, $options;
	$mtime = explode(' ', microtime());
	$totaltime = number_format(($mtime[1] + $mtime[0] - $starttime), 6);
	$gzip = $options['gzipcompress'] ? 'enabled' : 'disabled';
	$sa_debug = 'Processed in '.$totaltime.' second(s), '.$DB->querycount.' queries, Gzip '.$gzip;
	PageEnd();
}

// 同上
function upload($aid){
	global $article, $attachmentids, $options;
	if ($article['image'][$aid]) {
		$attachmentids[]=$aid;
		return "<a href=\"".$options['url']."attachment.php?id={$article[image][$aid][0]}\" target=\"_blank\"><img src=\"".$options['url']."{$article[image][$aid][1]}\" border=\"0\" alt=\"大小: {$article[image][$aid][2]}&#13;尺寸: {$article[image][$aid][3]} x {$article[image][$aid][4]}&#13;浏览: {$article[image][$aid][5]} 次&#13;点击打开新窗口浏览全图\" width=\"{$article[image][$aid][3]}\" height=\"{$article[image][$aid][4]}\" /></a>";
	} elseif ($article['file'][$aid]) {
		$attachmentids[]=$aid;
		return "<a href=\"".$options['url']."attachment.php?id={$article[file][$aid][0]}\" title=\"{$article[file][$aid][2]}, 下载次数:{$article[file][$aid][3]}\" target=\"_blank\">{$article[file][$aid][1]}</a>";
	} else {
		return "[attach=$aid]";
	}
}

// 消息显示页面
function message($msg,$returnurl='javascript:history.go(-1);',$min='3') {
	global $options;
	require_once PrintEot('message');
	PageEnd();
}

// 获得模板文件的路径
function PrintEot($template){
	global $options;
	if(!$template) $template='none';
	$path = TXP_ROOT.'content/templates/'.$options['templatename'].'/'.$template.'.php';
	if (file_exists($path)) {
		return $path;
	} else {
		return TXP_ROOT.'content/templates/default/'.$template.'.php';
	}
}

// 根据伪静态状态返回相应的文章连接
function getarticleurl($articleid, $page = 1) {
	global $options;
	!$page && $page = 1;
	if ($options['rewrite_enable']) {
		$articleurl = 'show-'.$articleid.'-'.$page.'.'.$options['rewrite_ext'];
	} else {
		$articleurl = './?action=show&id='.$articleid.'&page='.$page;
	}
	return $articleurl;
}

$modelink = '';
if ($action) {
	$modelink .= '&amp;action='.$action;
}
if ($cid) {
	$modelink .= '&amp;cid='.$cid;
}
if ($setdate) {
	$modelink .= '&amp;setdate='.$setdate;
}
if ($setday) {
	$modelink .= '&amp;setday='.$setday;
}
if (intval($_GET['searchid'])) {
	$modelink .= '&amp;searchid='.$_GET['searchid'];
}
if (intval($_GET['userid'])) {
	$modelink .= "&amp;userid=".$_GET['userid'];
}
if ($_GET['item']) {
	$item = urlencode(addslashes($item));
	$modelink .= '&amp;item='.$item;
}
?>
