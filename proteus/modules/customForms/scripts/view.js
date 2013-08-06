function updateToken(formID)
{
	var frm = $("form[data-formid='" + formID + "']");
	var tokenCtl = $("input[name='fields[token]']", frm);
	
	$.getJSON("admin/module/customForms", { action: "getToken", formID: formID }).then(function(output)
	{
		if (!$.ajaxError(output))
		{
			tokenCtl.val(output.token);
		}		
	});	
}