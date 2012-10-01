	<div id="sidebar">
		<div id="blogLogo">
			<h1 id="blogTitle"><a href="<?php echo get_option('home'); ?>"><?php bloginfo('name'); ?></a></h1>
			<div>
				<p><?php bloginfo('description'); ?></p>
			</div>
		</div>
		
		<ul id="widgetBar">
			<?php if ( !function_exists('dynamic_sidebar') || !dynamic_sidebar() ) : ?>
				<p><?php _e("Use the widgets options to add things to this sidebar.", "c756")?></p>
			<?php endif; ?>
		</ul>
	</div>

