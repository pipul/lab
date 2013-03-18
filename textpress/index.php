<?php

// 文件说明：前台主程序

require_once('global.php');
$os = getSystem();

if (!$action) {
	$action = 'index';
}

//文章列表
if ( $action == 'index' ) {
	$pageurl = './?action=index';
	session_start();
	$pagenum = intval($options['normal_shownum']);
	$articlenum = $DB->num_rows($DB->query("SELECT articleid FROM {$db_prefix}articles WHERE visible = '1'"));
	$timeformat = $options['normaltime'];
	$query_sql = "SELECT a.articleid,a.cid,a.stick,a.dateline,a.title,a.readpassword,c.name as cname,u.username
		FROM {$db_prefix}articles a 
		LEFT JOIN {$db_prefix}categories c ON c.cid=a.cid
		LEFT JOIN {$db_prefix}users u ON a.uid=u.userid
		WHERE a.visible='1'";
	
	if($page) {
		$start_limit = ($page - 1) * $pagenum;
	} else {
		$start_limit = 0;
		$page = 1;
	}
	
	// 读取分类目录
	$catedb = array();
	$query = $DB->query("SELECT * FROM {$db_prefix}categories");
	while ($cate = $DB->fetch_array($query)) {
		$catedb[] = $cate;
	}
	unset($cate);
	
	// 查看首页文章

	$tatol = $articlenum;
	
	// 检查是否设置$cid参数
	$cateadd = '';
	$cid = intval($_GET['cid']);
	if ($cid) {			
		$cateadd = " AND a.cid='$cid' ";
		$query_sql .= " AND a.cid='$cid' ";
		$r = $DB->fetch_one_array("SELECT name,articles FROM {$db_prefix}categories WHERE cid='$cid'");
		$tatol = $r['articles'];
		$pageurl .= '&amp;cid='.$cid;
	}
	// 检查是否设置$setdate参数
	if ($setdate && strlen($setdate) == 6) {
		// 检查是否设置$setday参数
		$pageurl .= '&amp;setdate='.$setdate;
		$setday = intval($_GET['setday']);
		if ($setday && is_numeric($setday)) {
			if ($setday > 31 || $setday < 1) {
				$setday = sadate('d');
			}
			$start = strtotime($setyear.'-'.$setmonth.'-'.$setday);
			$end = $start + 86400;
			$pageurl .= '&amp;setday='.$setday;
		}
	}
	//*******************************//
	$startadd = $start ? " AND a.dateline >= '".correcttime($start)."' " : '';
	$endadd   = $end ? " AND a.dateline < '".correcttime($end)."' " : '';
	//*******************************//
	if($setdate || $setday) {
		$query = $DB->query("SELECT COUNT(*) FROM {$db_prefix}articles a WHERE a.visible='1' ".$cateadd.$startadd.$endadd);
		$tatol = $DB->result($query, 0);
	}
	//*******************************//
	$query_sql .= $startadd.$endadd." ORDER BY a.stick DESC, a.$article_order DESC LIMIT $start_limit, ".$pagenum;

	$query = $DB->query($query_sql);
	$multipage = multi($tatol, $pagenum, $page, $pageurl);
	$articledb=array();
	while ($article = $DB->fetch_array($query)) {

		$article['dateline'] = sadate($timeformat, $article['dateline']);
		$article['title'] = trimmed_title($article['title'], $options['title_limit']);

		if ($article['readpassword'] && ($_SESSION['readpassword_'.$article['articleid']] != $article['readpassword']) && $txp_group != 1 && $txp_group != 2) {
			$article['allowread'] = false;
		} else {
			$article['allowread'] = true;
		}

		$articledb[]=$article;
	}
	unset($article);

	// 友情链接
	$query = $DB->query("SELECT displayorder,name,url,note FROM {$db_prefix}links WHERE visible = '1' ORDER BY displayorder ASC, name ASC");
	$linkdb = array();
	while ($link = $DB->fetch_array($query)) {
		$link['note'] = $link['note'] ? $link['note'] : $link['url'];
		$linkdb[] = $link;
	}
	unset($link);

	$DB->free_result($query);
	$pagefile = 'content';


	require_once PrintEot('index');
	footer();
}

