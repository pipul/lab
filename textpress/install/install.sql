# 安装用

DROP TABLE IF EXISTS txp_articles;
CREATE TABLE txp_articles (
  articleid mediumint(8) unsigned NOT NULL auto_increment,
  cid smallint(6) unsigned NOT NULL default '0',
  uid mediumint(8) unsigned NOT NULL default '0',
  title varchar(255) NOT NULL default '',
  description text NOT NULL,
  content mediumtext NOT NULL,
  dateline int(10) unsigned NOT NULL default '0',
  views int(10) unsigned NOT NULL default '0',
  comments mediumint(8) unsigned NOT NULL default '0',
  attachments text NOT NULL,
  closecomment tinyint(1) NOT NULL default '0',
  visible tinyint(1) NOT NULL default '1',
  stick tinyint(1) unsigned NOT NULL default '0',
  readpassword varchar(20) NOT NULL default '',
  PRIMARY KEY  (articleid),
  KEY cid (cid),
  KEY uid (uid),
  KEY dateline (dateline),
  KEY visible (visible)
) ENGINE=MyISAM;


DROP TABLE IF EXISTS txp_attachments;
CREATE TABLE txp_attachments (
  attachmentid mediumint(8) unsigned NOT NULL auto_increment,
  articleid int(10) unsigned NOT NULL default '0',
  dateline int(10) unsigned NOT NULL default '0',
  filename varchar(100) NOT NULL default '',
  filetype varchar(50) NOT NULL default '',
  filesize int(10) unsigned NOT NULL default '0',
  downloads mediumint(8) NOT NULL default '0',
  filepath varchar(255) NOT NULL default '',
  thumb_filepath varchar(255) NOT NULL default '',
  thumb_width smallint(6) NOT NULL default '0',
  thumb_height smallint(6) NOT NULL default '0',
  isimage tinyint(1) unsigned NOT NULL default '0',
  PRIMARY KEY  (attachmentid),
  KEY articleid (articleid),
  KEY dateline (dateline)
) ENGINE=MyISAM;


DROP TABLE IF EXISTS txp_categories;
CREATE TABLE txp_categories (
  cid smallint(6) unsigned NOT NULL auto_increment,
  name char(50) NOT NULL default '',
  articles mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (cid)
) ENGINE=MyISAM;


DROP TABLE IF EXISTS txp_comments;
CREATE TABLE txp_comments (
  commentid int(10) unsigned NOT NULL auto_increment,
  articleid mediumint(8) unsigned NOT NULL default '0',
  author varchar(20) NOT NULL default '',
  url varchar(255) NOT NULL default '',
  dateline int(10) unsigned NOT NULL default '0',
  content mediumtext NOT NULL,
  ipaddress varchar(16) NOT NULL default '',
  visible tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (commentid),
  KEY articleid (articleid),
  KEY dateline (dateline),
  KEY ipaddress (ipaddress)
) ENGINE=MyISAM;


DROP TABLE IF EXISTS txp_links;
CREATE TABLE txp_links (
  linkid smallint(6) unsigned NOT NULL auto_increment,
  displayorder tinyint(3) NOT NULL default '0',
  name varchar(100) NOT NULL default '',
  url varchar(200) NOT NULL default '',
  note varchar(200) NOT NULL default '',
  visible tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (linkid),
  KEY displayorder (displayorder)
) ENGINE=MyISAM;

DROP TABLE IF EXISTS txp_settings;
CREATE TABLE txp_settings (
  title varchar(50) NOT NULL default '',
  value text NOT NULL,
  PRIMARY KEY  (title)
) ENGINE=MyISAM;


