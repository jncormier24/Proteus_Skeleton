$(function()
{
	// Just a simple wrapper to make creating WYSIWYG assets easier using the customContent module
	// see customContent/model.inc for supported object types (customContentData::getDataOject)
	
	$.fn.assetRedactor = function(typeID, dataID, contentID, redactorOptions)
	{
		var opts = {};
		
		if (dataID || contentID)
		{
			opts = 
			{
				imageUpload: "admin/module/customContent?action=assetUpload&typeID=" + typeID + "&dataID=" + dataID + "&contentID=" + contentID,
				imageUploadErrorCallback: function(output) { $.jqAlert(output.error); },
				fileUpload: "admin/module/customContent?action=assetUpload&isFile=1&typeID=" + typeID + "&dataID=" + dataID + "&contentID=" + contentID,
				fileUploadErrorCallback: function(output) { $.jqAlert(output.error); }
			}
		}
		
		// Allow passing of any variables to the redactor configuration
		redactorOptions = $.extend(opts, redactorOptions);
		
		return $(this).redactor(opts);
	}
});
function getContentEntries(typeID, dataID, container)
{	
	var container = $("#container_" + typeID + "_" + dataID);	
	
	container.showLoading();	
	
	$.getJSON("admin/module/customContent", {action: "getContentEntries", dataID: dataID, typeID: typeID}).then(function(output)
	{
		if (!$.ajaxError(output, container))
		{		
			container.html(output.content);
			
			initTableSort();
		}
	});
}
function getContentWin(contentID, dataID, typeID)
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
	
	$.getJSON("admin/module/customContent", {action: "getContentWin", contentID: contentID, dataID: dataID, typeID: typeID}).then(function(output)
	{
		if (!$.ajaxError(output, cont))			
		{		
			$.hideDialogs(true);
			
			$(output.content).appendTo("body");
			
			// Using blockContainer to help with the redactor -> modal dialog issue (7/13)
			$.blockContainer();
			
			var win = $("#contentWin");
			
			$(".wysiwyg", win).redactor({
				maxHeight: 370,
				imageUpload: "admin/module/customContent?action=assetUpload&contentID=" + contentID + "&dataID=" + output.dataID + "&typeID=" + output.uploadType,
				imageUploadErrorCallback: function(output) { $.jqAlert(output.error); },
				fileUpload: "admin/module/customContent?action=assetUpload&isFile=1&contentID=" + contentID + "&dataID=" + output.dataID + "&typeID=" + output.uploadType,
				fileUploadErrorCallback: function(output) { $.jqAlert(output.error); }				
			});
			
			win.dialog("option", "close", function()
			{
				$.showDialogs(true);				
				$(this).remove();
			}).updateHelper(updateContentEntry, {closeConfirmOnly: true, autoSave: false, disableControls: parseInt(output.disabled)});
		}
	});
}
function updateContentEntry()
{
	var win = $("#contentWin");
	var frm = $("#contentForm");	
	var contentID = $("#contentID", frm);
	
	if (contentID)
	{		
		cap = "Saving Custom Content";		
	}
	else
	{			
		cap = "Adding Custom Content";		
	}
	
	$.showLoading(cap);
	
	$.post("admin/module/customContent", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
		
			win.updateHelper("reset").dialog("close");
			
			if (!contentID)
			{
				getContentWin(output.contentID);
			}
		
			getContentEntries(output.typeID, output.dataID);
		}
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