// 归档和搜索
elseif ( $action == 'archives' ) {
	$timeformat = $options['normaltime'];
	$keywords = addslashes(trim($_POST['keywords'] ? $_POST['keywords'] : $_GET['keywords']));
	
	// 读取分类目录
	$catedb = array();
	$query = $DB->query("SELECT * FROM {$db_prefix}categories");
	while ($cate = $DB->fetch_array($query)) {
		$catedb[] = $cate;
	}
	unset($cate);

	if (!$keywords) {
		$query = $DB->query("SELECT dateline FROM {$db_prefix}articles WHERE visible = '1' ORDER BY dateline DESC");
		$articledb = $arr = $archivedb = array();
		while ($article = $DB->fetch_array($query)) {
			$articledb[] = sadate('Y-m',$article['dateline']);
		}
		unset($article);
		$arr = array_count_values($articledb);
		unset($articledb);
		foreach($arr as $key => $val){
			list($y, $m) = explode('-', $key);
			$archivedb[$y][$m] = $val;
		}

		$pagefile = 'archives';

	} else {
		$keywords = str_replace("_","\_",$keywords);
		$keywords = str_replace("%","\%",$keywords);
		if(preg_match("(AND|\+|&|\s)", $keywords) && !preg_match("(OR|\|)", $keywords)) {
			$andor = ' AND ';
			$sqltxtsrch = '1';
			$keywords = preg_replace("/( AND |&| )/is", "+", $keywords);
		} else {
			$andor = ' OR ';
			$sqltxtsrch = '0';
			$keywords = preg_replace("/( OR |\|)/is", "+", $keywords);
		}
		$keywords = str_replace('*', '%', addcslashes($keywords, '%_'));
		foreach(explode("+", $keywords) AS $text) {
			$text = trim($text);
			if($text) {
				$sqltxtsrch .= $andor;
				$sqltxtsrch .= ($_POST['searchin'] == 'title') ? "title LIKE '%".$text."%'" : "(content LIKE '%".str_replace('_', '\_', $text)."%' OR description LIKE '%".$text."%' OR title LIKE '%".$text."%')";
			}
		}
				
		//搜索文章
		$query_sql = "SELECT articleid,dateline,title,readpassword FROM {$db_prefix}articles WHERE visible='1'";
		$sortby = 'dateline';
		$orderby = 'desc';
		$cid = $_POST['cid'];
				
		//设置搜索的分类
		$catearray = array();
		if($cid) {
			foreach((is_array($cid) ? $cid : explode('_', $cid)) as $cateid) {
				if($cateid = intval(trim($cateid))) {
					$catearray[] = $cateid;
				}
			}
		}
		
		$cids = $comma = '';
		foreach($catedb as $data) {
			if(!$catearray || in_array($data['cid'], $catearray)) {
				$cids .= $comma.intval($data['cid']);
				$comma = ',';
			}
		}
			
		if ($cids) {
			$query_sql .= " AND cid IN ($cids)";
		}
				
		//设置搜索的归档日期

		$articledb = $archivedb = array();
		
		$query = $DB->query("SELECT dateline FROM {$db_prefix}articles WHERE visible = '1' ORDER BY dateline DESC");
		while ($article = $DB->fetch_array($query)) {
			$articledb[] = sadate('Y-m',$article['dateline']);
		}
		unset($article);
		$DB->free_result($query);
		$archivecount = array_count_values($articledb);
		unset($articledb);
		
		foreach($archivecount as $key => $val){
			$key = str_replace('-', '', $key);
			$archivedb[] = $key;
		}
		
		if (in_array($_POST['dateline'], $archivedb)) {
			$dateline = intval($_POST['dateline']);
			$setyear = substr($dateline,0,4);
			$setmonth = substr($dateline,-2);
			list($start, $end) = explode('-', gettimestamp($setyear,$setmonth));
		}
		$startadd = $start ? " AND dateline >= '".correcttime($start)."' " : '';
		$endadd   = $end ? " AND dateline < '".correcttime($end)."' " : '';
		$query_sql .= $startadd.$endadd." AND ($sqltxtsrch) ORDER BY $sortby $orderby";
		
		/* 开始搜索 */
		$query = $DB->query($query_sql);
			
		$articledb = array();
		while ($article = $DB->fetch_array($query)) {
			$article['dateline'] = sadate($timeformat, $article['dateline']);
			$article['title'] = trimmed_title($article['title'], $options['title_limit']);

			if ($article['readpassword'] && ($_SESSION['readpassword_'.$article['articleid']] != $article['readpassword']) && $txp_group != 1 && $txp_group != 2) {
				$article['allowread'] = false;
			} else {
				$article['allowread'] = true;
			}
			$articledb[]=$article;
		}
		unset($article);
		$pagefile = 'content';
	}
	
	$DB->free_result($query);	
	require_once PrintEot('index');
	footer();

}
// 显示文章
elseif ( $action == 'show') {
	session_start();
	$articleid = intval($_GET['id']);

	// 获取文章信息	
	$article = $DB->fetch_one_array("SELECT a.*,c.name as cname,u.username
		FROM {$db_prefix}articles a
		LEFT JOIN {$db_prefix}categories c ON c.cid=a.cid
		LEFT JOIN {$db_prefix}users u ON a.uid=u.userid
		WHERE a.visible='1' AND articleid='$articleid'");
	if (!$article) {
		message('记录不存在', './');
	}
	if ($_POST['readpassword'] && ($article['readpassword'] == addslashes($_POST['readpassword']))) {
		$_SESSION['readpassword_'.$articleid] = addslashes($_POST['readpassword']);
	}

	//设置文章的分类名、作者、TAG、标题成为meta\title信息
	$options['meta_keywords'] = $article['cname'].','.$article['username'].','.$article['title'].','.$options['meta_keywords'];
	$options['meta_description'] = $article['cname'].','.$article['username'].','.$article['title'].','.$options['meta_description'];
	$options['title_keywords'] = ' - '.$article['cname'].','.$article['username'].','.$options['title_keywords'];

	//隐藏变量,默认模板用不着,方便那些做模板可以单独显示月份和号数的的朋友.
	$article['month'] = sadate('M', $article['dateline']);
	$article['day'] = sadate('d', $article['dateline']);

	$article['dateline'] = sadate($options['normaltime'], $article['dateline']);

	if ($article['readpassword'] && ($_SESSION['readpassword_'.$articleid] != $article['readpassword']) && $txp_group != 1 && $txp_group != 2) {
		$article['allowread'] = false;
	} else {
		$article['allowread'] = true;
		$DB->unbuffered_query("UPDATE {$db_prefix}articles SET views=views+1 WHERE articleid='$articleid'");

		// 跳转
		$goto = $_GET['goto'];
		$article_comment_num = intval($options['article_comment_num']);
		if ($goto == 'newcm') {
			//跳转到评论
			$cmid = intval($_GET['cmid']);
			if ($options['comment_order']) {
				$cmnum = '#cm'.$cmid;
				if ($article_comment_num) {
					$cpost = $DB->fetch_one_array("SELECT COUNT(*) as comments FROM {$db_prefix}comments WHERE articleid='$articleid' AND visible='1' AND commentid<='$cmid'");
					if (($cpost['comments'] / $article_comment_num) <= 1 ) {
						$page = 1;
					} else {
						$page = @ceil(($cpost['comments']) / $article_comment_num);
					}
				} else {
					$page = 1;
				}
				if ($options['showmsg']) {
					message('正在读取.请稍侯.', getarticleurl($articleid, $page).$cmnum);
				} else {
					@header('Location: '.getarticleurl($articleid, $page).$cmnum);
				}
			} else {
				if ($options['showmsg']) {
					message('正在读取.请稍侯.', getarticleurl($articleid).'#comment');
				} else {
					@header('Location: '.getarticleurl($articleid).'#comment');
				}
			}
		} elseif ($goto == 'next') {
			//跳转到下一篇文章
			$query    = $DB->query("SELECT dateline FROM {$db_prefix}articles WHERE articleid='$articleid'");
			$lastpost = $DB->result($query, 0);
			$row      = $DB->fetch_one_array("SELECT articleid FROM {$db_prefix}articles WHERE dateline > '$lastpost' AND visible='1' ORDER BY dateline ASC LIMIT 1");
			if($row) {
				if ($options['showmsg']) {
					message('正在读取.请稍侯.', getarticleurl($row['articleid']));
				} else {
					@header('Location: '.getarticleurl($row['articleid']));
				}
			} else {
				message('没有比当前更新的文章', getarticleurl($articleid));
			}
		} elseif ($goto == 'previous') {
			//跳转到上一篇文章
			$query    = $DB->query("SELECT dateline FROM {$db_prefix}articles WHERE articleid='$articleid'");
			$lastpost = $DB->result($query, 0);
			$row      = $DB->fetch_one_array("SELECT articleid FROM {$db_prefix}articles WHERE dateline < '$lastpost' AND visible='1' ORDER BY dateline DESC LIMIT 1");
			if($row) {
				if ($options['showmsg']) {
					message('正在读取.请稍侯.', getarticleurl($row['articleid']));
				} else {
					@header('Location: '.getarticleurl($row['articleid']));
				}
			} else {
				message('没有比当前更早的文章', getarticleurl($articleid));
			}
		}
		//附件
		if ($article['attachments']) {
			require_once(TXP_ROOT.'include/attachment.php');
			$attachs= unserialize(stripslashes_array($article['attachments']));
			if (is_array($attachs)) {
				foreach ($attachs AS $attach) {
					$a_path = $options['attachments_dir'].$attach['filepath'];
					if (file_exists($a_path)) {
						$a_ext = strtolower(getextension($attach['filename']));
						if ($a_ext == 'gif' || $a_ext == 'jpg' || $a_ext == 'jpeg' || $a_ext == 'png') {
							$imagesize = @getimagesize($a_path);
							$a_size = sizecount($attach['filesize']);
							$a_thumb_path = $options['attachments_dir'].$attach['thumb_filepath'];
							if ($attach['thumb_filepath'] && $options['attachments_thumbs'] && file_exists($a_thumb_path)) {
								$article['image'][$attach['attachmentid']]=array($attach['attachmentid'],$a_thumb_path,$a_size,$attach['thumb_width'],$attach['thumb_height'],$attach['downloads'],1);
							} else {
								// 如果缩略图不存在
								$size = explode('x', strtolower($options['attachments_thumbs_size']));
								$im = scale_image( array(
									'max_width'  => $size[0],
									'max_height' => $size[1],
									'cur_width'  => $imagesize[0],
									'cur_height' => $imagesize[1]
								));	
								$article['image'][$attach['attachmentid']]=array($attach['attachmentid'],$a_path,$a_size,$im['img_width'],$im['img_height'],$attach['downloads'],0);
							}
						} else {
							// 如果非图片文件
							$a_size = sizecount($attach['filesize']);	
							$article['file'][$attach['attachmentid']]=array($attach['attachmentid'],$attach['filename'],$a_size,$attach['downloads']);
						}
					}
				}
				//如果空,释放掉变量
				$attachmentids=array();

				$article['content'] = preg_replace("/\[attach=(\d+)\]/ie", "upload('\\1')", $article['content']);

				foreach($attachmentids as $key => $value){
					if($article['image'][$value]){
						unset($article['image'][$value]);
					}
					if($article['file'][$value]){
						unset($article['file'][$value]);
					}
				}
			}
		}
		// 获取附件结束

		//处理PHP高亮
		$article['content'] = preg_replace("/\s*\[php\](.+?)\[\/php\]\s*/ies", "phphighlite('\\1')", $article['content']);
			
		// 评论	
		if ($article['comments']) {
			$commentsql = '';
			if($article_comment_num) {
				if($page) {
					$cmtorderid = ($page - 1) * $article_comment_num;
					$start_limit = ($page - 1) * $article_comment_num;
				} else {
					$cmtorderid = 0;
					$start_limit = 0;
					$page = 1;
				}
				$multipage = multi($article['comments'], $article_comment_num, $page, "./?action=show&amp;id=$articleid");
				$commentsql = " LIMIT $start_limit, $article_comment_num";
			}
			$cmtorder = $options['comment_order'] ? 'ASC' : 'DESC';
			$query = $DB->query("SELECT commentid,author,url,dateline,content FROM {$db_prefix}comments WHERE articleid='$articleid' AND visible='1' ORDER BY commentid $cmtorder $commentsql");
			$commentdb=array();
			while ($comment=$DB->fetch_array($query)) {
				$cmtorderid++;
				$comment['cmtorderid'] = $cmtorderid;
				$comment['quoteuser'] = $comment['author'];
				if ($comment['url']) {
					if (isemail($comment['url'])) {
						//分解邮件地址并采用javascript输出
						$frontlen = strrpos($comment['url'], '@');
						$front    = substr($comment['url'], 0, $frontlen);
						$emaillen = strlen($comment['url']);
						$back     = substr($comment['url'], $frontlen+1, $emaillen);
						$comment['author'] = "<a href=\"javascript:navigate('mai' + 'lto:' + '".$front."' + '@' + '".$back."')\" target=\"_blank\">".$comment['author']."</a>";
					} else {
						$comment['author'] = '<a href="'.$comment['url'].'" target="_blank">'.$comment['author'].'</a>';
					}
				}
				$comment['content'] = html_clean($comment['content']);
				$comment['dateline'] = sadate($options['comment_timeformat'], $comment['dateline']);
				$commentdb[]=$comment;
			}
			unset($comment);
			$DB->free_result($query);
		}
	}
	$options['title'] = $article['title'];
	$pagefile = 'show';
	require_once PrintEot('index');
	footer();

}
// 登陆
elseif ( $action == 'login') {
	$pagefile = 'login';
	require_once PrintEot('login');
}

?>
