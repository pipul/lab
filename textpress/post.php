<?php

// 文件说明：对提交的数据进行操作

require_once('global.php');

if($_SERVER['REQUEST_METHOD'] == 'POST' && (empty($_SERVER['HTTP_REFERER']) || $_POST['formhash'] != formhash() || preg_replace("/https?:\/\/([^\:\/]+).*/i", "\\1", $_SERVER['HTTP_REFERER']) !== preg_replace("/([^\:]+).*/", "\\1", $_SERVER['HTTP_HOST']))) {
	message('您的请求来路不正确,无法提交');
}

//如果开启伪静态则设置伪静态的页面的URL
if ($options['rewrite_enable']) {
	$loginurl = 'login.'.$options['rewrite_ext'];
} else {
	$loginurl = './?action=login';
}

//登陆
if($_POST['action'] == 'dologin') {
	if ($options['seccode_enable']) {
		$clientcode = $_POST['clientcode'];
		session_start();
		if (!$clientcode || strtolower($clientcode) != strtolower($_SESSION['code'])) {
			unset($_SESSION['code']);
			message('验证码错误,请返回重新输入.', $loginurl);
		}
	}
	// 取值并过滤部分
	$username = char_cv(trim($_POST['username']));
	$password = md5($_POST['password']);
	$userinfo = $DB->fetch_one_array("SELECT userid,password,logincount,url,groupid FROM {$db_prefix}users WHERE username='$username'");
	if($userinfo['userid'] && $userinfo['password'] == $password) {
		$DB->unbuffered_query("UPDATE {$db_prefix}users SET logincount=logincount+1, logintime='$timestamp', loginip='$onlineip' WHERE userid='".$userinfo['userid']."'");
		$logincount = $userinfo['logincount']+1;
		setcookie('txp_auth', authcode("$userinfo[userid]\t$password\t$logincount"), $timestamp+2592000);
		message('登陆成功', './');
	} else {
		message('登陆失败', $loginurl);
	}
}

//注销
if ($_GET['action'] == 'logout') {
	setcookie('txp_auth', '');
	setcookie('comment_post_time', '');
	setcookie('search_post_time', '');
	setcookie('comment_username', '');
	setcookie('comment_url', '');
	message('注销成功', './');
}

