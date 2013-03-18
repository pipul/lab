<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
	<h2>Friends</h2>
<!--
EOT;
if ($linkdb) {
	print <<<EOT
-->
	<ul>
<!--
EOT;
	foreach($linkdb as $key => $link){
		print <<<EOT
-->
		<li><a href="$link[url]" target="_blank">$link[name]</a><br />$link[note]</li>
<!--
EOT;
	}
	print <<<EOT
-->
	</ul>
<!--
EOT;
} else {
	print <<<EOT
-->
<p><center><strong>没有任何友情链接</strong></center></p>
<!--
EOT;
}
?>
