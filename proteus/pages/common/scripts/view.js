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
	
	$(document).on({
		touchstart: function(event) 
		{ 
			$("ul li ul", navObj).stop().slideUp();
		}
	}, "body");
	
	// Disable the enter key automatically submitting forms	
	$(document).on({
		keydown: function(event) 
		{ 
			if (event.keyCode == 13)
			{
				event.preventDefault();
				return false;
			}
		}
	}, "form > input, form select");
		
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
		$("td", this).removeClass("evenCell oddCell").closest("table").addClass("striped");
		$("td", $("tr:has(td)", this).filter(":even")).each(function() 
		{ 		
			$(this).addClass("evenCell"); 
		});
		
		return $(this);
	}
});
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