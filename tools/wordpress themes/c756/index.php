	<?php get_header(); ?>

	<div id="content">

		<?php get_sidebar(); ?>

		<div id="mainContent">
		
		<?php if(is_search()) : ?>
		<div id="contextualInfo">
			<p><?php echo sprintf(__("Search results for <span>%s</span>", "mandm"), get_search_query()); ?></p>
		</div>
		<?php endif; ?>

		<?php if(is_tag()) : ?>
		<div id="contextualInfo">
			<p><?php echo sprintf(__("Posts tagged with <span>%s</span>", "mandm"), single_tag_title("", false)); ?></p>
		</div>
		<?php endif; ?>

		<?php if(is_category()) : ?>
		<div id="contextualInfo">
			<?php $categories = get_the_category(); ?>
			<p><?php echo sprintf(__("Posts archived in <span>%s</span>", "mandm"), $categories[0]->cat_name); ?></p>
		</div>
		<?php endif; ?>

		<?php if(is_year()) : ?>
		<div id="contextualInfo">
			<p><?php echo sprintf(__("Posts published during <span>%s</span>", "mandm"), get_the_time('Y')); ?></p>
		</div>
		<?php endif; ?>

		<?php if(is_month()) : ?>
		<div id="contextualInfo">
			<p><?php echo sprintf(__("Posts published during <span>%s</span>", "mandm"), get_the_time('F, Y')); ?></p>
		</div>
		<?php endif; ?>

		<?php if(is_day()) : ?>
		<div id="contextualInfo">
	
			<p><?php echo sprintf(__("Posts published at <span>%s</span>", "mandm"), get_the_time('m/d/Y')); ?></p>
		</div>
		<?php endif; ?>
			
		<?php if (have_posts()) : ?>
	
			<?php while (have_posts()) : the_post(); ?>
	
				<div <?php if(function_exists('post_class')) post_class(); ?> id="post-<?php the_ID(); ?>">
					<div class="postTitle">
						<a href="<?php echo is_single() ? '#comments' : comments_link(); ?>" class="commentCount"><span class="commentCountNumber"><?php echo comments_number('0', '1', '%'); ?></span></a>
						<p class="postDate" <?php if(is_page()) : ?>style="visibility:hidden;"<?php endif; ?>><?php the_date() ?></p>
						<h2><a href="<?php the_permalink() ?>" rel="bookmark" title="<?php echo sprintf(__('Permanent Link to %s', 'mandm'), the_title_attribute('echo=0')); ?>"><?php the_title(); ?></a></h2>
					</div>
	
					<div class="entry">
						<?php the_content(__('Read the rest of this entry &raquo;', 'mandm')); ?>
					</div>
					<?php edit_post_link(__('Edit', 'mandm')); ?>
					<?php if(!is_page() && !is_attachment() && ( (function_exists('is_singular') && is_singular()) || is_single() )) : ?>
					
						<?php wp_link_pages(array('before' => __('<p><strong>Pages:</strong> ', 'mandm'), 'after' => '</p>', 'next_or_number' => 'number')); ?>
						
						<div class="postmetadata">
							<h3 class="postmetadata_title"><?php _e('Post Info', 'mandm'); ?></h3>
							<p class="postmetadata_category"><?php _e('Category:', 'mandm'); ?> <?php the_category(', ') ?></p>
							<p class="postmetadata_tags"><?php _e('Tags:', 'mandm'); ?> <?php the_tags('', ', ', '.'); ?></p>
							<?php //if(function_exists('the_ratings')) the_ratings(); ?>
						</div>
					<?php endif; ?>
					<div class="commentsWrapper">
					<?php comments_template('', true); ?>
					</div>
					
				</div>
	
			<?php endwhile; ?>
			
			<div class="navigation">
				<div class="alignleft"><?php next_posts_link(__('Older Entries', 'mandm')) ?></div>
				<div class="alignright"><?php previous_posts_link(__('Newer Entries', 'mandm')) ?></div>
			</div>
	
		<?php else : ?>
	
			<h2 class="center"><?php _e("Not Found", "mandm"); ?></h2>
			<p class="center"><?php _e("Sorry, but you are looking for something that isn't here.", "mandm"); ?></p>
			<?php get_search_form(); ?>
	
		<?php endif; ?>
	
		</div><!-- main content -->

	</div> <!-- content -->

	<?php get_footer(); ?>  
