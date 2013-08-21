<?php
 
	showCategories();
	
function showCategories()
{
	$a = new adminPage("Manage Custom Categories");
	$a->addPageModule(new jqUploadModule());
	$a->addPageModule(new jqFilterHelperModule());
	$a->addPageModule(new jqTableSortModule());	
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new jqUpdateHelperModule());
	$a->addPageModule(new customContentModule());	
	$a->addPageModule(new jqPopupMenuModule());	
	$a->addPageModule(new jqCheckboxModule());
	$a->addPageModule(new thickboxModule());
	
	// There could probably be a more elegant way to do this; include the view script from the customForms so we can access submissions
	// via the context menu that appears when an item has a Custom Form Feature. Same applies for Showcases (Context Menu)
	$a->addScriptObject(new javascriptControl("", "proteus/pages/customForms/scripts/view.js"));
	$a->addStylesheetObject(new stylesheetControl("proteus/pages/customForms/styles/view.css"));
	
	$a->addScriptObject(new javascriptControl("", "proteus/pages/showcases/scripts/view.js"));
	$a->addStylesheetObject(new stylesheetControl("proteus/pages/showcases/styles/view.css"));
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->is(permissions::$manage_category_types) &&
		!$g_sec->is(permissions::$modify_category_type)) $g_sec->security_error();	
	
	$tblFilter = new tableFilter("categoriesFilter", "#categoriesTable", "getCategories");			
	$tblFilter->setPaging(25);	
	
	$tblFilter->addCustomFilter("status", tableFilter::filter_dropdown, "Status:", array("Active"=>0, "Inactive"=>1));
	
	$dv = new phpDIV();
	$dv->id = "categoriesContainer";
		 
	$iBlock = new panelBlock("", "Current Custom Categories");
	
	if ($g_sec->hasAccess(permissions::$manage_category_types, permissions::insert))
	{
		$mnu = new jqPopupMenu();
		
		$ctl = $mnu->addMenuItem("", "Add a new Custom Category", "", "ui-icon-plusthick");
		$ctl->onclick = "getCategoryEdit(0)";
	
		$iBlock->header->addControl($mnu);
	}
	
	$iBlock->addControl($tblFilter);
	$iBlock->addControl($dv);		
		
	$a->addControl($iBlock);
	
	$a->render();
}
?>