function getCategories()
{
	var cont = $("#categoriesContainer");
	var filterObj = $("#categoriesFilter");	
	var ajaxOpts = { action: "getCategories" };	
	
	$.showLoading("Loading Custom Categories");	
	
	$.getJSON("admin/ajax/customCategories", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate;
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		
			initTableSort();
		}
	});
}
function getCategoryEdit(catID)
{
	$.showLoading("Loading Custom Category Interface");
	
	$.getJSON("admin/ajax/customCategories", {action: "getCategoryEdit", catID: catID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$(output.content).appendTo("body");
			
			$.blockContainer();
			
			var win = $("#categoryWin");
			
			// Need to inialized redactor before initializing the updateHelper to check for dirty controls (accesses Redactor method "getEditor")
			$("textarea.wysiwyg", win).assetRedactor(2, catID);
			
			win.dialog("option", "close", function() 
			{  
				$.unblockContainer();						
				$(this).remove();
			}).find("#frmCategory").updateHelper(updateCategory, {closeConfirmOnly: true, autoSave: false, disableControls: output.disabled});
			
			if (catID)
			{
				//getFeatures(catID);
				//getExposure(catID);
			}
		}
	});
}
function updateCategory()
{
	
}