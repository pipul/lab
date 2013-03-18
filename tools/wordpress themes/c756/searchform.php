	<form action="<?php bloginfo('url'); ?>/" method="get" id="searchform">
		<p><input type="text" name="s" value="<?php get_search_query() == "" ? _e("Search", "c756") : the_search_query(); ?>" id="searchInput" onclick = "document.getElementById('searchInput').select();" onkeypress = "return submitenter(this,event);" /></p>
	</form>
