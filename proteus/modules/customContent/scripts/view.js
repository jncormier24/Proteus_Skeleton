function getContentEntries(typeID, dataID, container)
{	
	var dataElem = "container_" + typeID + "_" + dataID;
	
	if (!container) container = $(document).data(dataElem);
	
	$(container).setWorking();
	
	//Store the container for later retrieval (refreshes)
	$(document).data(dataElem, container);
	
	$.getJSON("admin/module/customContent", {action: "getContentEntries", dataID: dataID, typeID: typeID}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(container).html(output.content);
		initTableSort();
	});
}
function getContentWin(contentID)
{
	var cont;
	var dlg = $(".ui-dialog:last");
	
	cont = dlg.length ? dlg : $;
	
	cont.showLoading("Loading Custom Content Editor");
	
	$.getJSON("admin/module/customContent", {action: "getContentWin", contentID: contentID}).then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
		
		//initWYSIWYG(".wysiwyg", "#contentWin");
		
		$(".wysiwyg").redactor({
			maxHeight: 370,
			imageUpload: "index.php?action=uploadSupportImage&dataID=" + contentID + "&typeID=" + output.uploadType,
			fileUpload: "index.php?action=uploadSupportImage&isFile=1&dataID=" + contentID + "&typeID=" + output.uploadType
		});
		
		

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
}