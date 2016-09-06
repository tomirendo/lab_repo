jQuery("#header-search-form .search-submit").focus(function() {
	jQuery("#header-search-form .search-submit").animate({width: 160}, 'medium');
});

jQuery(".search-submit").focusout(function() {	
	jQuery("#header-search-form .search-submit").animate({width: 0}, 'medium');	
});

jQuery(window).scroll(function(){
       var scrolled = jQuery(window).scrollTop();
       jQuery("#ease-scroll").stop().animate({opacity: (scrolled>450 ? 0.9 : 0) }, 600);
});

jQuery(window).ready(function() {
	if ( jQuery('#attachment-image').length ) {
	  jQuery(window).scrollTop( 380 );
	}
});