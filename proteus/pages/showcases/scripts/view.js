function getShowcases()
{
	var cont = $("#showcases");
	
	$.showLoading("Loading Showcases");
	
	$.getJSON("admin/ajax/showcases", { action: "getShowcases" }).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate();
		}
	});
}
function getShowcaseEdit(showcaseID)
{
	$.showLoading("Loading Showcase");
	
	$.getJSON("admin/ajax/showcases", { action: "getShowcaseEdit", showcaseID: showcaseID }).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$(output.content).appendTo("body");
		}
	});
}
function updateShowcase()
{

}