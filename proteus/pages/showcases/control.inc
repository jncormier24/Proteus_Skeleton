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
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->hasAccess(permissions::$manage_showcases, permissions::read)) $g_sec->security_error();	
	
	$iBlock = new panelBlock("", "Current Custom Showcases");
	$iBlock->id = "showcasesBlock";
	$iBlock->addControl(new jqLoader("getShowcases()"));	
	
	$dv = new phpDIV();
	$dv->id = "showcases";
	
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