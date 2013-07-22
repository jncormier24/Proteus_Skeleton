function getSubscriptionSendWin(optionID, entryID, typeID, dataID, template)
{
	if (!typeID) typeID = 0;
	if (!dataID) dataID = 0;
	
	$.showLoading("Loading Email Interface");
	
	$.getJSON("admin/module/subscribers", 
	{
		action: "getSubscriptionSendWin", 
		optionID: optionID, 
		entryID: entryID, 
		typeID: typeID, 
		dataID: dataID,
		template: template
	}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{				
			if (output.template)
			{
				$.hideDialogs(true);
				
				$(output.content).appendTo("body");
				
				var win = $("#emailWin");
				
				// Manually bind the keyupCallback so that Redactor will properly route keyup through the forced-iframe view (for full page editing)
				$(".wysiwyg", win).redactor({fullpage: true, keyupCallback: function() { win.updateHelper("setDirty"); }});
				
				// No support for image/file uploads in Redactor for subscriptions
				win.dialog("option","close", function()
				{
					$.showDialogs(true);
					$(this).remove();
				}).updateHelper(function() { subscriptionSend(-1); }, {closeConfirmOnly: true, autoSave: false});
			}
			else
			{
				$(output.content).appendTo("body");
			}
		}
	});
}
function subscriptionSend(override)
{
	var stored = $("#emailOverride");		
	var cont = $("#emailWin");
	
	var dfd = new $.Deferred();
	
	if (override == 1)
	{		
		//Send to a specific address as a sample
		emailOverride = prompt("Enter an address to send to:", stored.val());
		
		if (!emailOverride)
		{			
			return;
		}
		
		stored.val(emailOverride);
		$.showLoading("Sending Email Sample");
	}
	else if (override == -1)
	{
		//Just save
		stored.val(-1);
		$.showLoading("Saving Email Draft");
	}
	else
	{
		if (!confirm('Are you *sure* you want to send all emails now?\n\nThis process cannot be undone and should not be interrupted or the procedure will not complete, and sending again may produce duplicate emails to subscribers.\n\nPlease be very sure you are ready to send all emails before proceeding!')) return;
		
		stored.val("");
		
		$.showLoading("Sending Email(s)...<br /><strong><em>DO NOT INTERRUPT THIS PROCESS OR EMAILS MAY BE LOST!</em></strong>");	
	}
	
	$.post("admin/module/subscribers", $("#emailForm").serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			if (output.entryID)
			{
				$("#entryID", cont).val(output.entryID);
			}
		
			getMessages();		
		
			getHistory(1);
			getHistory(2);
			
			$("#emailWin").updateHelper("reset");
			
			if (override != 1) cont.dialog("close");
			
			dfd.resolve();
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();
}
function showSubscribe(optionID, subID)
{
	$.showLoading("Loading Subscription Interface");
	
	return $.getJSON("admin/module/subscribers", {
		action: "showSubscribe", 
		optionID: optionID, 
		subID: subID
	}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
		}
		
		$(output.content).appendTo("body");
	});
}
function subscribe()
{
	var dfd = new $.Deferred();
	var cont = $('#subscribeWin');
	
	$.showLoading("Saving Subscription Preferences");
	
	$.post("admin/module/subscribers", $('#subscribeForm').serializeArray(), null, 'json').then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
			
			if (!output.content)
			{
				cont.dialog("close");
				if ($.isFunction(getSubscribers)) getSubscribers();
				
				dfd.resolve();
				
				return;
			}
			
			$(output.content).appendTo("body");
			
			dfd.resolve();
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();
}