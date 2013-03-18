<?php

// 文件说明：附件相关函数库


if(!defined('TXP_ROOT')) {
	exit('Access Denied');
}

$attachs = array();


// 获取附件大小

function sizecount($filesize) {
	if($filesize >= 1073741824) {
		$filesize = round($filesize / 1073741824 * 100) / 100 . ' G';
	} elseif($filesize >= 1048576) {
		$filesize = round($filesize / 1048576 * 100) / 100 . ' M';
	} elseif($filesize >= 1024) {
		$filesize = round($filesize / 1024 * 100) / 100 . ' K';
	} else {
		$filesize = $filesize . ' bytes';
	}
	return $filesize;
}



// 获得文件扩展名

function getextension($filename) {
	$pathinfo = pathinfo($filename);
	return $pathinfo['extension'];
}


// 生成缩略图

function generate_thumbnail($attach_thumb=array()) {
	global $options, $attachsubdir;
	$return = array();
	$image  = '';
	$thumb_file = $attach_thumb['filepath'];
	$remap  = array( 1 => 'gif', 2 => 'jpg', 3 => 'png' );
	if ($attach_thumb['thumbswidth'] && $attach_thumb['thumbsheight'] ) {
		$filesize = @GetImageSize( $thumb_file );
		if ( $filesize[0] < 1 && $filesize[1] < 1 ) {
			$filesize = array();
			$filesize[0] = $attach_thumb['thumbswidth'];
			$filesize[1] = $attach_thumb['thumbsheight'];
			$return['thumbwidth']    = $attach_thumb['thumbswidth'];
			$return['thumbheight']   = $attach_thumb['thumbsheight'];
			$return['thumbfilepath'] = $attach_thumb['filepath'];
			return $return;
		}
		if ( $filesize[0] > $thumb['thumbswidth'] || $filesize[1] > $attach_thumb['thumbsheight'] ) { 
			$im = scale_image( array(
				'max_width'  => $attach_thumb['thumbswidth'],
				'max_height' => $attach_thumb['thumbsheight'],
				'cur_width'  => $filesize[0],
				'cur_height' => $filesize[1]
			));
			$return['thumbwidth']   = $im['img_width'];
			$return['thumbheight']  = $im['img_height'];
			if ( $remap[$filesize[2]] == 'gif' ) {
				if (function_exists('imagecreatefromgif')) {
					if(!($image = @imagecreatefromgif($thumb_file))){
						redirect($attach_thumb['filename'].'文件生成缩略图失败');
					}
					$type = 'gif';
				}
			} else if ($remap[$filesize[2]] == 'png') {
				if (function_exists('imagecreatefrompng')) {
					if(!($image = @imagecreatefrompng($thumb_file))){
						redirect($attach_thumb['filename'].'文件生成缩略图失败');
					}
					$type = 'png';
				}
			} else if ($remap[$filesize[2]] == 'jpg') {
				if (function_exists('imagecreatefromjpeg')) {
					if(!($image = @imagecreatefromjpeg($thumb_file))){
						redirect($attach_thumb['filename'].'文件生成缩略图失败');
					}
					$type = 'jpg';
				}
			}
			if ( $image ) {
				if (function_exists('imagecreatetruecolor')) {
					$thumb = @imagecreatetruecolor($im['img_width'], $im['img_height']);
					@imagecopyresampled($thumb, $image, 0, 0, 0, 0, $im['img_width'], $im['img_height'], $filesize[0], $filesize[1]);
				} else {
					$thumb = @imagecreate($im['img_width'], $im['img_height']);
					@imagecopyresized($thumb, $image, 0, 0, 0, 0, $im['img_width'], $im['img_height'], $filesize[0], $filesize[1]);
				}
				if (PHP_VERSION != '4.3.2') {
					UnsharpMask($thumb);
				}
				if ( ! $thumb_filename ) {
					$thumb_filename = 'thumb_'.$attach_thumb['filename'];
				}
				$file_extension = $attach_thumb['extension'];
				if ($file_extension == 'jpg' && function_exists('imagejpeg')) {
					@imagejpeg( $thumb, '../'.$options['attachments_dir'].$attach_thumb['attachsubdir'].$thumb_filename.'.jpg' );
					@imagedestroy( $thumb );
				} else if ($file_extension == 'png' && function_exists('imagepng'))	{
					@imagepng( $thumb, '../'.$options['attachments_dir'].$attach_thumb['attachsubdir'].$thumb_filename.'.png' );
					@imagedestroy( $thumb );
				} else if ($file_extension == 'gif' && function_exists('imagegif'))	{
					@imagegif( $thumb, '../'.$options['attachments_dir'].$attach_thumb['attachsubdir'].$thumb_filename.'.gif' );
					@imagedestroy( $thumb );
				} else {
					$return['thumbfilepath'] = $thumb['filepath'];
					return $return;
				}
				$return['thumbfilepath'] = $thumb_filename.'.'.$file_extension;
				return $return;
			} else {
				$return['thumbwidth']    = $im['img_width'];
				$return['thumbheight']   = $im['img_height'];
				$return['thumbfilepath'] = $attach_thumb['filepath'];
				return $return;
			}
		} else { 
			$return['thumbwidth']    = $filesize[0];
			$return['thumbheight']   = $filesize[1];
			$return['thumbfilepath'] = $attach_thumb['filepath'];
			return $return;
		}
	}
}


// 计算缩略图的大小

