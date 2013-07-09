function getCalendars(typeID, dataID)
{
	var cont = $("#calendarsContainer");
	
	cont.setWorking("Fetching Calendars");
	
	$.getJSON("admin/module/calendar", { action: "getCalendars", dataID: dataID, typeID: typeID }).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);
	});
}
function getCalendarWin(calID, typeID, dataID)
{
	$.showLoading("Loading Calendar Interface");
	
	$.getJSON("admin/module/calendar", { action: "getCalendarWin", calID: calID, typeID: typeID, dataID: dataID }).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
	});
}
function saveCalendar(calID)
{
	if (!calID) calID = 0;
	$.post("admin/module/calendar?action=saveCalendar&calID=" + calID, $("#calForm").serialize(), function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}

		getMessages();
		getCalendars(output.typeID, output.dataID);
		
		if (!calID)
		{
			$("#calendarEdit").dialog('close');			
			getCalendarWin(output.calID);			
		}		
		
	}, "json");
}
function getEntries(calID)
{
	var cont = $("#cal_entries");
	cont.setWorking("Loading Calendar Entries");
	
	$.getJSON("admin/module/calendar", {
		action: "getEntries",
		calID: calID
	}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);
	});	
}
function getEntryEdit(entryID, calID)
{
	$.showLoading("Loading Edit Interface");

	$.getJSON("admin/module/calendar", {
		action: "getEntryEdit",
		entryID: entryID,
		calID: calID
	}).then(function(output)
	{
		$.hideLoading();

		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}

		$(output.content).appendTo("body");

		if (entryID)
		{
			getEntryFiles(entryID);
			getEntryIcon(entryID);
		}
		
		initWYSIWYG(".wysiwyg");
		//initBBCodeControl();
	});
}
function getEntryFiles(entryID)
{
	var cont = $("#entryFiles");
	cont.setWorking('Loading Files...');

	$.getJSON("admin/module/calendar", {
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
	$.getJSON("admin/module/calendar", {
		action: "getEntryIcon",
		entryID: entryID
	}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}

		$("#entryIconDiv").html(output.content);
		$("#entryIcon").fileinput();
	});
}
function deleteEntryIcon(entryID)
{
	var cont = $("#enWin");	
	cont.showLoading("Clearing Entry Icon");
	
	$.getJSON("admin/module/calendar", {
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
}
function saveEntry(entryID)
{
	$.post("admin/module/calendar?action=saveEntry&entryID=" + entryID, $("#calEntryForm").serializeArray(), function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}

		getMessages();
		getEntries(output.calID);

		if(!entryID) //if this is a new item, refresh the whole window so that the id is in the form (and we don't create 2x if hit save again)
		{
			$("#enWin").dialog('close');
			getEntryEdit(output.entryID, output.calID);
		}
		else
		{
			getEntryFiles(output.entryID);
			getEntryIcon(output.entryID);
		}

	}, "json");
}
function deleteEntry(entryID)
{	
	$.showLoading("Removing Calendar Entry");
	
	$.getJSON("admin/module/calendar", {
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
	
	$.getJSON("admin/module/calendar", {
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
		url : 'admin/module/calendar?action=attachFile&entryID=' + entryID,
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
	var cont = $("#enWin");	
	cont.showLoading("Uploading Entry Icon");	
	
	$.ajaxFileUpload( {
		url : 'admin/module/calendar?action=uploadIcon&entryID=' + entryID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			cont.hideLoading();
			
			if (data.error)
			{
				alert(data.error);
			}			
			
			$("#" + _element).val('').fileinput();
			
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
	if (!confirm("Are you SURE you want to permanently delete this calendar and all events? This CANNOT be undone!")) return;
	
	$.showLoading("Removing Calendar and Entries");
	
	$.getJSON("admin/module/calendar", { action: "deleteCalendar", calID: calID }).then(function(output)
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
}