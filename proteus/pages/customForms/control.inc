<?php
	showForms();

function showForms()
{
	global $config;
	
	$a = new adminPage("Manage Custom Forms");
	
	$a->addPageModule(new jqTableSortModule());	
	$a->addPageModule(new jqFilterHelperModule());
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new customContentModule());
	$a->addPageModule(new jqPopupMenuModule());
	$a->addPageModule(new jqUpdateHelperModule());
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->hasAccess(permissions::$manage_custom_forms, permissions::read) && 
		!$g_sec->is(permissions::$modify_custom_form)) $g_sec->security_error();	
	
	$iBlock = new panelBlock("", "Current Custom Forms");		
	
	$tblFilter = new tableFilter("formsFilter", "#formsTable", "getForms");			
	$tblFilter->setPaging(25);	
	
	$tblFilter->addCustomFilter("status", tableFilter::filter_dropdown, "Status:", array("Active"=>0, "Inactive"=>1));	
	
	$dv = new phpDIV();
	$dv->id = "customForms";		
	
	$iBlock->addControl($tblFilter);	
	$iBlock->addControl($dv);

	if ($g_sec->hasAccess(permissions::$manage_custom_forms, permissions::insert))
	{
		$mnu = new jqPopupMenu();	
		
		$ctl = $mnu->addMenuItem("", "New Custom Form", "", "ui-icon-plusthick");
		$ctl->onclick = "getFormEdit(0)";
	
		$iBlock->header->addControl($mnu);	
	}
	
	$a->addControl($iBlock);
	$a->render();
}
?>