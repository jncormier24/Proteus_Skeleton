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
				getCategoryFeatures(catID);
				//getExposure(catID);
			}
		}
	});
}
function updateCategory()
{
	var dfd = $.Deferred();
	var frm = $("#frmCategory");
	var win = $("#categoryWin");
	var catID = parseInt($("#catID", frm).val());
	
	$.showLoading(catID ? "Saving Custom Category" : "Adding new Custom Category");
	
	$.post("admin/ajax/customCategories", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			
			frm.updateHelper("reset");
			
			win.dialog("close");
			
			if (!catID)
			{
				getFormEdit(output.catID);
			}
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();	
}
function getCategoryFeatures(catID)
{
	var cont = $("#featuresContainer");
	
	cont.showLoading("Loading Category Features");
	
	$.getJSON("admin/ajax/customCategories", {action: "getCategoryFeatures", catID: catID}).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{
			cont.html(output.content).tableRowAlternate();		
			initTableSort();
		}
	});
}
function getCategoryFeatureEdit(featureID, categoryID)
{
	var featureType = !featureID ? parseInt($("#featureType").val()) : 0;
	
	$.showLoading("Loading Category Feature Interface");
	
	$.getJSON("admin/ajax/customCategories", 
	{
		action: "getCategoryFeatureEdit", 
		categoryID: categoryID, 
		featureID: featureID, 
		featureType: featureType
	}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$(output.content).appendTo("body");
		}
	});
}
function updateCategoryFeature()
{
	var frm = $("#frmFeature");
	var featureID = parseInt($("#featureID", frm).val());
	
	$.showLoading(featureID ? "Saving Category Featyre" : "Adding Category Feature");
	
	$.post("admin/ajax/customCategories", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			
			$("#featureEditWin").dialog("close");
			
			getCategoryFeatures(output.categoryID);
		}
	});
}
function deleteCategoryFeature(featureID)
{
	$.jqConfirm("Are you sure you want to remove this Category Feature? This will remove ALL submitted data for this feature and *CANNOT* be undone!", function()
	{
		$.showLoading("Removing Category Feature");
		
		$.getJSON("admin/ajax/customCategories", { action: "deleteCategoryFeature", featureID: featureID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
				getCategoryFeatures(output.categoryID);
			}
		});
	});	
}
function getCategoryItemsWin(categoryID)
{
	$.showLoading("Loading Category Items Management Console");
	
	$.getJSON("admin/ajax/customCategories", {action: "getCategoryItemsWin", categoryID: categoryID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
			getCategoryItems(categoryID);
		}
	});
}
function getCategoryItems(categoryID)
{
	var cont = $("#itemsDiv");
	var filterObj = $("#itemsFilter");	
	var ajaxOpts = { action: "getCategoryItems", categoryID: categoryID };	
	
	$.showLoading("Loading Category Items");	
	
	$.getJSON("admin/ajax/customCategories", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
			
			initTableSort();			
		}
	});
}