function scale_image($arg) {

	$ret = array('img_width' => $arg['cur_width'], 'img_height' => $arg['cur_height']);

	if ( $arg['cur_width'] > $arg['max_width'] ) {

		$ret['img_width']  = $arg['max_width'];

		$ret['img_height'] = ceil( ( $arg['cur_height'] * ( ( $arg['max_width'] * 100 ) / $arg['cur_width'] ) ) / 100 );

		$arg['cur_height'] = $ret['img_height'];

		$arg['cur_width']  = $ret['img_width'];

	}

	if ( $arg['cur_height'] > $arg['max_height'] ) {

		$ret['img_height']  = $arg['max_height'];

		$ret['img_width']   = ceil( ( $arg['cur_width'] * ( ( $arg['max_height'] * 100 ) / $arg['cur_height'] ) ) / 100 );

	}

	return $ret;

}



function UnsharpMask($img, $amount = 100, $radius = .5, $threshold = 3) {

	$amount = min($amount, 500);

	$amount = $amount * 0.016;

	if ($amount == 0) return true;

	$radius = min($radius, 50);

	$radius = $radius * 2;

	$threshold = min($threshold, 255);

	$radius = abs(round($radius));

	if ($radius == 0) return true;

	$w = ImageSX($img);

	$h = ImageSY($img);

	$imgCanvas  = ImageCreateTrueColor($w, $h);

	$imgCanvas2 = ImageCreateTrueColor($w, $h);

	$imgBlur    = ImageCreateTrueColor($w, $h);

	$imgBlur2   = ImageCreateTrueColor($w, $h);

	ImageCopy($imgCanvas,  $img, 0, 0, 0, 0, $w, $h);

	ImageCopy($imgCanvas2, $img, 0, 0, 0, 0, $w, $h);

	for ($i = 0; $i < $radius; $i++) {

		ImageCopy($imgBlur, $imgCanvas, 0, 0, 1, 1, $w - 1, $h - 1);

		ImageCopyMerge($imgBlur, $imgCanvas, 1, 1, 0, 0, $w, $h, 50);

		ImageCopyMerge($imgBlur, $imgCanvas, 0, 1, 1, 0, $w - 1, $h, 33.33333);

		ImageCopyMerge($imgBlur, $imgCanvas, 1, 0, 0, 1, $w, $h - 1, 25);

		ImageCopyMerge($imgBlur, $imgCanvas, 0, 0, 1, 0, $w - 1, $h, 33.33333);

		ImageCopyMerge($imgBlur, $imgCanvas, 1, 0, 0, 0, $w, $h, 25);

		ImageCopyMerge($imgBlur, $imgCanvas, 0, 0, 0, 1, $w, $h - 1, 20 );

		ImageCopyMerge($imgBlur, $imgCanvas, 0, 1, 0, 0, $w, $h, 16.666667); // dow

		ImageCopyMerge($imgBlur, $imgCanvas, 0, 0, 0, 0, $w, $h, 50);

		ImageCopy($imgCanvas, $imgBlur, 0, 0, 0, 0, $w, $h);

		ImageCopy($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h);

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 50);

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 33.33333);

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 25);

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 33.33333);

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 25);

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 20 );

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 16.666667);

		ImageCopyMerge($imgBlur2, $imgCanvas2, 0, 0, 0, 0, $w, $h, 50);

		ImageCopy($imgCanvas2, $imgBlur2, 0, 0, 0, 0, $w, $h);

	}

	for ($x = 0; $x < $w; $x++)	{

		for ($y = 0; $y < $h; $y++)	{

			$rgbOrig = ImageColorAt($imgCanvas2, $x, $y);

			$rOrig = (($rgbOrig >> 16) & 0xFF);

			$gOrig = (($rgbOrig >>  8) & 0xFF);

			$bOrig =  ($rgbOrig        & 0xFF);

			$rgbBlur = ImageColorAt($imgCanvas, $x, $y);

			$rBlur = (($rgbBlur >> 16) & 0xFF);

			$gBlur = (($rgbBlur >>  8) & 0xFF);

			$bBlur =  ($rgbBlur        & 0xFF);

			$rNew = (abs($rOrig - $rBlur) >= $threshold) ? max(0, min(255, ($amount * ($rOrig - $rBlur)) + $rOrig)) : $rOrig;

			$gNew = (abs($gOrig - $gBlur) >= $threshold) ? max(0, min(255, ($amount * ($gOrig - $gBlur)) + $gOrig)) : $gOrig;

			$bNew = (abs($bOrig - $bBlur) >= $threshold) ? max(0, min(255, ($amount * ($bOrig - $bBlur)) + $bOrig)) : $bOrig;

			if (($rOrig != $rNew) || ($gOrig != $gNew) || ($bOrig != $bNew)) {

				$pixCol = ImageColorAllocate($img, $rNew, $gNew, $bNew);

				ImageSetPixel($img, $x, $y, $pixCol);

			}

		}

	}

	ImageDestroy($imgCanvas);

	ImageDestroy($imgCanvas2);

	ImageDestroy($imgBlur);

	ImageDestroy($imgBlur2);

	return true;

}



// 删除附件函数
function removeattachment($attacharr) {
	global $DB, $db_prefix, $options;
	$attachids = 0;
	$attachnum = count($attacharr);
	if ($attacharr && $attachnum) {
		$filepath = '../'.$options['attachments_dir'];
		foreach ($attacharr as $attachid => $attach) {
			$attachids .= ','.intval($attachid);
			@chmod ($filepath.$attach['filepath'], 0777);
			@unlink($filepath.$attach['filepath']);
			if ($attach['thumb_filepath']) {
				@chmod ($filepath.$attach['thumb_filepath'], 0777);
				@unlink($filepath.$attach['thumb_filepath']);
			}
		}

		$DB->unbuffered_query("DELETE FROM {$db_prefix}attachments WHERE attachmentid IN ($attachids)");
	}
}

?>
