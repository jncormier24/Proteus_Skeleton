function getShowcases()
{
	var cont = $("#showcases");
	var filterObj = $("#showcasesFilter");	
	var ajaxOpts = { action: "getShowcases" };	
	
	$.showLoading("Loading Showcases");	
	
	$.getJSON("admin/ajax/showcases", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
			
			initTableSort();			
			
			$("div.showcaseLeft:has(img.showcaseIcon)").each(function()
			{	
				var obj = $(this);
				var scID = obj.closest("tr").attr("data-id");
				
				obj.popupClearIcon(
				{	
					positionParent: "img.showcaseIcon",
					clickMethod: function() { clearShowcaseIcon(scID); }
				});
			});
		}
	});
}
function getShowcaseEdit(showcaseID)
{
	$.showLoading("Loading Showcase");
	
	$.getJSON("admin/ajax/showcases", { action: "getShowcaseEdit", showcaseID: showcaseID }).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$(output.content).appendTo("body");
			
			var win = $("#showcaseWin");			
			
			win.updateHelper(updateShowcase, { closeConfirmOnly: true, disableControls: output.disabled });
		}
	});
}
function updateShowcase()
{
	var dfd = $.Deferred();
	var win = $("#showcaseWin");
	var frm = $("#frmShowcase", win);
	var showcaseID = parseInt($("#showcaseID", frm).val());
	
	$.showLoading(showcaseID ? "Saving Showcase" : "Adding new Showcase");
	
	$.post("admin/ajax/showcases", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			
			win.updateHelper("reset").dialog("close");
			
			if (!showcaseID)	
			{				
				getShowcaseEdit(output.showcaseID);
			}				
			
			getShowcases();
			
			dfd.resolve();
		}
		else
		{
			dfd.reject();
		}			
	});
	
	return dfd.promise();
}
function deleteShowcase(showcaseID)
{
	var inactive = $(this).attr("data-inactive");
	
	$.jqConfirm(inactive ? "Are you sure you want to permanently delete this showcase?\n\nThis will *PERMANENTLY* delete all images in the showcase folder and cannot be undone!" : "Are you sure you want to deactivate this showcase?", function()
	{
		$.showLoading((inactive ? "Deleting" : "Deactivating") + " Showcase");
		
		$.getJSON("admin/ajax/showcases", { action: "deleteShowcase", showcaseID: showcaseID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
				getShowcases();
			}
		});
	});
}
function uploadShowcaseIcon(showcaseID)
{
	var _element = "showcaseIcon_" + showcaseID;
	
	$.showLoading("Uploading Showcase Icon");	
	
	$.ajaxFileUpload( {
		url : 'admin/ajax/showcases?action=uploadShowcaseIcon&showcaseID=' + showcaseID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			$.hideLoading();
			
			$("#" + _element).fileinput();
			
			if (data.error)
			{
				$.jqAlert(data.error);
			}	
			else
			{					
				getMessages();
				getShowcases();	
			}
		},
		error : function(data, status, e)
		{
			alert(e);
		}
	});

	return false;
}
function clearShowcaseIcon(showcaseID)
{
	$.jqConfirm("Are you sure you want to clear this Showcase icon?", function()
	{
		$.showLoading("Clearing Showcase Icon");
		
		$.getJSON("admin/ajax/showcases", { action: "clearShowcaseIcon", showcaseID: showcaseID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
				getShowcases();
			}
		});
	});
}
function getShowcaseEntriesWin(showcaseID)
{
	$.showLoading("Loading Showcase Management Console");
	
	$.getJSON("admin/ajax/showcases", {action: "getShowcaseEntriesWin", showcaseID: showcaseID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
			getShowcaseEntries(showcaseID);
			
			var frm = $("#filesForm");
			var win = $("#showcaseWin");
			
			win.next().prepend(frm);
		}
	});
}
function getShowcaseEntries(showcaseID)
{
	var cont = $("#showcaseEntries");
	$.showLoading("Loading Showcase Entries");
	
	$.getJSON("admin/ajax/showcases", {action: "getShowcaseEntries", showcaseID: showcaseID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			cont.html(output.content).tableRowAlternate();
			
			initTableSort();
			initJqPopupMenus();
		}
	});
}
function uploadShowcaseImage(showcaseID)
{
	var win = $("#showcaseWin");
	var element = "fileToUpload";
	
	$.showLoading("Uploading Showcase Image, please wait for this message to close.");	
	
	$.ajaxFileUpload(
	{
		url: "admin/ajax/showcases?action=uploadShowcaseImage&showcaseID=" + showcaseID, 			
		secureuri:false,
		fileElementId: element,
		dataType: 'json',		
		success: function (data, status)
		{		
			$.hideLoading();
			
			$("#" + element).val("").fileinput();
			
			if (data.error)
			{								
				$.jqAlert(data.error);				
				return;
			}			
			
			getMessages();
			getShowcaseEntries(showcaseID);
			getShowcases();
		},
		error: function (data, status, e)
		{
			$.hideLoading();			
		}
	});
	
	return false;
}
function deleteShowcaseItem(itemID)
{
	$.jqConfirm("Are you sure you want to permanently remove this Showcase Item? This deletes the physical file as well and cannot be undone.", function()
	{
		$.showLoading("Loading Deleting Showcase Item");
	
		$.getJSON("admin/ajax/showcases", {action: "deleteShowcaseItem", itemID: itemID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{		
				getMessages();
				getShowcaseEntries(output.showcaseID);
				getShowcases();
			}
		});
	});
}
function getShowcaseItemEdit(itemID)
{
	$.showLoading("Loading Showcase Item Data");
	
	$.getJSON("admin/ajax/showcases", { action: "getShowcaseItemEdit", itemID: itemID }).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$.hideDialogs(true);
			
			$(output.content).appendTo("body");
			
			var win = $("#showcaseItemWin");
			
			$(".wysiwyg", win).assetRedactor(-6, output.showcaseID, itemID);
			
			win.dialog("option", "close", function()
			{
				$.showDialogs(true);
				$(this).remove();				
			}).updateHelper(updateShowcaseItem, { closeConfirmOnly: true, disableControls: output.disabled });
		}
	});
}
function updateShowcaseItem()
{
	var dfd = $.Deferred();
	var win = $("#showcaseItemWin");
	var frm = $("#frmShowcaseItem", win);	
	
	$.showLoading("Saving Showcase Item");
	
	$.post("admin/ajax/showcases", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			
			win.updateHelper("reset").dialog("close");
			
			getShowcaseItems(output.showcaseID);
			getShowcases();			
			
			dfd.resolve();
		}
		else
		{
			dfd.reject();
		}			
	});
	
	return dfd.promise();
}