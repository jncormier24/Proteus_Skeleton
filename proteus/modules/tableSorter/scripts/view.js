$(function()
{	
	initTableSort();
});
function initTableSort()
{
	$("table.sortable tbody").each(function()
	{
		// Don't initialize if there are less than 2 items.
		if ($("tr:has(td)", this).length < 2) return;		
		
		$("tr", this).each(function()
		{
			$(this).css("height", $(this).height() + "px");
		});
		
		$(this).tableSort({
			start: function(event, ui) 
			{					
				$(this).data("index_start", ui.item.index()); 
				return true;
			},
			stop: function(event, ui)
			{
				var obj = $(this);
				var startIndex = obj.data("index_start");
				var nextItem = ui.item.next();
				var prevItem = ui.item.prev();
				var replacementID;
				
				if ((nextItem.length || !prevItem.length) && ui.item.index() < startIndex)
				{
					replacementID = nextItem.attr("data-id");
				}
				else
				{
					replacementID = prevItem.attr("data-id");
				}
				
				$.showLoading("Saving Position");
				
				$.getJSON("admin/module/tableSorter", {
					action: "updatePosition",
					sortType: obj.parent().attr("data-sort-type"),
					sortID: ui.item.attr("data-id"),
					replacementID: replacementID
				}).then(function(output)
				{
					if (!$.ajaxError(output, $))
					{
						getMessages();
						if (obj.parent().is(".striped") && $.isFunction($.fn.tableRowAlternate)) obj.parent().tableRowAlternate();
					}
				});				
			}				
		}).css("cursor", "move");
	});
}