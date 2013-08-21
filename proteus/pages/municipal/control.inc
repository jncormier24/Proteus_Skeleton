<?php
	$g_sec = permissions::getAuthorizedUser();
		
	switch($action)
	{
		default:
			showEntities();
			break;
	}
	
function showEntities()
{
	global $g_sec, $config;
	
	$a = new adminPage("Modify Municipal Entities");
	$a->addPageModule(new calendarModule());
	$a->addPageModule(new customContentModule());
	$a->addPageModule(new jqPopupMenuModule());
	$a->addPageModule(new jqUploadModule());
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new jqFilterHelperModule());
	$a->addPageModule(new jqTableSortModule());
	$a->addPageModule(new subscriptionModule());
	$a->addPageModule(new jqUpdateHelperModule());	
	
	if (!$g_sec->hasAccess(permissions::$manage_municipal_entities, permissions::read) && !$g_sec->hasAccess(permissions::$municipal_entity, permissions::read))
	{
		$g_sec->security_error();	
	}
	
	$eDiv = new phpDIV();
	$eDiv->id = "entities";
	
	$eBlock = new panelBlock("", "Town Hall: Municipal Entities");
	$eBlock->id = "entitiesBlock";
	$eBlock->addControl($eDiv);
	$eBlock->addControl(new jqLoader("getEntities(0)"));	
	
	if ($g_sec->hasAccess(permissions::$manage_municipal_entities, permissions::write))
	{
		$mnu = new jqPopupMenu();
		
		$ctl = $mnu->addMenuItem("", "Municipal Settings", "", "ui-icon-wrench");
		$ctl->onclick = "getSettingsEdit();";		
		
		$eBlock->header->addControl($mnu);
	}	
	
	$a->addControl($eBlock);
	$a->render();
}
?>