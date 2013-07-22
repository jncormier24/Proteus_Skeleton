$(function() 
{
	getSubscriberOptions();
	//getSubscribers(0, 0, 0);
	
	getHistory(1);
	getHistory(2);
	
	//getEmailTemplates();
});

function getSubscriberOptions(hideLoader)
{
	var cont = $("#subscriber_options");
	if (!hideLoader) cont.showLoading("Loading Subscription Channels");
	
	$.getJSON("admin/ajax/subscribers", {action: "getSubscriberOptions"}).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{		
			cont.html(output.content).tableRowAlternate();
			initJqPopupMenus();
		}
	});
}
function getLinkedOptions()
{	
	$.showLoading("Loading Available Options");
	
	$.getJSON("admin/ajax/subscribers", {action: "getLinkedOptions", typeID: $("#subTypeID").val()}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$("#newChannelOptions").html(output.content);
		}
	});
}
function addSubscriptionOption()
{
	$.showLoading("Adding Subscription Channel");
	
	$.getJSON("admin/ajax/subscribers", 
	{
		action: "addSubscriptionOption", 
		dataID: $("#linkData").val(), 
		typeID: $("#subTypeID").val()
	}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
			getSubscriberOptions(true);
			getSubscribers(0,0,0);
		}
	});
}
function deleteSubscriptionOption(optionID)
{	
	$.jqConfirm("Are you sure you want to remove this channel? All subscribers will be retained if this Channel is ever added again.", function()
	{
		$.showLoading("Removing Subscription Channel");
		
		$.getJSON("admin/ajax/subscribers", {action: "deleteSubscriptionOption", optionID: optionID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{			
				getMessages();
				getSubscriberOptions(true);
				getSubscribers(0,0,0);
			}
		});
	});
}
function getSubscribers(filter, startPos, inactive)
{
	var cont = $("#subscribers");
	var filterObj = $("#subscribersFilter");	
	var ajaxOpts = { action: "getSubscribers" };	
	
	cont.showLoading("Loading Subscribers");	
	
	$.getJSON("admin/ajax/subscribers", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{
			cont.html(output.content).tableRowAlternate();
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		}
	});
}
function deactivateSubscriber(subID)
{
	$.jqConfirm("Are you sure you want to deactivate this subscriber?", function()
	{
		$.showLoading("Deactivating Subscription");	
		
		$.getJSON("admin/ajax/subscribers", {action: "deactivateSubscriber", subID: subID}).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getSubscriberOptions(true);
			getSubscribers()
		});
	});
}
function getHistory(type)
{
	var cont = type == 1 ? $("#email_drafts") : $("#email_sent");
	
	$.getJSON("admin/ajax/subscribers", {action: "getHistory", type: type}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);
	});
}
function deleteDraft(histID)
{
	$.jqCconfirm("Are you sure you want to remove this Draft without sending?", function()
	{	
		$.showLoading("Removing Draft");
		
		$.getJSON("admin/ajax/subscribers", {action: "deleteDraft", histID: histID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{			
				getMessages();
				getHistory(1);
			}
		});
	});
}
function getEmailTemplates()
{
	var cont = $("#email_templates");
	
	$.getJSON("admin/ajax/subscribers", {action: "getEmailTemplates"}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);
	});
}
function selectEmailTemplate(path)
{
	if (!confirm("Are you sure you want to make this the active email template?")) return;
	
	$.showLoading("Activating email template");

	$.getJSON("admin/ajax/subscribers", {action: "selectEmailTemplate", path: path}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			getEmailTemplates();
		}	
	});
}