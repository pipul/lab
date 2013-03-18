<?php

// 文件说明：附件上传


if(!defined('TXP_ROOT') || !isset($php_self) || !preg_match("/[\/\\\\]admincp\.php$/", $php_self)) {
	exit('Access Denied');
}

// 创建水印
function create_watermark($uploadfile) {
	global $options;

	$waterimg = '../content/templates/'.$options['templatename'].'/img/watermark.png';
    if (file_exists($waterimg)) {
		$upload_info = @getimagesize($uploadfile);
		if (!$upload_info[0] || !$upload_info[1]) return;
		switch ($upload_info['mime']) {
			case 'image/jpeg':
				$tmp = @imagecreatefromjpeg($uploadfile);
				break;
			case 'image/gif':
				$tmp = @imagecreatefromgif($uploadfile);
				break;
			case 'image/png':
				$tmp = @imagecreatefrompng($uploadfile);
				break;
			default :
				return;
		}
		$marksize = @getimagesize($waterimg);
		$width    = $marksize[0];
		$height   = $marksize[1];
		unset($marksize);
		$pos_padding = ($options['pos_padding'] && $options['pos_padding'] > 0) ? $options['pos_padding'] : 5; //水印边距
		switch ($options['waterpos']) {
			// 左上
			case '1':
				$pos_x = $pos_padding;
				$pos_y = $pos_padding;
				break;
			// 左下
			case '2':
				$pos_x = $pos_padding;
				$pos_y = $upload_info[1] - $height - $pos_padding;
				break;
			// 右上
			case '3':
				$pos_x = $upload_info[0] - $width - $pos_padding;
				$pos_y = $pos_padding;
				break;
			// 右下
			case '4':
				$pos_x = $upload_info[0] - $width - $pos_padding;
				$pos_y = $upload_info[1] - $height - $pos_padding;
				break;
			// 中间
			case '5':
				$pos_x = ($upload_info[0] - $width) / 2;
				$pos_y = ($upload_info[1] - $height) / 2;
				break;
			// 随机
			default:
				$pos_x = rand(0,($upload_info[0] - $width));
				$pos_y = rand(0,($upload_info[1] - $height));
				break;
		}
		if($imgmark = @imagecreatefrompng($waterimg)) {
			if ($upload_info[0] < ($width * 2) || $upload_info[1] < ($height * 2)) {
				return;
				//如果水印占了原图一半就不搞水印了.影响浏览.抵制影响正常浏览的广告.
			}
			if ($options['watermarktrans']) {
				@imagecopymerge($tmp, $imgmark, $pos_x, $pos_y, 0, 0, $width, $height, $options['watermarktrans']);
			} else {
				@imagecopy($tmp, $imgmark, $pos_x, $pos_y, 0, 0, $width, $height);
			}
		}
		switch ($upload_info['mime']) {
			case 'image/jpeg':
				@imagejpeg($tmp,$uploadfile,100);
				@imagedestroy($tmp);
				break;
			case 'image/gif':
				@imagegif($tmp,$uploadfile);
				@imagedestroy($tmp);
				break;
			case 'image/png':
				@imagepng($tmp,$uploadfile);
				@imagedestroy($tmp);
				break;
			default :
				return;
		}
    }
}

$attachments = $attachs = $attach_data = array();

if(isset($_FILES['attach']) && is_array($_FILES['attach'])) {
	foreach($_FILES['attach'] as $key => $var) {
		foreach($var as $id => $val) {
			$attachments[$id][$key] = $val;
		}
	}
}

