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
		
		var op = $.extend({start: function() {}}, options);
		
		$(this).not(".sortLoaded").sortable({
			helper: 'original',
			opacity: 0.85,
			items: $("tr", this).not(":has(th)"),
			start: op.start,
			stop: op.stop,
			containment: $(this).parent()
		}).data("sortable-loaded", true).addClass(".sortLoaded");
		
		return $(this);
	};
	$.fn.evenOddStripe = function()
	{
		$("td", this).removeClass("evenCell oddCell");
		$("tr:odd:not(:has(th)) td", this).addClass('oddCell');
		$("tr:even:not(:has(th)) td", this).addClass('evenCell');
	};
})(jQuery);