INSERT INTO txp_settings VALUES ('name', 'yourname''s blog');
INSERT INTO txp_settings VALUES ('url', '');
INSERT INTO txp_settings VALUES ('description', '成功，源于对美学的执着追求......');
INSERT INTO txp_settings VALUES ('viewmode', 'normal');
INSERT INTO txp_settings VALUES ('templatename', 'default');
INSERT INTO txp_settings VALUES ('normal_shownum', '8');
INSERT INTO txp_settings VALUES ('article_order', 'dateline');
INSERT INTO txp_settings VALUES ('title_limit', '0');
INSERT INTO txp_settings VALUES ('related_shownum', '0');
INSERT INTO txp_settings VALUES ('related_title_limit', '0');
INSERT INTO txp_settings VALUES ('related_order', 'dateline');
INSERT INTO txp_settings VALUES ('show_debug', '1');
INSERT INTO txp_settings VALUES ('audit_comment', '0');
INSERT INTO txp_settings VALUES ('seccode', '1');
INSERT INTO txp_settings VALUES ('comment_order', '1');
INSERT INTO txp_settings VALUES ('article_comment_num', '20');
INSERT INTO txp_settings VALUES ('comment_min_len', '4');
INSERT INTO txp_settings VALUES ('comment_max_len', '6000');
INSERT INTO txp_settings VALUES ('comment_post_space', '20');
INSERT INTO txp_settings VALUES ('search_post_space', '30');
INSERT INTO txp_settings VALUES ('search_keywords_min_len', '3');
INSERT INTO txp_settings VALUES ('attachments_dir', 'content/attachments');
INSERT INTO txp_settings VALUES ('attachments_save_dir', '2');
INSERT INTO txp_settings VALUES ('attachments_display', '0');
INSERT INTO txp_settings VALUES ('attachments_thumbs', '1');
INSERT INTO txp_settings VALUES ('attachments_thumbs_size', '500x500');
INSERT INTO txp_settings VALUES ('display_attach', '1');
INSERT INTO txp_settings VALUES ('remote_open', '1');
INSERT INTO txp_settings VALUES ('watermark', '0');
INSERT INTO txp_settings VALUES ('watermark_size', '300x300');
INSERT INTO txp_settings VALUES ('waterpos', '2');
INSERT INTO txp_settings VALUES ('watermarktrans', '100');
INSERT INTO txp_settings VALUES ('pos_padding', '5');
INSERT INTO txp_settings VALUES ('server_timezone', '8');
INSERT INTO txp_settings VALUES ('normaltime', 'Y, F j, g:i A');
INSERT INTO txp_settings VALUES ('comment_timeformat', 'Y, F j, g:i A');
INSERT INTO txp_settings VALUES ('close', '0');
INSERT INTO txp_settings VALUES ('close_note', '系统升级中....');
INSERT INTO txp_settings VALUES ('gzipcompress', '1');
INSERT INTO txp_settings VALUES ('showmsg', '0');

INSERT INTO txp_settings VALUES ('seccode_enable', '1');
INSERT INTO txp_settings VALUES ('artlink_ext', 'html');
INSERT INTO txp_settings VALUES ('title_keywords', '');
INSERT INTO txp_settings VALUES ('meta_keywords', '');
INSERT INTO txp_settings VALUES ('meta_description', '');
INSERT INTO txp_settings VALUES ('banip_enable', '0');
INSERT INTO txp_settings VALUES ('ban_ip', '');
INSERT INTO txp_settings VALUES ('spam_enable', '1');
INSERT INTO txp_settings VALUES ('spam_words', '虚拟主机,域名注册,出租网,六合彩,铃声下载,手机铃声,和弦铃声,手机游戏,免费铃声,彩铃,网站建设,操你妈,rinima,日你妈,αngel,鸡,操,鸡吧,小姐,fuck,胡锦涛,温家宝,胡温,李洪志,法轮,民运,反共,专制,专政,独裁,极权,中共,共产,共党,六四,民主,人权,毛泽东,中国政府,中央政府,游行示威,天安门,达赖,他妈的,我操,强奸,法轮');
INSERT INTO txp_settings VALUES ('spam_url_num', '3');
INSERT INTO txp_settings VALUES ('spam_content_size', '2000');
INSERT INTO txp_settings VALUES ('rss_enable', '1');
INSERT INTO txp_settings VALUES ('rss_num', '20');
INSERT INTO txp_settings VALUES ('rss_ttl', '30');
INSERT INTO txp_settings VALUES ('rewrite_enable', '0');
INSERT INTO txp_settings VALUES ('rewrite_ext', 'html');

DROP TABLE IF EXISTS txp_sessions;
CREATE TABLE txp_sessions (
  hash varchar(20) NOT NULL default '',
  uid mediumint(8) NOT NULL default '0',
  groupid smallint(6) NOT NULL,
  ipaddress varchar(16) NOT NULL default '',
  agent varchar(200) NOT NULL,
  lastactivity int(10) NOT NULL default '0',
  PRIMARY KEY  (hash)
) ENGINE=MyISAM;

DROP TABLE IF EXISTS txp_users;
CREATE TABLE txp_users (
  userid mediumint(8) unsigned NOT NULL auto_increment,
  username varchar(20) NOT NULL default '',
  password varchar(32) NOT NULL default '',
  logincount smallint(6) NOT NULL default '0',
  loginip varchar(16) NOT NULL default '',
  logintime int(10) NOT NULL default '0',
  url varchar(255) NOT NULL,
  articles int(11) NOT NULL default '0',
  regdateline int(10) NOT NULL,
  regip varchar(16) NOT NULL,
  groupid smallint(4) NOT NULL,
  lastpost int(10) NOT NULL,
  PRIMARY KEY  (userid)
) ENGINE=MyISAM;
