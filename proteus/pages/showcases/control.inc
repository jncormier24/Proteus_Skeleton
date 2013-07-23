<?php 
	
	switch($action)
	{ 
		default:
			showShowcases();
			break;
	}
	
function showShowcases()
{
	global $config, $g_sec;
	
	$a = new adminPage("Manage Showcases");
	$a->addPageModule(new jqTableSortModule());	
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new customContentModule());
	$a->addPageModule(new jqPopupMenuModule());
	$a->addPageModule(new jqUpdateHelperModule());
	$a->addPageModule(new jqMaskedInputModule());
	$a->addPageModule(new jqUploadModule());
	$a->addPageModule(new jqFilterHelperModule());
	$a->addPageModule(new thickboxModule());
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->hasAccess(permissions::$manage_showcases, permissions::read)) $g_sec->security_error();	
	
	$iBlock = new panelBlock("", "Current Custom Showcases");
	$iBlock->id = "showcasesBlock";		
	
	$tblFilter = new tableFilter("showcasesFilter", "#showcasesTable", "getShowcases");			
	$tblFilter->setPaging(25);	
	
	$tblFilter->addCustomFilter("status", tableFilter::filter_dropdown, "Status:", array("Active"=>0, "Inactive"=>1));
	
	$dv = new phpDIV();
	$dv->id = "showcases";
	
	$iBlock->addControl($tblFilter);
	$iBlock->addControl($dv);	

	if ($g_sec->hasAccess(permissions::$manage_showcases, permissions::insert))
	{
		$mnu = new jqPopupMenu();	
		
		$ctl = $mnu->addMenuItem("", "New Showcase", "", "ui-icon-plusthick");
		$ctl->onclick = "getShowcaseEdit(0)";
	
		$iBlock->header->addControl($mnu);	
	}
	
	$a->addControl($iBlock);
	$a->render();
}
?>