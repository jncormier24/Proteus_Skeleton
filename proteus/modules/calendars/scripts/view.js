function getCalendars(typeID, dataID)
{
	var cont = $("#calendarsContainer");
	
	$.showLoading("Fetching Calendars");
	
	$.getJSON("admin/module/calendars", { action: "getCalendars", dataID: dataID, typeID: typeID }).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			cont.html(output.content).tableRowAlternate();
			
			initJqPopupMenus();
		}
	});
}
function getCalendarWin(calID, typeID, dataID)
{
	$.showLoading("Loading Calendar Interface");
	
	$.getJSON("admin/module/calendars", { action: "getCalendarWin", calID: calID, typeID: typeID, dataID: dataID }).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
		}
	});
}
function saveCalendar()
{	
	var frm = $("#calForm");
	var calID = $("#calID", frm);
	
	$.showLoading(calID ? "Saving Calendar Detail" : "Adding Calendar");
	
	$.post("admin/module/calendars", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
			getCalendars(output.typeID, output.dataID);
		
			if (!parseInt(calID))
			{
				$("#calendarEdit").dialog('close');			
				getCalendarWin(output.calID);			
			}
		}		
	});
}
function getEntriesWin(calID)
{
	$.showLoading("Loading Entry Management Interface");
	
	$.getJSON("admin/module/calendars", { action: "getEntriesWin", calID: calID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$(output.content).appendTo("body");
			getEntries(calID);
		}
	});
}
function getEntries(calID)
{
	var cont = $("#entriesWin");
	cont.setWorking("Loading Calendar Entries");
	
	$.getJSON("admin/module/calendars",	{ action: "getEntries", calID: calID}).then(function(output)
	{
		if (!$.ajaxError(output))
		{		
			cont.html(output.content).tableRowAlternate();
			initJqPopupMenus();
		}
	});	
}
function getEntryEdit(entryID, calID)
{
	$.showLoading("Loading Edit Interface");

	$.getJSON("admin/module/calendars", {
		action: "getEntryEdit",
		entryID: entryID,
		calID: calID
	}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$.hideDialogs(true);		
			
			$(output.content).appendTo("body");

			if (entryID)
			{
				getEntryIcon(entryID);
			}
			
			var win = $("#entryEditWin");
			
			$(".wysiwyg", win).assetRedactor(7, output.calID, entryID);
		
			win.dialog("option", "close", function()
			{
				$.showDialogs(true);
				$(this).remove();
			}).updateHelper(saveEntry, {closeConfirmOnly: true, disableControls: parseInt(output.disabled)});
		}
	});
}
function getEntryFiles(entryID)
{
	var cont = $("#entryFiles");
	cont.setWorking('Loading Files...');

	$.getJSON("admin/module/calendars", {
		action: "getEntryFiles",
		entryID: entryID
	}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}

		cont.html(output.content);
		$("#fileToUpload", cont).fileinput();
	});
}
function getEntryIcon(entryID)
{
	$.getJSON("admin/module/calendars", {
		action: "getEntryIcon",
		entryID: entryID
	}).then(function(output)
	{
		if (!$.ajaxError(output))
		{		
			$("#entryIconDiv").html(output.content);
			$("#entryIcon").fileinput();
		}
	});
}
function deleteEntryIcon(entryID)
{
	$.jqConfirm("Are you sure you want to delete this Entry's icon?", function()
	{
		var cont = $("#enWin");	
		cont.showLoading("Clearing Entry Icon");
		
		$.getJSON("admin/module/calendars", {
			action: "deleteEntryIcon",
			entryID: entryID
		}).then(function(output)
		{
			cont.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getEntryIcon(entryID);
		});
	});
}
function saveEntry(entryID)
{
	$.showLoading("Saving Entry");
	
	$.post("admin/module/calendars", $("#calEntryForm").serializeArray(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
			getEntries(output.calID);

			if (!entryID)
			{
				$("#entryEditWin").dialog("close");
				getEntryEdit(output.entryID);
			}
			else
			{
				//getEntryFiles(output.entryID);
				getEntryIcon(output.entryID);
			}
		}
	});
}
function deleteEntry(entryID)
{	
	$.showLoading("Removing Calendar Entry");
	
	$.getJSON("admin/module/calendars", {
		action: "deleteEntry",
		entryID: entryID
	}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getEntries(output.calID);
	});
}
function deleteEntryFile(entryID, hash, salt)
{
	var cont = $("#enWin");	
	cont.showLoading("Removing Linked File");
	
	$.getJSON("admin/module/calendars", {
		action: "deleteEntryFile",
		entryID: entryID,
		fileHash: hash,
		salt: salt
	}).then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getEntryFiles(entryID);
	});
}
function attachFile(entryID)
{
	var _element = "fileToUpload";
	var cont = $("#enWin");	
	cont.showLoading("Attaching File");
	
	$.ajaxFileUpload( {
		url : 'admin/module/calendars?action=attachFile&entryID=' + entryID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			cont.hideLoading();
			
			if (data.error)
			{
				alert(data.error);
				return;
			} 
			
			$("#" + _element).val("").fileinput();
					
			getMessages();
			getEntryFiles(entryID);			
		},
		error : function(data, status, e)
		{
			cont.hideLoading();
			alert(e);
		}
	});

	return false;
}
function uploadIcon(entryID)
{
	var _element = "entryIcon";
	
	$.showLoading("Uploading Entry Icon");	
	
	$.ajaxFileUpload( {
		url : 'admin/module/calendars?action=uploadIcon&entryID=' + entryID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			$.hideLoading();
			
			if (data.error)
			{
				$.jqAlert(data.error);
			}			
			
			$("#" + _element).val('').fileinput().nextAll("button").prop("disabled", true);
			
			getMessages();
			getEntryIcon(entryID);
		},
		error : function(data, status, e)
		{
			alert(e);
		}
	});

	return false;
}
function deleteCalendar(calID)
{
	$.jqConfirm("Are you SURE you want to permanently delete this calendar and all events? This CANNOT be undone!", function()
	{	
		$.showLoading("Removing Calendar and Entries");
		
		$.getJSON("admin/module/calendars", { action: "deleteCalendar", calID: calID }).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getCalendars(output.typeID, output.dataID);
		});
	});
}