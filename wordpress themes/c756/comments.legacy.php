<?php

// Do not delete these lines
	if (!empty($_SERVER['SCRIPT_FILENAME']) && 'comments.php' == basename($_SERVER['SCRIPT_FILENAME']))
		die (_e('Please do not load this page directly. Thanks!', "c756"));

	if ( $post->post_password ) { ?>
		<p class="nocomments"><?php _e("This post is password protected. Enter the password to view comments.", "c756"); ?></p>
	<?php
		return;
	}
?>

<!-- You can start editing here. -->

<?php if ( $comments ) : ?>
	<h3 id="comments"><span><?php comments_number(__('No comments', "c756"), __('One comment', "c756"), __('% comments', "c756") );?></span> <?php _e("to", "c756"); ?> &#8220;<?php the_title(); ?>&#8221;</h3>

	<ol class="commentlist">

		<?php foreach ($comments as $comment) : ?>

			<li class="comment" id="comment-<?php comment_ID(); ?>">
				<div id="div-comment-<?php comment_ID(); ?>">
					<div class="comment-author">
						<?php echo get_avatar($comment, $size = 85); ?>
						<cite><?php comment_author_link(); ?></cite> <?php _e("says", "c756"); ?>
					</div>
					<div class="comment-meta">
						<a href="#comment-<?php comment_ID() ?>" title=""><?php comment_date('F jS, Y') ?> at <?php comment_time() ?></a> <?php edit_comment_link('edit','&nbsp;&nbsp;',''); ?>
					</div>
				</div>

				<?php comment_text(); ?>
				
				<?php if ($comment->comment_approved == '0') : ?>
					<em>Your comment is awaiting moderation.</em>
				<?php endif; ?>

			</li>

		<?php endforeach; /* end for each comment */ ?>

	</ol>

 <?php else : // this is displayed if there are no comments so far ?>

	<?php if ('open' == $post->comment_status) : ?>
		<!-- If comments are open, but there are no comments. -->

	 <?php else : // comments are closed ?>
		<!-- If comments are closed. -->
		<p class="nocomments"><?php _e("Comments are closed.", "c756"); ?></p>

	<?php endif; ?>
<?php endif; ?>


<?php if ('open' == $post->comment_status) : ?>

<div id="respond">

<h3><?php _e('Leave a Reply', "c756"); ?></h3>


<?php if ( get_option('comment_registration') && !$user_ID ) : ?>
	<?php $loginLink = '<a href="'. get_option('siteurl') .'/wp-login.php?redirect_to='. urlencode(get_permalink()) .'">'.__('logged in', 'c756').'</a>'; ?>
	<p><?php sprintf(__('You must be $1%s to post a comment', "c756"), $loginLink) ?>.</p>
<?php else : ?>

<form action="<?php echo get_option('siteurl'); ?>/wp-comments-post.php" method="post" id="commentform">

<?php if ( $user_ID ) : ?>

<p><?php _e("Logged in as", "c756"); ?> <a href="<?php echo get_option('siteurl'); ?>/wp-admin/profile.php"><?php echo $user_identity; ?></a>. <a href="<?php echo get_option('siteurl'); ?>/wp-login.php?action=logout" title="<?php _e('Log out of this account', 'c756'); ?>"><?php _e("Log out", "c756"); ?> &raquo;</a></p>

<?php else : ?>

<p><input type="text" name="author" id="author" value="<?php echo $comment_author; ?>" size="22" tabindex="1" <?php if ($req) echo "aria-required='true'"; ?> />
<label for="author"><small><?php _e("Name", "c756"); ?> <?php if ($req) echo __("(required)", "c756"); ?></small></label></p>

<p><input type="text" name="email" id="email" value="<?php echo $comment_author_email; ?>" size="22" tabindex="2" <?php if ($req) echo "aria-required='true'"; ?> />
<label for="email"><small><?php _e("Mail (will not be published)", "c756"); ?> <?php if ($req) echo  __("(required)", "c756"); ?></small></label></p>

<p><input type="text" name="url" id="url" value="<?php echo $comment_author_url; ?>" size="22" tabindex="3" />
<label for="url"><small><?php _e("Website", "c756"); ?></small></label></p>

<?php endif; ?>

<!--<p><small><strong>XHTML:</strong> You can use these tags: <code><?php echo allowed_tags(); ?></code></small></p>-->

<p><textarea name="comment" id="comment" width="550px" rows="10" tabindex="4"></textarea></p>

<p><input name="submit" type="submit" id="submit" tabindex="5" value="<?php _e("Submit Comment", "c756"); ?>" />

</p>
<?php do_action('comment_form', $post->ID); ?>

</form>
</div>

<?php endif; // If registration required and not logged in ?>

<?php endif; // if you delete this the sky will fall on your head ?>
