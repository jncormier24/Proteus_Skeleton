function getForms()
{
	var cont = $("#customForms");
	var filterObj = $("#formsFilter");	
	var ajaxOpts = { action: "getForms" };	
	
	$.showLoading("Loading Custom Forms");	
	
	$.getJSON("admin/ajax/customForms", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate;
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		}
	});
}
function getFormEdit(formID)
{
	$.showLoading("Loading Custom Form Interface");
	
	$.getJSON("admin/ajax/customForms", {action: "getFormEdit", formID: formID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$(output.content).appendTo("body");
			
			$.blockContainer();
			
			var win = $("#formWin");
			
			// Need to inialized redactor before initializing the updateHelper to check for dirty controls (accesses Redactor method "getEditor")
			$("textarea.wysiwyg", win).assetRedactor(6, formID);
			
			win.dialog("option", "close", function() 
			{  
				$.unblockContainer();						
				$(this).remove();
			}).find("#frmForm").updateHelper(updateForm, {closeConfirmOnly: true, autoSave: false, disableControls: output.disabled});
			
			if (formID) getFormFields(formID);
		}
	});
}
function updateForm()
{
	var dfd = $.Deferred();
	var frm = $("#frmForm");
	var win = $("#formWin");
	var formID = parseInt($("#formID", frm).val());
	
	$.showLoading(formID ? "Saving Form" : "Adding new Form");
	
	$.post("admin/ajax/customForms", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			
			frm.updateHelper("reset");
			
			win.dialog("close");
			
			if (!formID)
			{
				getFormEdit(output.formID);
			}
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();
}
function getFormFields(formID)
{
	var cont = $("#fieldsContainer");
	
	cont.showLoading("Loading Custom Form Interface");
	
	$.getJSON("admin/ajax/customForms", {action: "getFormFields", formID: formID}).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{			
			cont.html(output.content).tableRowAlternate();
			initTableSort();
		}
	});
}
function getFieldEdit(fieldID, formID)
{
	$.showLoading("Loading Form Field Interface");
	
	$.getJSON("admin/ajax/customForms", {action: "getFieldEdit", fieldID: fieldID, formID: formID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$(output.content).appendTo("body");
			
			toggleFieldType();		
			
			getFieldsets().then(function()
			{
				var ctl = $("[name='field[fieldsetID]']");
				ctl.val(ctl.attr("data-selected"));
			});
	
			$("#frmField").updateHelper(updateFormField, {closeConfirmOnly: true, disableControls: parseInt(output.disabled)});
		}
	});
}
function updateFormField()
{	
	var dfd = $.Deferred();
	var frm = $("#frmField");
	var win = $("#formFieldWin");
	var fieldID = parseInt($("#fieldID", frm).val());
	
	$.showLoading(fieldID ? "Saving Form Field" : "Adding new Form Field");
	
	$.post("admin/ajax/customForms", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			
			frm.updateHelper("reset");
			
			win.dialog("close");
			
			getFormFields(output.formID);
			
			dfd.resolve();
		}		
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();
}
function getFieldsets()
{
	return $.getJSON("admin/ajax/customForms", { action: "getFieldsets" }).then(function(output)
	{
		if (!$.ajaxError(output))
		{
			$("[name='field[fieldsetID]']").html(output.content);
		}
	});
}
function getFieldsetAdd()
{
	$.showLoading("Loading Fieldset Interface");
	
	$.getJSON("admin/ajax/customForms", { action: "getFieldsetAdd" }).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$(output.content).appendTo("body");
		}
	});
}
function setFieldStat(fieldID, stat)
{
	var obj = $(this);
	
	$.showLoading("Saving Field Preferences");
	
	$.getJSON("admin/ajax/customForms", 
	{
		action: "setFieldStat", 
		fieldID: fieldID, 
		typeID: stat, 
		val: obj.is(":checked") ? 1 : 0 
	}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
		}
	});
}
function deleteFormField(fieldID)
{
	$.jqConfirm("Are you sure you want to remove this form field? This will remove ALL submitted responses to this field, and *CANNOT* be undone!", function()
	{
		$.showLoading("Removing Form Field");
		
		$.getJSON("admin/ajax/customForms", { action: "deleteFormField", fieldID: fieldID }).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
				
				$("#formFieldWin").dialog("close");
				
				$("tr[data-id='" + fieldID + "']").fadeOut("normal", function() { $(this).remove(); });
			}
		});	
	});
}
function addFieldset()
{
	var frm = $("#frmFieldset");
	var win = $("#fieldsetWin");
	
	$.showLoading("Adding Fieldset");
	
	$.post("admin/ajax/customForms", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			
			win.dialog("close");
			
			getFieldsets().then(function()
			{
				$("[name='field[fieldsetID]']").val(output.fieldsetID);
			});
		}
	});
}
function toggleFieldType()
{	
	var frm = $("#frmField")
	var typeID = parseInt($("[name='field[typeID]']", frm).val());	
	
	var verCtl = $("[name='field[verification]']", frm).fadeOut("fast");
	var verLbl = verCtl.prev().fadeOut("fast");
	
	var extraCtl = $("[name='field[extraData]']", frm).fadeOut("fast");	
	var extraLbl = extraCtl.prev().fadeOut("fast");
	
	var descCtl = $("[name='field[exampleText]']", frm);
	var descLbl = descCtl.prev();
	
	// TODO: Need to consider this in the field list later
	//$("#req_" + fieldID).enable();
	
	switch(typeID)
	{
		case 1:
			verCtl.fadeIn();
			verLbl.fadeIn();
			
			toggleVerifyCustom();
			
			break;
		case 3:
			extraCtl.fadeIn();
			extraLbl.fadeIn().html("Comma Separated List:");
			
			break;
		case 4:
			//$("#req_" + fieldID).attr("checked","").disable();			
			break;			
		case 5:
			descCtl.fadeOut();
			descLbl.fadeOut();
			
			return;
	}
	
	descCtl.fadeIn();
	descLbl.fadeIn();
}
function toggleVerifyCustom()
{
	var frm = $("#frmField")
	var typeID = parseInt($("[name='field[verification]']", frm).val());
	
	var extraCtl = $("[name='field[extraData]']", frm).fadeOut("fast");	
	var extraLbl = extraCtl.prev().fadeOut("fast");
	
	switch(typeID)
	{
		case -1:
			extraLbl.html("Custom RegEx (Pearl):");
			break;
		case 1:
			extraLbl.html("Input Min,Max (ex: 2,5):");
			break;			
		default:		
			return;
	}
	
	extraCtl.fadeIn();
	extraLbl.fadeIn();	
}
function getSubmissionsWin(formID)
{
	$.showLoading("Loading Submissions Interface");
	
	$.getJSON("admin/ajax/customForms", {action: "getSubmissionsWin", formID: formID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$(output.content).appendTo("body");
			getSubmissions(formID);			
		}
	});
}
function getSubmissions(formID)
{
	var cont = $("#submissionsDiv");
	var filterObj = $("#submissionsFilter");	
	var ajaxOpts = { action: "getSubmissions", formID: formID };	
	
	$.showLoading("Loading Submissions");	
	
	$.getJSON("admin/ajax/customForms", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content);
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		}
	});
}
function viewSubmission(subID)
{
	$.showLoading("Loading Submission");
	
	$.getJSON("admin/ajax/customForms", { action: "viewSubmission", subID: subID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$(output.content).appendTo("body");
		}
	});
}
function deleteSubmission(subID)
{
	$.jqConfirm("Are you SURE you want to permanently remove this form submission and all responses?", function()
	{
		$.showLoading("Deleting Submission");
		
		$.getJSON("admin/ajax/customForms", { action: "deleteSubmission", subID: subID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
				getSubmissions(output.formID);
			}
		});
	});
}