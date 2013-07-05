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
	
	$.fn.tableRowAlternate = function()
	{
		$("table.listTable td", this).removeClass("evenCell oddCell");
		$("td", $("table.listTable tr:has(td)", this).filter(":even")).each(function() 
		{ 		
			$(this).addClass("evenCell"); 
		});
		
		return $(this);
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