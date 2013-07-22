function initConfig(disableControls)
{
	var frm = $("#frmConfig");
	var mods = $("#modules");
	
	frm.updateHelper(saveConfig, {autoSave: false, disableControls: disableControls});
	
	$("div.mod_select > div", mods).buttonset();	
	$("input[type='radio']", mods).change(function()
	{
		$.showLoading("Saving Module Configuration");
		
		$.getJSON("admin/ajax/config", { 
			action: "setModStat", 
			siteType: $(this).parents(".mod_select").attr("data-sitetype"), 
			stat: $(this).val() 
		}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
			}
		});
	});
}
function saveConfig()
{
	$.showLoading("Saving Configuration Options");
	
	return $.post("admin/ajax/config", $("#frmConfig").serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
		}
	});
}