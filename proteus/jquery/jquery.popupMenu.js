$(function()
{
	initJqPopupMenus();
});

function initJqPopupMenus()
{
	$("div.jqPopupMenu").not(".jqPopupRendered").each(function(output)
	{
		var parent = $(this);
		var btn = $("button", parent);
		var list = $("ul", parent);
		
		btn.css({
			height: "22px"
		});
		
		list.css({
			position: "absolute",			
			zIndex: 5000,
			textAlign: "left",
			left: -500
		}).menu().hover(function()
		{
			clearTimeout(parent.data("timer"));
		}, function()
		{
			parent.data("timer", setTimeout(function() { list.fadeOut(); }, 250));
		}).hide().addClass("jqPopup").click(function() { $(this).fadeOut("fast"); }).appendTo("body");
		
		$("a", list).click(function() { list.fadeOut("fast"); });
		
		btn.hover(function()
		{
			clearTimeout(parent.data("timer"));
			
			if (list.is(":visible")) return;
			
			$("ul.jqPopup").not(list).hide();			
			
			list.show().position({
				of: btn,
				my: "right center",
				at: "left center",
				collision: "fit"
			}).hide().fadeIn();
		}, function()
		{
			parent.data("timer", setTimeout(function() { list.fadeOut(); }, 250));
		}).click(function()
		{
			$("a:eq(0)", list).click();
			list.fadeOut();
		});		
			
		parent.css("position","relative").addClass("jqPopupRendered");
	});
}