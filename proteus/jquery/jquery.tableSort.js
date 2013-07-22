(function($)
{	
	$.fn.tableSort = function(options)
	{
		if ($(this).data("sortable-loaded")) return $(this);
		if ($("tr:has(td)", this).length < 2)
		{
			$(this).removeClass('sortable');
			return;
		}
		
		var parent = $(this).parent();
		var container = parent.closest(".ui-dialog-content");
		
		if (!container.length) container = parent;
		
		var op = $.extend({
			start: function() {},
			stop: function() {},
			container: container
		}, options);
		
		var fixHelper = function(e, ui) 
		{
			ui.children().each(function() {
				$(this).width($(this).width());
			});
			
			return ui;
		};
		
		$(this).not("sortLoaded").sortable({
			helper: fixHelper,
			forcePlaceholderSize: true,
			opacity: 0.85,
			items: $("tr", this).not(":has(th)"),
			start: op.start,
			stop: op.stop,
			containment: op.container
		}).data("sortable-loaded", true).addClass("sortLoaded");
		
		return $(this);
	};
	
})(jQuery);