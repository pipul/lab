<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<div>
	<form action="admincp.php?job=configurate" method="post">
		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">基本设置</td>
			</tr>
					
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">博客名称:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[name]" maxlength="50" value="$settings[name]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">博客地址:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[url]" maxlength="50" value="$settings[url]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">博客描述:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[description]" maxlength="255" value="$settings[description]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">显示设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">标准模式每页显示文章的数量:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[normal_shownum]" maxlength="50" value="$settings[normal_shownum]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">文章排列依据:</td>
				<td style="width:40%; text-align:left;"><select name="setting[article_order]">
					<option value="articleid" $article_order[articleid]>按创建时间</option>
					<option value="dateline" $article_order[dateline]>按修改时间</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">文章列表标题截取字节数:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[title_limit]" maxlength="50" value="$settings[title_limit]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">评论设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">评论需要审核:</td>
				<td style="width:40%; text-align:left;"><select name="setting[audit_comment]">
					<option value="1" $audit_comment_Y>是</option>
					<option value="0" $audit_comment_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">评论启用验证码:</td>
				<td style="width:40%; text-align:left;"><select name="setting[seccode]">
					<option value="1" $seccode_Y>是</option>
					<option value="0" $seccode_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">新评论排列顺序:</td>
				<td style="width:40%; text-align:left;"><select name="setting[comment_order]">
					<option value="1" $comment_order_Y>靠后</option>
					<option value="0" $comment_order_N>靠前</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">单篇文章显示评论数:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[article_comment_num]" maxlength="50" value="$settings[article_comment_num]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">评论内容的最少字节数:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[comment_min_len]" maxlength="50" value="$settings[comment_min_len]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">评论内容允许最大字数:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[comment_max_len]" maxlength="50" value="$settings[comment_max_len]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">提交评论时间间隔:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[comment_post_space]" maxlength="50" value="$settings[comment_post_space]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">搜索设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">搜索间隔:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[search_post_space]" maxlength="50" value="$settings[search_post_space]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">搜索关键字的最少字节数:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[search_keywords_min_len]" maxlength="50" value="$settings[search_keywords_min_len]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">附件设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">附件存放目录:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[attachments_dir]" maxlength="50" value="$settings[attachments_dir]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">附件存放方式:</td>
				<td style="width:40%; text-align:left;"><select name="setting[attachments_save_dir]">
					<option value="0" $attachments_save_dir[0]>全部存放同一目录</option>
					<option value="1" $attachments_save_dir[1]>按分类存放</option>
					<option value="2" $attachments_save_dir[2]>按月份存放</option>
					<option value="3" $attachments_save_dir[3]>按文件类型存放</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">文章列表的附件显示方式:</td>
				<td style="width:40%; text-align:left;"><select name="setting[attachments_display]">
					<option value="0" $attachments_display[0]>显示附件</option>
					<option value="1" $attachments_display[1]>提示附件</option>
					<option value="2" $attachments_display[2]>隐藏附件</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">图片附件生成缩略图:</td>
				<td style="width:40%; text-align:left;"><select name="setting[attachments_thumbs]">
					<option value="1" $attachments_thumbs_Y>是</option>
					<option value="0" $attachments_thumbs_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">附件图片缩略图大小:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[attachments_thumbs_size]" maxlength="50" value="$settings[attachments_thumbs_size]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">图片附件是否允许直接显示:</td>
				<td style="width:40%; text-align:left;"><select name="setting[display_attach]">
					<option value="1" $display_attach_Y>是</option>
					<option value="0" $display_attach_N>否</option>
				  </select>
				</td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">附件是否禁止从其他站查看:</td>
				<td style="width:40%; text-align:left;"><select name="setting[remote_open]">
					<option value="1" $remote_open_Y>是</option>
					<option value="0" $remote_open_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">水印设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">是否使用图片水印功能:</td>
				<td style="width:40%; text-align:left;"><select name="setting[watermark]">
					<option value="1" $watermark_Y>是</option>
					<option value="0" $watermark_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">添加水印的图片大小控制:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[watermark_size]" maxlength="50" value="$settings[watermark_size]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">水印位置:</td>
				<td style="width:40%; text-align:left;"><select name="setting[waterpos]">
					<option value="1" $waterpos[1]>左上</option>
					<option value="2" $waterpos[2]>左下</option>
					<option value="3" $waterpos[3]>右上</option>
					<option value="4" $waterpos[4]>右下</option>
					<option value="5" $waterpos[5]>中间</option>
					<option value="6" $waterpos[6]>随机</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">水印透明度:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[watermarktrans]" maxlength="50" value="$settings[watermarktrans]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">水印边距:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[pos_padding]" maxlength="50" value="$settings[pos_padding]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">时间设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">服务器所在时区:</td>
				<td style="width:40%; text-align:left;"><select name="setting[server_timezone]">
					<option value="-12" $zone_012>(GMT-12:00) 日界线西</option>
					<option value="-11" $zone_011>(GMT-11:00) 中途岛</option>
					<option value="-10" $zone_010>(GMT-10:00) 夏威夷</option>
					<option value="-9" $zone_09>(GMT-9:00) 阿拉斯加</option>
					<option value="-8" $zone_08>(GMT-8:00) 太平洋时间</option>
					<option value="-7" $zone_07>(GMT-7:00) 山地时间</option>
					<option value="-6" $zone_06>(GMT-6:00) 中部时间</option>
					<option value="-5" $zone_05>(GMT-5:00) 东部时间</option>
					<option value="-4" $zone_04>(GMT-4:00) 大西洋时间</option>
					<option value="-3.5" $zone_03_5>(GMT-3:30) 纽芬兰</option>
					<option value="-3" $zone_03>(GMT-3:00) 巴西</option>
					<option value="-2" $zone_02>(GMT-2:00) 中大西洋</option>
					<option value="-1" $zone_01>(GMT-1:00) 亚速尔群岛</option>
					<option value="111" $zone_111>(GMT+0:00) 伦敦</option>
					<option value="1" $zone_1>(GMT+1:00) 中欧时间</option>
					<option value="2" $zone_2>(GMT+2:00) 开罗</option>
					<option value="3" $zone_3>(GMT+3:00) 莫斯科</option>
					<option value="3.5" $zone_3_5>(GMT+3:30) 德黑兰</option>
					<option value="4" $zone_4>(GMT+4:00) 阿布扎比</option>
					<option value="4.5" $zone_4_5>(GMT+4:30) 喀布尔</option>
					<option value="5" $zone_5>(GMT+5:00) 叶卡捷琳堡</option>
					<option value="5.5" $zone_5_5>(GMT+5:30) 孟买</option>
					<option value="6" $zone_6>(GMT+6:00) 阿拉木图</option>
					<option value="7" $zone_7>(GMT+7:00) 曼谷</option>
					<option value="8" $zone_8>(GMT+8:00) 北京</option>
					<option value="9" $zone_9>(GMT+9:00) 东京</option>
					<option value="9.5" $zone_9_5>(GMT+9:30) 阿德莱德</option>
					<option value="10" $zone_10>(GMT+10:00) 悉尼</option>
					<option value="11" $zone_11>(GMT+11:00) 马加丹</option>
					<option value="12" $zone_12>(GMT+12:00) 奥克兰</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">标准模式文章的日期格式:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[normaltime]" maxlength="50" value="$settings[normaltime]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">列表模式文章的日期格式:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[listtime]" maxlength="50" value="$settings[listtime]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">评论内容的日期格式:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[comment_timeformat]" maxlength="50" value="$settings[comment_timeformat]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">功能设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">关闭博客:</td>
				<td style="width:40%; text-align:left;"><select name="setting[close]">
					<option value="1" $close_Y>是</option>
					<option value="0" $close_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">关闭的原因:</td>
				<td style="width:40%; text-align:left;"><input id="close_note" type="text" name="setting[close_note]" value="$settings[close_note]" /></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">页面Gzip压缩:</td>
				<td style="width:40%; text-align:left;"><select name="setting[gzipcompress]">
					<option value="1" $gzipcompress_Y>是</option>
					<option value="0" $gzipcompress_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">显示信息提示页面:</td>
				<td style="width:40%; text-align:left;"><select name="setting[showmsg]">
					<option value="1" $showmsg_Y>是</option>
					<option value="0" $showmsg_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>

		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">伪静态</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">伪静态开启:</td>
				<td style="width:40%; text-align:left;"><select name="setting[rewrite_enable]">
					<option value="1" $rewrite_enable_Y>是</option>
					<option value="0" $rewrite_enable_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">伪静态扩展名:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[artlink_ext]" value="$settings[artlink_ext]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">标题附加字:</td>
				<td style="width:40%; text-align:left;"><input id="title_keywords" type="text" name="setting[title_keywords]" value="$settings[title_keywords]" /></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">Meta Keywords:</td>
				<td style="width:40%; text-align:left;"><input id="meta_keywords" type="text" name="setting[meta_keywords]" value="$settings[meta_keywords]" /></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">Meta Description:</td>
				<td style="width:40%; text-align:left;"><input id="meta_description" type="text" name="setting[meta_description]" value="$settings[meta_description]" /></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>

		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">限制设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">开启IP禁止功能:</td>
				<td style="width:40%; text-align:left;"><select name="setting[banip_enable]">
					<option value="1" $banip_enable_Y>是</option>
					<option value="0" $banip_enable_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">禁止IP:</td>
				<td style="width:40%; text-align:left;"><input id="ban_ip" type="text" name="setting[ban_ip]" value="$settings[ban_ip]" /></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">开启Spam机制:</td>
				<td style="width:40%; text-align:left;"><select name="setting[spam_enable]">
					<option value="1" $spam_enable_Y>是</option>
					<option value="0" $spam_enable_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">垃圾词语特征:</td>
				<td style="width:40%; text-align:left;"><input id="spam_words" type="text" name="setting[spam_words]" value="$settings[spam_words]" /></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">评论中允许出现的链接次数:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[spam_url_num]" maxlength="50" value="$settings[spam_url_num]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">评论内容字节数:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[spam_content_size]" maxlength="50" value="$settings[spam_content_size]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>

		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr class="tb_header">
				  <td colspan="3">RSS订阅设置</td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">开启RSS订阅功能:</td>
				<td style="width:40%; text-align:left;"><select name="setting[rss_enable]">
					<option value="1" $rss_enable_Y>是</option>
					<option value="0" $rss_enable_N>否</option>
				</select></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">RSS 订阅文章数量:</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[rss_num]" maxlength="50" value="$settings[rss_num]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
			<tr class="tb_row">
				<td style="width:50%; text-align:right;">RSS TTL(分钟):</td>
				<td style="width:40%; text-align:left;"><input type="text" name="setting[rss_ttl]" maxlength="50" value="$settings[rss_ttl]"></td>
				<td style="vertical-align: middle;"><a target="_blank" href="" onclick="popWin(this.href); return false;" class="pophelp">?</a></td>
			</tr>
		</table>


		<table class="cp_config" cellpadding="0" cellspacing="0" border="0">
			<tr align="center">
				<td><input type="submit" value="提交"></td>
				<td><input type="reset" value="重置"></td>
			</tr>
		</table>
		<input type="hidden" name="action" value="updatesetting" />
	</form>
</div>
<!--
EOT;
?>
-->
