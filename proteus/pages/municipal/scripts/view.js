function getEntities(typeID)
{
	var cont = $("#entities");
	
	cont.showLoading("Loading Entities ...");
	
	$.getJSON("admin/ajax/municipal", {action: "getEntities", typeID: typeID}).then(function(output)
	{		
		if (!$.ajaxError(output, cont))
		{
			cont.html(output.content);
		}
	});
}
function getEntityEdit(entityID)
{
	if (!entityID) entityID = 0;
	$.showLoading("Loading Entity Edit Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getEntityEdit", entityID: entityID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
			
			$("#container").block({message: "", baseZ: 10});
			
			var opts = null;
			
			if (entityID)
			{
				opts = 
				{
					imageUpload: "admin/ajax/common/upload.ajax?typeID=2&dataID=" + entityID,
					fileUpload: "admin/ajax/common/upload.ajax?isFile=1&typeID=2&dataID=" + entityID
				}
			}
			
			$("#entityWin").dialog("option", "close", function() 
			{  
				$("#container").unblock();
				$(this).remove();
			}).find("textarea.wysiwyg").redactor(opts);
		}
	});
}
function updateEntity()
{
	var win = $("#entityWin");
	
	win.showLoading($("#entityID", win).val() ? "Saving Changes" : "Adding new Entity");	
	
	$.post("admin/ajax/municipal", $("#entityForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();		
		win.dialog('close');
		
		getEntities(output.typeID);		
	});
}
function deactivateEntity(entityID)
{
	if (!confirm("Are you SURE you want to permanently delete this entity and all associated data?")) return;
	var win = $("#entityWin");
	
	win.showLoading("Deactivating Entity");	
	
	$.post("admin/ajax/municipal?action=deactivateEntity&entityID=" + entityID, $("#entityForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();		
		win.dialog('close');
		
		getEntities(output.typeID);		
	});
}
function uploadEntityIcon(entityID)
{
	var _element = "entityIcon_" + entityID;
	
	$.showLoading("Uploading Entity Icon");	
	
	$.ajaxFileUpload( {
		url : 'admin/ajax/municipal?action=uploadEntityIcon&entityID=' + entityID,
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
				getEntities(data.typeID);	
			}
		},
		error : function(data, status, e)
		{
			alert(e);
		}
	});

	return false;
}
function getSettingsEdit()
{
	$.showLoading("Loading Settings Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getSettingsEdit"}).then(function(output)
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
function saveSettings()
{
	var win = $("#settingsWin");
	
	win.showLoading("Saving Settings");
	
	$.post("admin/ajax/municipal?action=saveSettings", $("#settingsForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		win.dialog('close');
	});
}
function getStaffWin(entityID)
{
	$.showLoading("Loading Staff Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getStaffWin", entityID: entityID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$(output.content).appendTo("body");
			
			/*getStaff(entityID, 1);
			getStaff(entityID, 2);*/
		}
	});	
}
function getStaff(entityID)
{	
	var cont = $("#staff");
	var filterObj = $("#staffFilter");	
	var ajaxOpts = { action: "getStaff", entityID: entityID };	
	
	cont.showLoading("Loading Staff Positions");
	
	$.getJSON("admin/ajax/municipal", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, cont))
		{		
			cont.html(output.content).tableRowAlternate();
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
			
			//initTableSort(true);
		}
	});	
}
function getStaffEdit(staffID, entityID)
{
	var win = $("#staffWin");
	win.showLoading("Loading Staff Edit Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getStaffEdit", entityID: entityID, staffID: staffID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
	});	
}
function saveStaff(staffID, entityID)
{
	var win = $("#staffEditWin");
	win.showLoading(staffID ? "Saving Staff Entry" : "Adding Staff Entry");
	
	$.post("admin/ajax/municipal?action=saveStaff&entityID=" + entityID + "&staffID=" + staffID, $("#staffEditForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getStaff(output.entityID, output.filter);
		
		win.dialog('close');
	});
}
function uploadStaffIcon(staffID)
{
	var _element = "staffIcon_" + staffID;
	var win = $("#staffWin");
	
	win.showLoading("Uploading Staff Icon");	
	
	$.ajaxFileUpload( {
		url : 'admin/ajax/municipal?action=uploadStaffIcon&staffID=' + staffID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			win.hideLoading();
			
			$("#" + _element).val("").fileinput();
			
			if (data.error)
			{
				alert(data.error);
			}			
			
			getMessages();
			getStaff(data.entityID, data.filter);
		},
		error : function(data, status, e)
		{
			alert(e);
		}
	});

	return false;
}
function toggleStaff(staffID)
{
	var win = $("#staffWin");
	
	win.showLoading("Modifying Status");
	
	$.getJSON("admin/ajax/municipal", {action: "toggleStaff", staffID: staffID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getStaff(output.entityID, output.filter);
		
		$("tr[data-id='" + staffID +"']").fadeOut("normal", function() { $(this).remove(); });
	});
}
function deleteStaff(staffID)
{
	if (!confirm("Are you SURE you want to permanently delete this staff member?\n\nThis cannot be undone!")) return;
	
	var win = $("#staffWin");
	
	win.showLoading("Removing Staff Entry");
	
	$.getJSON("admin/ajax/municipal", {action: "deleteStaff", staffID: staffID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();		
		$("tr[data-id='" + staffID +"']").fadeOut("normal", function() { $(this).remove(); });
	});
}
function getFaqWin(entityID)
{
	$.showLoading("Loading FAQ Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getFaqWin", entityID: entityID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
			getFaqEntries(entityID);
		}
	});	
}
function getFaqEntries(entityID)
{
	var cont = $("#faqWin div.faqContainer");
	cont.setWorking("Loading FAQ Entries");
	
	$.getJSON("admin/ajax/municipal", {action: "getFaqEntries", entityID: entityID}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);
		initTableSort();
	});	
}
function saveFaqEntry(faqEntryID, entityID)
{
	var win = $("#faqEditWin");
	win.showLoading(faqEntryID ? "Saving Faq Entry" : "Adding Faq Entry");
	
	$.post("admin/ajax/municipal?action=saveFaqEntry&entityID=" + entityID + "&faqEntryID=" + faqEntryID, $("#faqEditForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getFaqEntries(output.entityID);
		
		win.dialog('close');
	});
}
function getFaqEdit(faqEntryID, entityID)
{
	var win = $("#faqWin");
	win.showLoading("Loading Faq Edit Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getFaqEdit", entityID: entityID, faqEntryID: faqEntryID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
		initWYSIWYG(".wysiwyg", "#faqEditWin");
	});	
}
function deleteFaqEntry(faqEntryID)
{
	if (!confirm("Are you SURE you want to permanently delete this Faq Entry?\n\nThis cannot be undone!")) return;
	
	var win = $("#faqWin");
	
	win.showLoading("Removing Faq Entry");
	
	$.getJSON("admin/ajax/municipal", {action: "deleteFaqEntry", faqEntryID: faqEntryID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();		
		getFaqEntries(output.entityID);
	});
}
function getPostsWin(entityID)
{
	$.showLoading("Loading News &amp; Notices Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getPostsWin", entityID: entityID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
			refreshPostings(entityID);
		}
	});	
}
function getPostEntries(startPos, entityID, filter)
{
	if (!startPos) startPos = 0;
	var typeID = $("#postsFilter").val();	
	var cont;
	
	switch(filter)
	{
		case 1:
			cont = $("#active_posts");
			break;
		case 2:
			cont = $("#pending_posts");
			break;			
		case 3:
			cont = $("#expired_posts");
			break;
		case 4:
			cont = $("#inactive_posts");
			break;
	}
	
	
	cont = $("> div", cont).setWorking("Loading Posts");
	
	$.getJSON("admin/ajax/municipal", {action: "getPostEntries", entityID: entityID, typeID: typeID, filter: filter, startPos: startPos}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);		
		$("#" + cont.parent().attr("data-tablink")).removeClass('tabLoading').find(".tabLoaderIcon").replaceWith("<span class='commentCount tabLoaderIcon'> (" + output.count + ")</span>");
	});	
}
function savePostEntry(postEntryID, entityID)
{
	var win = $("#postEditWin");
	win.showLoading(entityID ? "Saving Post" : "Adding Post");
	
	$.post("admin/ajax/municipal?action=savePostEntry&entityID=" + entityID + "&postEntryID=" + postEntryID, $("#postEditForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		refreshPostings(output.entityID);
		
		win.dialog('close');
	});
}
function getPostEdit(postEntryID, entityID)
{
	var win = $("#postWin");
	win.showLoading("Loading Edit Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getPostEdit", entityID: entityID, postEntryID: postEntryID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
		
		initWYSIWYG(".wysiwyg", "#postEditWin");
		initProteusDatePickers();
		
	});	
}function deletePostEntry(postEntryID)
{
	if (!confirm("Are you SURE you want to permanently delete this Posting?\n\nThis cannot be undone!")) return;
	
	var win = $("#postWin");
	
	win.showLoading("Removing Post");
	
	$.getJSON("admin/ajax/municipal", {action: "deletePostEntry", postEntryID: postEntryID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();		
		refreshPostings(output.entityID);
	});
}
function refreshPostings(entityID)
{	
	$(function()
	{
		getPostEntries(0, entityID, 1);
		getPostEntries(0, entityID, 2);
		getPostEntries(0, entityID, 3);
		getPostEntries(0, entityID, 4);
	});
}
function togglePostStatus(postEntryID)
{
	var win = $("#postEditWin");
	
	win.showLoading("Saving");
	
	$.getJSON("admin/ajax/municipal", {action: "togglePostStatus", postEntryID: postEntryID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();			
		refreshPostings(output.entityID);
		
		win.dialog('close');
	});
}
function uploadPostIcon(postEntryID)
{
	var _element = "postIcon_" + postEntryID;
	var win = $("#postWin");
	
	win.showLoading("Uploading Post Icon");	
	
	$.ajaxFileUpload( {
		url : 'admin/ajax/municipal?action=uploadPostIcon&postEntryID=' + postEntryID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			win.hideLoading();
			
			$("#" + _element).val("").fileinput();
			
			if (data.error)
			{
				alert(data.error);
				return;
			}			
			
			getMessages();
			refreshPostings(data.entityID);
		},
		error : function(data, status, e)
		{
			alert(e);
		}
	});

	return false;
}
function getDocumentsWin(entityID)
{
	$.showLoading("Loading Documents Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getDocumentsWin", entityID: entityID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
			
			getContentEntries(1, entityID, '#custom_content > div');			
			getFiles(1, entityID);
			getLinks(1, entityID);
		}
	});
}
function getFilesWin(typeID, dataID, container)
{
	var cont = $(container);
	
	cont.showLoading("Loading File Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getFilesWin", typeID: typeID, dataID: dataID}).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{		
			$(output.content).appendTo("body");
			
			getFiles(typeID, dataID);
		}
	});
}
function getLinks(typeID, dataID)
{
	var cont = $("#linksContainer");
	cont.setWorking("Loading Link Resources");
	
	$.getJSON("admin/ajax/municipal", {action: "getLinks", typeID: typeID, dataID: dataID}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);
		initTableSort();
	});
}
function getLinkEdit(linkID, typeID, dataID, window)
{
	var win = window ? $(window) : $("#docWin");
	win.showLoading("Loading Link Resource Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getLinkEdit", linkID: linkID, typeID: typeID, dataID: dataID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
	});	
}
function saveLink(linkID, typeID, dataID)
{
	var win = $("#linkEditWin");
	win.showLoading("Saving Link Resource");
	
	$.post("admin/ajax/municipal?action=saveLink&linkID=" + linkID + "&typeID=" + typeID + "&dataID=" + dataID, $("#linkEditForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getLinks(output.typeID, output.dataID);		
		
		win.dialog('close');
	});
}
function deleteLink(linkID, container)
{
	if (!confirm("Are you SURE you want to permanently remove this link resource? This cannot be undone, and may affect any published hyperlinks indexed by search engines!")) return;
	
	var win = container ? $(container) : $("#docWin");
	win.showLoading("Removing Link Resource");
	
	$.getJSON("admin/ajax/municipal", {action: "deleteLink", linkID: linkID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();			
		getLinks(output.typeID, output.dataID);
	});
}
function getFiles(typeID, dataID)
{
	var cont = $("#filesContainer");
	cont.setWorking("Loading Files");
	
	$.getJSON("admin/ajax/municipal", {action: "getFiles", typeID: typeID, dataID: dataID}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);
		initTableSort();
	});
}
function getFileEdit(fileID, window)
{
	var win = window ? $(window) : $("#docWin");
	win.showLoading("Loading File Edit Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getFileEdit", fileID: fileID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
	});	
}
function saveFile(fileID)
{
	var win = $("#fileEditWin");
	win.showLoading("Saving File Entry");
	
	$.post("admin/ajax/municipal?action=saveFile&fileID=" + fileID, $("#fileEditForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getFiles(output.typeID, output.dataID);		
		
		win.dialog('close');
	});
}
function uploadFile(typeID, dataID, container)
{
	var _element = "entityFile";
	var win = container ? $(container) : $("#docWin");
	
	win.showLoading("Uploading File");	
	
	$.ajaxFileUpload( {
		url : 'admin/ajax/municipal?action=uploadFile&typeID=' + typeID + "&dataID=" + dataID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			win.hideLoading();
			
			$("#" + _element).val("").fileinput();
			
			if (data.error)
			{
				alert(data.error);
				return;
			}			
			
			getMessages();
			getFiles(typeID, dataID);
		},
		error : function(data, status, e)
		{
			alert(e);
		}
	});

	return false;
}
function deleteFile(fileID, container)
{
	if (!confirm("Are you SURE you want to permanently remove this file? This cannot be undone, and may affect any published hyperlinks indexed by search engines!")) return;
	
	var win = container ? $(container) : $("#docWin");
	win.showLoading("Removing File Entry");
	
	$.getJSON("admin/ajax/municipal", {action: "deleteFile", fileID: fileID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();			
		getFiles(output.typeID, output.dataID);
	});
}
function getAgendaWin(entityID)
{
	$.showLoading("Loading Agenda Management Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getAgendaWin", entityID: entityID}).then(function(output)
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
function getAgendas(entityID, filter)
{
	var cont = filter ? $("#past_meetings") : $("#upcoming_meetings");
	cont.setWorking("Loading Agenda Entries");
	
	$.getJSON("admin/ajax/municipal", {action: "getAgendas", entityID: entityID, filter: filter}).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{		
			cont.html(output.content);
			
			getAgendas(entityID, 0);
			getAgendas(entityID, 1);
		}
	});	
}
function getAgendaEdit(agendaID, entityID)
{
	var win = $("#agendaWin");
	win.showLoading("Loading Faq Edit Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getAgendaEdit", entityID: entityID, agendaID: agendaID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
		initWYSIWYG(".wysiwyg", "#agendaEditWin");
	});	
}
function saveAgenda(agendaID, entityID)
{
	var win = $("#agendaEditWin");
	win.showLoading(agendaID ? "Saving Agenda" : "Adding Agenda");
	
	$.post("admin/ajax/municipal?action=saveAgenda&entityID=" + entityID + "&agendaID=" + agendaID, $("#agendaEditForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getAgendas(output.entityID, 0);
		getAgendas(output.entityID, 1); //need to update both the upcoming and past meetings tabs
		
		win.dialog('close');
	});
}
function deleteAgenda(agendaID)
{
	if (!confirm("Are you SURE you want to permanently delete this Agenda?\n\nThis cannot be undone, and any associated Meeting Minutes will also be removed!")) return;
	
	var win = $("#agendaWin");
	
	win.showLoading("Removing Agenda");
	
	$.getJSON("admin/ajax/municipal", {action: "deleteAgenda", agendaID: agendaID}).then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();		
		getAgendas(output.entityID, 0);
		getAgendas(output.entityID, 1);
	});
}

function getEventsWin(entityID)
{
	$.showLoading("Loading Events Interface");
	
	$.getJSON("admin/ajax/municipal", {action: "getEventsWin", entityID: entityID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{				
			$(output.content).appendTo("body");
			getCalendars(1, entityID);
		}
	});	
}