<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" <?php language_attributes(); ?>>
	<head profile="http://gmpg.org/xfn/11">
		<meta http-equiv="Content-Type" content="<?php bloginfo('html_type'); ?>; charset=<?php bloginfo('charset'); ?>" />
	
		<title><?php wp_title('&laquo;', true, 'right'); ?> <?php bloginfo('name'); ?></title>
		<link rel="stylesheet" href="<?php bloginfo('stylesheet_url'); ?>" type="text/css" media="screen" />
		<link rel="alternate" type="application/rss+xml" title="<?php bloginfo('name'); ?> RSS Feed" href="<?php bloginfo('rss2_url'); ?>" />
		<link rel="alternate" type="application/atom+xml" title="<?php bloginfo('name'); ?> Atom Feed" href="<?php bloginfo('atom_url'); ?>" />
		<link rel="pingback" href="<?php bloginfo('pingback_url'); ?>" />
		
		<?php if ( is_singular() ) wp_enqueue_script( 'comment-reply' ); ?>
		<?php wp_head(); ?>
		
		<script type="text/javascript">
			<!--
				function submitenter(myfield,e)
				{
					var keycode;
					if (window.event) keycode = window.event.keyCode;
					else if (e) keycode = e.which;
					else return true;
					
					if (keycode == 13)
					{
						myfield.form.submit();
						return false;
					}
					else
						return true;
				}
			-->
		</script>
	</head>
	<body>

		<div id="header">
			<div id="headerContent">
				
				<ul id="blogMenu">
					<li class="<?php echo is_home() ? 'selected' : ''; ?>"><a href="<?php echo get_option('home'); ?>"><?php _e("Home", "c756"); ?></a></li>
					<li class="<?php echo is_archive() || is_single() ? 'selected' : ''; ?>" id="archiveOption"><a id="archiveLink" href="<?php echo get_option('home'); ?>"><?php _e("Archive", 'c756'); ?></a>
						<ul id="archiveDropdown">
							<?php wp_list_categories('sort_column=menu_order&title_li='); ?>
						</ul>
					</li>
					<?php wp_list_pages('sort_column=menu_order&title_li='); ?>
				</ul>
			</div>
			<div style="clear: both;"></div>
		</div>
