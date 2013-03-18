<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<script type="text/javascript" src="include/ajax.js"></script>
	<h2 class="posttitle">$article[title]</h2>
	<p class="postmeta"> 提交于 $article[dateline]. <a href="./?action=index&amp;cid=$article[cid]">$article[cname]</a> 类</p>
<!--
EOT;
if (!$article['allowread']) {
	print <<<EOT
-->
	<div>
		<form action="./?action=show&amp;id=$article[articleid]" method="post">这篇日志被加密了。请输入密码后查看。<br /><input type="password" name="readpassword" /> <button type="submit">提交</button></form>
	</div>
<!--
EOT;
} else {
	print <<<EOT
-->
	<div class="postdata" >$article[content]</div>

<!--
EOT;
	if ($article['image']) {
		foreach ($article['image'] as $image) {
			if($image[6]){
				print <<<EOT
-->
	<p class="attach">图片附件(缩略图):<br /><a href="attachment.php?id=$image[0]" target="_blank"><img src="$image[1]" border="0" alt="大小: $image[2]&#13;尺寸: $image[3] x $image[4]&#13;浏览: $image[5] 次&#13;点击打开新窗口浏览全图" width="$image[3]" height="$image[4]" /></a></p>
<!--
EOT;
			} else {
				print <<<EOT
-->
	<p class="attach">图片附件:<br /><a href="attachment.php?id=$image[0]" target="_blank"><img src="$image[1]" border="0" alt="大小: $image[2]&#13;尺寸: $image[3] x $image[4]&#13;浏览: $image[5] 次&#13;点击打开新窗口浏览全图" width="$image[3]" height="$image[4]" /></a></p>
<!--
EOT;
			}
		}
	}
	if($article['file']){
		foreach($article['file'] as $file){
			if($file){
				print <<<EOT
-->
	<p class="attach"><strong>附件: </strong><a href="attachment.php?id=$file[0]" target="_blank">$file[1]</a> ($file[2], 下载次数:$file[3])</p>
<!--
EOT;
			}
		}
	}
	print <<<EOT
-->
	<p class="postnav">&laquo; <a href="./?action=show&amp;id=$article[articleid]&amp;goto=previous">上一篇</a> | <a href="./?action=show&amp;id=$article[articleid]&amp;goto=next">下一篇</a> &raquo;</p>
<!--
EOT;
	if ($article['comments']) {
		print <<<EOT
-->
	<p class="posttitle" >评论列表</p>
	<div id="comment">
<!--
EOT;
		foreach($commentdb as $key => $comment){
			print <<<EOT
-->
		<p id="comm_$comment[commentid]"><img style="cursor: hand" src="content/templates/$options[templatename]/img/sns.gif" border="0" alt="引用此文发表评论" /> <font color="#000000">#<strong>$comment[cmtorderid]</strong></font>&nbsp;&nbsp;$comment[content]</p>
		<p class="comdate"> $comment[author] [$comment[dateline]]</p>
<!--
EOT;
		}
		print <<<EOT
-->
	</div>
	$multipage
<!--
EOT;
	}
	if (!$article['closecomment']) {
		print <<<EOT
-->
	<p class="posttitle">发表评论</p>
	<form method="post" name="form" id="form" action="post.php" >
		<input type="hidden" name="articleid" value="$article[articleid]" />
		<input type="hidden" name="formhash" value="$formhash" />	
		<div id="respond">
			<div class="cominfo">
				<input type="text" name="username" class="textfield" value="$_COOKIE[comment_username]" tabindex="1" />
				<label for="username" >昵称 (必填)</label>
			</div>
			<div class="cominfo">
				<input type="text" name="url" class="textfield" value=""  tabindex="3" />
				<label for="url" >网址或电邮</label>
			</div>


			<!-- comment input -->
			<div class="cominfo">
				<textarea name="content" rows="8" cols="50"></textarea>
			</div>

			<!-- comment submit and rss -->
			<div class="cominfo">
<!--
EOT;
		if ($options['seccode'] && $txp_group != 1 && $txp_group !=2) {
			print <<<EOT
-->
				<input name="clientcode" id="clientcode" value="" tabindex="5" size="6" maxlength="6" />&nbsp;<img id="seccode" class="codeimg" src="include/seccode.php" alt="单击图片换张图片" border="0" onclick="this.src='include/seccode.php?update=' + Math.random()" />

<!--
EOT;
		}
		print <<<EOT
-->
				<input type="hidden" name="action" value="addcomment" /><input type="submit" name="submit" value="提交" />
			</div>
		</div>
	</form>
<!--
EOT;
	} else {
		print <<<EOT
-->
	<p align="center"><strong>本文因为某种原因此时不允许访客进行评论</strong></p>
<!--
EOT;
	}
}
?>
