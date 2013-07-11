function getContentEntries(typeID, dataID, container)
{	
	var dataElem = "container_" + typeID + "_" + dataID;
	
	if (!container) container = $(document).data(dataElem);
	
	container = $(container);
	
	container.showLoading();
	
	//Store the container for later retrieval (refreshes)
	$(document).data(dataElem, container);
	
	$.getJSON("admin/module/customContent", {action: "getContentEntries", dataID: dataID, typeID: typeID}).then(function(output)
	{
		if (!$.ajaxError(output, container))
		{		
			container.html(output.content);
			initTableSort();
		}
	});
}
function getContentWin(contentID)
{
	if ($("#contentWin").length)
	{	
		$("#contentWin").dialog("moveToTop");
		$.jqAlert("Editor already open; please close existing editor and try again.");
		
		return;		
	}
	
	var cont;
	var dlg = $(".ui-dialog:last");
	
	cont = dlg.length ? dlg : $;
	
	cont.showLoading("Loading Custom Content Editor");
	
	$.getJSON("admin/module/customContent", {action: "getContentWin", contentID: contentID}).then(function(output)
	{
		if (!$.ajaxError(output, cont))			
		{		
			$.hideDialogs(true);
			
			$(output.content).appendTo("body");
			
			// Using blockContainer to help with the redactor -> modal dialog issue (7/13)
			$.blockContainer();
				
			$(".wysiwyg").redactor({
				maxHeight: 370,
				imageUpload: "admin/module/customContent?action=assetUpload&contentID=" + contentID + "&dataID=" + output.dataID + "&typeID=" + output.uploadType,
				imageUploadErrorCallback: function(output) { $.jqAlert(output.error); },
				fileUpload: "admin/module/customContent?action=assetUpload&isFile=1&contentID=" + contentID + "&dataID=" + output.dataID + "&typeID=" + output.uploadType,
				fileUploadErrorCallback: function(output) { $.jqAlert(output.error); }				
			});
			
			$("#contentWin").dialog("option", "close", function()
			{
				$.showDialogs(true);				
				$(this).remove();
			});
		}
	});
}
function updateContentEntry(contentID, typeID, dataID)
{
	var cont, cap, frm;	
	
	if (contentID)
	{
		cont = $("#contentWin");
		cap = "Saving Custom Content";
		frm = $("#contentForm");
	}
	else
	{		
		cont = $;
		cap = "Adding Custom Content";
		frm = $("#contentForm_new");
		
		var tmp = $(frm).closest(".ui-dialog");
		if (tmp.length) cont = tmp;
	}
	
	cont.showLoading(cap);
	
	$.post("admin/module/customContent", {
		action: "updateContentEntry",
		contentID: contentID,
		dataID: dataID,
		typeID: typeID,
		fields: frm.serializeArray()
	}, null, "json").then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		
		if (!contentID)
		{
			getContentWin(output.contentID);
		}
		else
		{
			cont.dialog('close');			
		}
		
		getContentEntries(output.typeID, output.dataID);
		
	});
}
function deleteContentEntry(contentID)
{
	$.jqConfirm("Are you sure you want to permanently remove this content page? This will remove all associated assets uploaded to the content as well!", function()
	{
		$.showLoading("Removing Custom Content");
		
		$.getJSON("admin/module/customContent", {action: "deleteContentEntry", contentID: contentID}).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getContentEntries(output.typeID, output.dataID);
		});
	});
}