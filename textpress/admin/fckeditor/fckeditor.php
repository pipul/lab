<?php

if ( version_compare( phpversion(), '5', '<' ) )
	include_once( 'fckeditor_php4.php' ) ;
else
	include_once( 'fckeditor_php5.php' ) ;

?>