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
	
	$.getJSON("admin/ajax/customCategories", {action: "getCategoryEdit", categoryID: catID}).then(function(output)
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
				$("input.exp").checkbox();
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
			getCategories();
			
			frm.updateHelper("reset");
			
			win.dialog("close");
			
			if (!catID)
			{
				getCategoryEdit(output.catID);
			}
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();	
}
function deleteCategory(catID, curStat)
{
	var msg = curStat ? "Deleting Custom Category" : "Deactivating Custom Category";
	var warn = curStat ? "Are you SURE you want to permanently delete this Custom Category? This will remove all category items and CANNOT be undone!" : "Are you sure you want to deactivate this Custom Category? This will not delete any Category Item Data.";
	
	$.jqConfirm(warn, function()
	{
		$.showLoading(msg);
		
		$.getJSON("admin/ajax/customCategories", {action: "deleteCategory", categoryID: catID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
				getCategories();				
			}
		});
	});
}
function getCategoryFeatures(catID)
{
	var cont = $("#featuresContainer");
	
	cont.showLoading("Loading Category Features");
	
	$.getJSON("admin/ajax/customCategories", {action: "getCategoryFeatures", categoryID: catID}).then(function(output)
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
function toggleExposure(categoryID, linkedCategoryID)
{
	$.showLoading("Toggling Custom Category Key Exposure");
	
	$.getJSON("admin/ajax/customCategories", {action: "toggleExposure", categoryID: categoryID, linkedCategoryID: linkedCategoryID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
		}
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
function getCategoryItemEdit(itemID, categoryID)
{
	$.showLoading("Loading Category Item Interface");
	
	$.getJSON("admin/ajax/customCategories", {action: "getCategoryItemEdit", itemID: itemID, categoryID: categoryID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$.hideDialogs(true);
			
			$(output.content).appendTo("body");
			
			var win = $("#itemWin");
			
			// Need to inialized redactor before initializing the updateHelper to check for dirty controls (accesses Redactor method "getEditor")
			$("textarea.wysiwyg", win).assetRedactor(2, 0, itemID);
			
			win.dialog("option", "close", function() 
			{  
				$.showDialogs(true);						
				$(this).remove();
			}).find("#frmItem").updateHelper(updateCategoryItem, {closeConfirmOnly: true, autoSave: false, disableControls: output.disabled});
			
			if (itemID)
			{
				//getCategoryFeatures(catID);
				//getExposure(catID);
			}
		}
	});
}
function updateCategoryItem()
{
	var dfd = $.Deferred();
	var frm = $("#frmItem");
	var win = $("#itemWin");
	var itemID = parseInt($("#itemID", frm).val());
	
	$.showLoading(itemID ? "Saving Category Item" : "Adding new Category Item");
	
	$.post("admin/ajax/customCategories", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			getCategories();
			
			frm.updateHelper("reset");
			
			win.dialog("close");
			
			if (!itemID)
			{
				getCategoryEdit(output.itemID);
			}
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();	
}