//添加评论
if($_POST['action'] == 'addcomment') {
	$cookietime = $timestamp+2592000;
	$articleid = intval($_POST['articleid']);
	$username = trim($_POST['username']);
	$password = $_POST['password'];
	$url = trim($_POST['url']);
	$content = addslashes(trim($_POST['content']));
	//把评论内容保存到cookie里以免丢失
	setcookie('cmcontent', $content, $cookietime);

	if (!$articleid) {
		message('缺少必要参数', './');
	}

	//禁止IP	
	if ($options['banip_enable'] && $options['ban_ip']) {
		$options['ban_ip'] = str_replace('，', ',', $options['ban_ip']);
		$ban_ips = explode(',', $options['ban_ip']);
		if (is_array($ban_ips) && count($ban_ips)) {
			foreach ($ban_ips AS $ban_ip) {
				$ban_ip = str_replace( '\*', '.*', preg_quote($ban_ip, "/") );
				if (preg_match("/^$ban_ip/", $onlineip)) {
					message('您的IP已经被系统禁止发表评论.', getarticleurl($articleid));
				}
			}
		}
	}

	if ($options['seccode'] && $txp_group != 1 && $txp_group !=2) {
		$clientcode = $_POST['clientcode'];
		session_start();
		if (!$clientcode || strtolower($clientcode) != strtolower($_SESSION['code'])) {
			unset($_SESSION['code']);
			message('验证码错误,请返回重新输入.', getarticleurl($articleid));
		}
	}

	//如果没有登陆
	if (!$txp_uid) {
		if(!$username || strlen($username) > 30) {
			message('用户名为空或用户名太长.', getarticleurl($articleid).'#addcomment');
		}
		$name_key = array("\\",'&',' ',"'",'"','/','*',',','<','>',"\r","\t","\n",'#','$','(',')','%','@','+','?',';','^');
		foreach($name_key as $value){
			if (strpos($username,$value) !== false){ 
				message('此用户名包含不可接受字符或被管理员屏蔽,请选择其它用户名.', getarticleurl($articleid).'#addcomment');
			}
		}
		$username = char_cv($username);
		$url = char_cv($url);
		
		//把用户名和URL信息保存到cookie
		setcookie('comment_username',$username,$cookietime);
		setcookie('comment_url',$url,$cookietime);
		

	} else {
		//如果已登陆
		$user = $DB->fetch_one_array("SELECT userid,username,logincount,groupid,password,url FROM {$db_prefix}users WHERE userid='$txp_uid'");
		if ($user['userid'] && $user['password'] == $txp_pw && $user['logincount'] == $logincount && $user['groupid'] == $txp_group) {
			$username = addslashes($user['username']);
			$url = addslashes($user['url']);
		} else {			
			message('读取用户信息出错,请重新登陆.', $loginurl);
		}
	}
	
	// 检查限制选项
	if ($options['audit_comment']) {
		$spam = TRUE;
	} elseif ($options['spam_enable']) {
		//链接次数
		if (substr_count($content, 'http://') >= $options['spam_url_num']) {
			$spam = TRUE;
		}
		//禁止词语
		if ($options['spam_words']) {
			$options['spam_words'] = str_replace('，', ',', $options['spam_words']);
			$badwords = explode(',', $options['spam_words']);
			if (is_array($badwords) && count($badwords) ) {
				foreach ($badwords AS $n) {
					if ($n) {
						if (preg_match( "/".preg_quote($n, '/' )."/i", $content)) {
							$spam = TRUE;
							break;
						}
					}
				}
			}
		}
		//内容长度
		if (strlen($content) >= $options['spam_content_size']) {
			$spam = TRUE;
		}
	} else {
		$spam = FALSE;
	}

	$visible = $spam ? '0' : '1';
	
	if ($txp_group != 1 && $txp_group != 2) {
		$lastposttime = $user['lastpost'] ? $user['lastpost'] : $_COOKIE['comment_post_time'];
		if ($options['comment_post_space'] && $timestamp - $lastposttime <= $options['comment_post_space'] && $txp_group != 1){
			message('为防止灌水,发表评论时间间隔为'.$options['comment_post_space'].'秒.', getarticleurl($articleid).'#addcomment');
		}
	}
	$article = $DB->fetch_one_array("SELECT closecomment FROM {$db_prefix}articles WHERE articleid='$articleid'");
	if ($article['closecomment']) {
		message('本文因为某种原因此时不允许访客进行评论.', getarticleurl($articleid));
	}
	$result  = '';
	$result .= checkurl($url);
	$result .= checkcontent($content);
	if($result){
		message($result, getarticleurl($articleid).'#addcomment');
	}
    $r = $DB->fetch_one_array("SELECT commentid FROM {$db_prefix}comments WHERE articleid='$articleid' AND author='$username' AND content='$content'");
    if($r['commentid']) {
		message('该评论已存在.', getarticleurl($articleid));
	}
	unset($r);
    $DB->query("INSERT INTO {$db_prefix}comments (articleid, author, url, dateline, content, ipaddress, visible) VALUES ('$articleid', '$username', '$url', '$timestamp', '$content', '$onlineip', '$visible')");
	$cmid = $DB->insert_id();
	if ($txp_uid) {
		$DB->unbuffered_query("UPDATE {$db_prefix}users SET lastpost='$timestamp' WHERE userid='$txp_uid'");
		// 更新用户最后发表时间
	}
	if (!$spam) {
		// 如果不是垃圾则更新当前文章评论数
		$DB->unbuffered_query("UPDATE {$db_prefix}articles SET comments=comments+1 WHERE articleid='$articleid'");
	}
	setcookie('comment_post_time',$timestamp);
	// 跳转到最新发表的评论
	$cmnum = '#cm'.$cmid;
	$article_comment_num = intval($options['article_comment_num']);
	if ($article_comment_num) {
		$cpost = $DB->fetch_one_array("SELECT COUNT(*) as comment FROM {$db_prefix}comments WHERE articleid='$articleid' AND visible='1' AND commentid<='$cmid'");
		if (($cpost['comment'] / $article_comment_num) <= 1 ) {
			$page = 1;
		} else {
			$page = @ceil(($cpost['comment']) / $article_comment_num);
		}
	} else {
		$page = 1;
	}
	if ($spam) {
		message('添加评论成功,目前发表评论需要管理员审核才会显示,请耐心等待管理员审核.', getarticleurl($articleid));
	}

	setcookie('cmcontent','');

	if ($options['comment_order']) { //新评论靠后排序
		if ($options['showmsg']) {
			message('添加评论成功,返回即可看到您所发表的评论.', getarticleurl($articleid, $page).$cmnum);
		} else {
			@header('Location: '.getarticleurl($articleid, $page).$cmnum);
		}
	} else {
		if ($options['showmsg']) {
			message('添加评论成功,返回即可看到您所发表的评论.', getarticleurl($articleid).'#comment');
		} else {
			@header('Location: '.getarticleurl($articleid).'#comment');
		}
	}
}



// 检查用户提交内容合法性
function checkcontent($content) {
	global $options;
    if(empty($content)) {
        $result .= '内容不能为空.<br />';
        return $result;
	}
    if(strlen($content) < $options['comment_min_len']) {
        $result .= '内容不能少于'.$options['comment_min_len'].'字节.<br />';
        return $result;
	}
	if(strlen($content) > $options['comment_max_len']) {
        $result .= '内容不能超过'.$options['comment_max_len'].'字节.<br />';
        return $result;
	}
}
// 检查链接URL是否符合逻辑
function checkurl($url) {
	if($url) {
		if (isemail($url)) {
			return false;
		} else {
			if (!preg_match("#^(http|news|https|ftp|ed2k|rtsp|mms)://#", $url)) {
				$result .= '网站URL错误.<br />';
				return $result;
			}
			$key = array("\\",' ',"'",'"','*',',','<','>',"\r","\t","\n",'(',')','+',';');
			foreach($key as $value){
				if (strpos($url,$value) !== false){ 
					$result .= '网站URL错误.<br />';
					return $result;
					break;
				}
			}
		}
	}
}

?>
