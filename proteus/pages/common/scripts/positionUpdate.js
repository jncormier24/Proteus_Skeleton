$(function()
{	
	initTableSort(false);
});
function initTableSort(noStripe)
{
	$("table.sortable").each(function()
	{
		$(this).tableSort({
			start: function(event, ui) { /*ui.helper.css("width", "100%");*/ },
			stop: function(event, ui)
			{
				var arr = new Array();
				var parent = this;
				var ct = 0;
				
				$("tr:not(:has(th))", parent).each(function()
				{
					arr.push($(this).attr("data-id"));
				});
				
				$.ajax({
					url: "index.php?action=updatePos",
					dataType: "text",
					data: {table: $(parent).attr("data-table"), 'idArr': arr, quantify: $(parent).attr("data-quantify")}, 
					type: "post",
					error: function (XMLHttpRequest, textStatus, errorThrown) {
						alert(textStatus + "--" + errorThrown);
					},			
					success: function()
					{
						getMessages();
						if (!noStripe) $(parent).evenOddStripe();
					}
				});
			},
		});
		
		if (!noStripe) $(this).evenOddStripe();
	});
	
	$("tr:not(:has(th))", "table.sortable").css("cursor", "move");	
}