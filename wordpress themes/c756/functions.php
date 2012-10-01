<?php

// Register the dynamic sidebar
if ( function_exists('register_sidebar') )
    register_sidebar(array(
        'before_widget' => '<li id="%1$s" class="widget %2$s">',
        'after_widget' => '</li>',
        'before_title' => '<h2 class="widgettitle">',
        'after_title' => '</h2>',
    ));

// Ensure backwards compatibility with old comments system
add_filter('comments_template', 'legacy_comments');
function legacy_comments($file) {
	if(!function_exists('wp_list_comments')) 	$file = TEMPLATEPATH . '/comments.legacy.php';
	return $file;
}

$content_width = 563;

?>