if($attachments) {
	$gd_version = gd_version();
	foreach($attachments as $key => $attach) {
		if(!disuploadedfile($attach['tmp_name']) || !($attach['tmp_name'] != 'none' && $attach['tmp_name'] && $attach['name'])) {
			continue;
		}

		$attach['name'] = strtolower($attach['name']);
		$attach['ext']  = getextension($attach['name']);

		$fnamehash = md5(uniqid(microtime()));
		// 判断上传目录的方式
		switch($options['attachments_save_dir']) {
			case 0: $attachsubdir = '/'; break; // 全部放一起
			case 1: $attachsubdir = '/cate_'.$cid.'/'; break; // 按分类放
			case 2: $attachsubdir = '/date_'.sadate('Ym').'/'; break; // 按月放
			case 3: $attachsubdir = '/ext_'.$attach['ext'].'/'; break; // 按文件类型
		}
		// 取得附件目录的绝对路径
		$attach_dir = TXP_ROOT.$options['attachments_dir'].$attachsubdir;
		if(!is_dir($attach_dir)) {
			mkdir($attach_dir, 0777);
			fclose(fopen($attach_dir.'/index.htm', 'w'));
			@chmod($attach_dir, 0777);
		}
		// 判断上传的类型
		// path变量为管理目录相对路径,后台操作用
		// filepath变量为跟目录相对路径,前台读取用
		// fnamehash变量为当前时间的MD5散列,重命名附件名
		if (!in_array($attach['ext'], array('gif', 'jpg', 'jpeg', 'png'))) {
			$path     = $attach_dir.$fnamehash.'.file';
			$filepath = $attachsubdir.$fnamehash.'.file';
		} else {
			$path     = $attach_dir.$fnamehash.'.'.$attach['ext'];
			$filepath = $attachsubdir.$fnamehash.'.'.$attach['ext'];
		}
		$attachment = upfile($attach['tmp_name'], $path);
		// 如果一种函数上传失败，还可以用其他函数上传
		if (!$attachment) {
			redirect('上传附件发生意外错误!');
		}

		$tmp_filesize = @filesize($attachment);
		if ($tmp_filesize != $attach['size']) {
			@unlink($attachment);
			redirect('上传附件发生意外错误!');
		}
		// 判断是否为图片格式
		if (in_array($attach['ext'], array('gif', 'jpg', 'jpeg', 'png'))) {
			if ($imginfo=@getimagesize($attachment)) {
				if (!$imginfo[2] || !$imginfo['bits']) {
					@unlink($attachment);
					redirect('上传的文件不是一个有效的GIF或者JPG文件!');
				} else {
					$isimage = '1';
				}
			}
			// 判断是否使用缩略图
			if ($options['attachments_thumbs'] && $gd_version) {
				$size = explode('x', strtolower($options['attachments_thumbs_size']));
				if (($imginfo[0] > $size[0] || $imginfo[1] > $size[1]) && $attach['size'] < 2048000) {
					$attach_thumb = array(
						'filepath'     => $attachment,
						'filename'     => $fnamehash,
						'extension'    => $attach['ext'],
						'attachsubdir' => $attachsubdir,
						'thumbswidth'  => $size[0],
						'thumbsheight' => $size[1],
					);
					$thumb_data = generate_thumbnail($attach_thumb);
					$attach_data['thumbwidth']    = $thumb_data['thumbwidth'];
					$attach_data['thumbheight']   = $thumb_data['thumbheight'];
					$attach_data['thumbfilepath'] = $attachsubdir.$thumb_data['thumbfilepath'];
				}
			}

			// 水印
			$watermark_size = explode('x', strtolower($options['watermark_size']));
			if($isimage && $options['watermark'] && $imginfo[0] > $watermark_size[0] && $imginfo[1] > $watermark_size[1] && $attach['size'] < 2048000) {
				create_watermark($path);
				$attach['size'] = filesize($path);
			}
		}

		// 把文件信息插入数据库
		$DB->query("INSERT INTO {$db_prefix}attachments (filename,filesize,filetype,filepath,dateline,downloads,isimage,thumb_filepath,thumb_width,thumb_height) VALUES ('".addslashes($attach['name'])."', '".$attach['size']."', '".addslashes($attach['type'])."', '".addslashes($filepath)."', '$timestamp', '0', '$isimage', '".$attach_data['thumbfilepath']."', '".$attach_data['thumbwidth']."','".$attach_data['thumbheight']."')");
		$aidtmp = $DB->insert_id();
		$attachs[$aidtmp] = array(
			'attachmentid' => $aidtmp,
			'dateline' => $timestamp,
			'filename' => addslashes($attach['name']),
			'filetype' => addslashes($attach['type']),
			'filepath' => addslashes($filepath),
			'filesize' => addslashes($attach['size']),
			'downloads' => 0,
			'thumb_filepath' => $attach_data['thumbfilepath'],
			'thumb_width' => $attach_data['thumbwidth'],
			'thumb_height' => $attach_data['thumbheight'],
			'isimage' => $isimage
		);
		unset($isimage);
		unset($attach_data);

		$searcharray[] = '[localfile='.$key.']';
		$replacearray[] = '[attach='.$aidtmp.']';
	}
}

$attachment_count = count($attachs);
$attachmentids = 0;
foreach($attachs as $key => $value){
	$attachmentids .= ','.$key;
}
$attachs = $attachs ? addslashes(serialize($attachs)) : '';

?>
