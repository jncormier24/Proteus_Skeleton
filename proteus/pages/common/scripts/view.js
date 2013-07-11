$(function()
{
	var navObj = $("#navContent");
	
	// Set up the admin menu
	$("ul li ul", navObj).menu().hide().parent().hover(function()
	{		
		var timeout = $(this).data("menuTimer");
		
		if (timeout)
		{
			clearTimeout(timeout);	
			$(this).data("menuTimer", "");
			
			return;
		}					
		
		$(".ui-menu:visible", navObj).slideUp("fast");
		
		$("ul", this).stop().slideDown("fast").position({
			of: this,
			my: "left top",
			at: "left bottom"
		});
	}, function()
	{
		var obj = $(this);
		
		if (!$("ul:visible", obj).length) return;
		
		var timeout = setTimeout(function() 
		{
			$("ul", obj).stop().slideUp();
			obj.data("menuTimer", "");
		}, 500);
		
		obj.data("menuTimer", timeout);
	});
	
	// Set all the inputs to react to being focused; add a class and define style in style.css (since it's global)
	$(document).on(
	{
		focus: function()
		{
			$(this).addClass("focused");
		},
		blur: function()
		{
			$(this).removeClass("focused");
		}
	}, "input, textarea");
	
	$.fn.tableRowAlternate = function()
	{
		$("table.listTable td", this).removeClass("evenCell oddCell");
		$("td", $("table.listTable tr:has(td)", this).filter(":even")).each(function() 
		{ 		
			$(this).addClass("evenCell"); 
		});
		
		return $(this);
	}	
	$.blockContainer = function()
	{
		// Just a wrapper function to facilitate manually locking the container div for issues with Redactor in modal JQuery Dialogs (7/13)
		$("#container").block({message: "", baseZ: 10});
	}
	$.unblockContainer = function()
	{
		$("#container").unblock();
	}
	$.hideDialogs = function(blockContainer)
	{
		if (blockContainer) $.blockContainer();
		
		$(".ui-dialog-content").each(function()
		{				
			// Hide any open dialogs (Redactor), reset the close event				
				$("#" + $(this).attr("id")).dialog("option", "close", null).dialog("close");
		});
	}
	$.showDialogs = function (unblockContainer)
	{
		if (unblockContainer) $.unblockContainer();				
		
		/* 
		 * SWB 7/13
		 * Restore the "close" directive on dialog close. This is hoakey, but it bullet-proofs the issue around the Redactor
		 * modal dialog issue, making this the only window open and the container element blocked for interaction. 
		 * Obviously all windows that call this function will be reset to remove on close, but that's the default
		 * functionality of most configuration windows anyway; reasonable workaround.
		*/
		
		$(".ui-dialog-content").each(function()
		{
			$("#" + $(this).attr("id")).dialog("option", "close", function() { $(this).remove(); }).dialog("open");
		});
	}
});

function getMessages()
{
	var ttl = 8000;
	
	//Get any message boxes...
	$.getJSON("admin/ajax/common", {action: "getMessages"}).then(function(output)
	{
		if (output.message) $.jGrowl("<span class='ui-icon ui-icon-circle-check' style='float:left; margin:1px 7px 5px 0px;'></span><span class='systemNotif'>" + output.message + "</span>", { life: ttl, header: "System Notification:" });
	});	
}
function logout(ref)
{
	$.jqConfirm("Are you sure you want to Log Out?", function()
	{
		$.showLoading("Logging you out");
		
		$.getJSON("admin/ajax/common", {action: "logout"}).then(function(output)
		{
			window.location = ref;
		});
	});
}