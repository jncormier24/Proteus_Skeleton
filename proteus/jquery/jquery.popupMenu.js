$(function()
{
	initJqPopupMenus();
	
	// Added an additional "popup" plugin here for quickly adding a fade in/out "close" button (e.g., image clear)
	$.fn.popupClearIcon = function(options)
	{
		var thisObj = $(this);
		
		var defaults = {
			controlClass: "clearIcon",
			controlTooltip: "Clear",
			positionParent: "img",
			iconClass: "ui-icon-close",				
			clickMethod: null
		}
		
		options = $.extend(defaults, options);
		
		thisObj.hover(function()
		{				
			var parent = $(this);
			var obj = $(options.positionParent, parent);
			var ctl = $("a." + options.controlClass, parent);
				
			if (!ctl.length)
			{
				ctl = $("<a href='javascript:void(0)'>&nbsp;</a>").addClass(options.controlClass)
					   .addClass("ui-icon").addClass(options.iconClass).attr("Title","Clear Image");
					
				parent.prepend(ctl);
					
				ctl.position({
					of: obj,
					my: "right top",
					at: "right+6 top-6"
				}).click(options.clickMethod);
			}
				
			ctl.stop().fadeIn();				
		},
		function()
		{
			var parent = $(this);
			var obj = $(options.positionParent, parent);
			var ctl = $("a." + options.controlClass, parent);
			
			ctl.stop().fadeOut();
		});			
	}	
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