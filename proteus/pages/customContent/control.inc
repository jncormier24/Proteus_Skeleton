<?php
	switch($action)
	{ 
		default:
			showContentPages();
			break;
	}
	
function showContentPages()
{
	global $config, $g_sec;
	
	$a = new adminPage('Manage Content Pages');
	$a->addPageModule(new jqTableSortModule());	
	$a->addPageModule(new jqUploadModule());
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new customContentModule());
	$a->addPageModule(new jqPopupMenuModule());
	$a->addPageModule(new jqUpdateHelperModule());
	
	
	$a->addPageModule(new jqMaskedInputModule());
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->hasAccess(permissions::$manage_custom_content, permissions::read)) $g_sec->security_error();	
	
	$iBlock = new panelBlock("", "Current Custom Content Pages");
	$iBlock->id = "contentPages";	
	
	$iBlock->addControl(customContentData::getContentContainer(proteus_core::$module_customContent["siteType"], 0));	

	if ($g_sec->hasAccess(permissions::$manage_custom_content, permissions::insert))
	{
		$mnu = new jqPopupMenu();	
		
		$ctl = $mnu->addMenuItem("", "New Content Page", "", "ui-icon-plusthick");
		$ctl->onclick = "getContentWin(0, 0, ".proteus_core::$module_customContent["siteType"].")";
	
		$iBlock->header->addControl($mnu);	
	}
	
	$a->addControl($iBlock);
	$a->render();
}
?>