<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
	<h2>Articles</h2>
	<table class="content" border="0" cellspacing="0" cellpadding="0">
<!--
EOT;
foreach($articledb as $key => $article){
	print <<<EOT
-->
		<tr>
			<td class="title"><a href="./?action=show&amp;id=$article[articleid]"><img src="content/templates/$options[templatename]/img/post.gif">
<!--
EOT;
	if($article['stick']){
		print <<<EOT
-->
<img src="http://www-cs-faculty.stanford.edu/~knuth/new.gif">
<!--
EOT;
	}
	print <<<EOT
-->
		$article[title]</a></td>
			<td class="time">$article[dateline]</td>
		</tr>
<!--
EOT;
}
print <<<EOT
-->
	</table>
	$multipage
<!--
EOT;
?>
