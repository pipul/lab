<!--<?php
if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}
print <<<EOT
-->
<h2>Archives</h2>
<form method="post" action="index.php" >
	<input type="hidden" name="formhash" value="$formhash" />
	<input type="hidden" name="action" value="archives" />

	<table cellpadding="0" cellspacing="0" style="margin:10px auto; width:100%;">
		<tr>
			<td id="search_advance" style="display:block">
				<select name="dateline">
					<option value="" selected="selected">任何时间</option>
<!--
EOT;
foreach($archivedb as $y => $arr){
	if (is_array($arr)) {
		foreach($arr as $m => $num){
			print <<<EOT
-->
	<option value="{$y}{$m}">{$y}-{$m}</option>
<!--
EOT;
		}
	}
}
print <<<EOT
-->
				</select>
				<select name="cid[]">
					<option value="" selected="selected">任何分类</option>
<!--
EOT;
if($catedb){
	foreach($catedb AS $data){
		print <<<EOT
-->
					<option value="$data[cid]">$data[name]</option>
<!--
EOT;
	}
}
print <<<EOT
-->
				</select>
			</td>
			<td>
				<input name="keywords" type="text" value="请输入关键字" onmouseover="this.style.borderColor='#FF6600'" onmouseout="this.style.borderColor=''" onFocus="if (value =='请输入关键字'){value =''}" onBlur="if (value ==''){value='请输入关键字'}" style="color:#888;" />
			</td>
			<td>
				<input type="submit" value="搜索" />
			</td>

		</tr>
	</table>
</form>

<!--
EOT;

foreach($archivedb as $y => $arr){
	print <<<EOT
-->
<ul class="linkover">
	<p>{$y}年</p>
<!--
EOT;
	if (is_array($arr)) {
		foreach($arr as $m => $num){
			print <<<EOT
-->
	<li><a href="./?action=index&amp;setdate={$y}{$m}">{$m}月</a> ($num)</li>
<!--
EOT;
		}
	}
	print <<<EOT
-->
</ul>
<!--
EOT;
}
?>
