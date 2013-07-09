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
	
	//$a->addPageModule(new jqWysiwygModule());	
	//$a->addPageModule(new tabPageModule());
	//$a->addPageModule(new jqTableSortModule());
	//$a->addPageModule(new jqDatePickerModule());	
	
	//$a->addScriptObject(new javaControl("", "includes/js/subscribers.js"));
	
	//$a->addScriptObject(new javaControl("", "includes/js/customContent.js"));
	//$a->addScriptObject(new javaControl("", "includes/js/calendar.js"));
	//$a->addStyleSheet("../admin/$config[theme]/customContent.css");
	//$a->addStyleSheet("../admin/$config[theme]/calendar.css");
	
	if (!$g_sec->hasAccess(permissions::$manage_municipal_entities, permissions::read) && !$g_sec->hasAccess(permissions::$municipal_entity, permissions::read))
	{
		$g_sec->security_error();	
	}
	
	$eDiv = new phpDIV();
	$eDiv->id = "entities";
	
	$eBlock = new panelBlock("", "Town Hall: Municipal Entities");
	$eBlock->addControl($eDiv);
	$eBlock->addControl(new jqLoader("getEntities(0)"));	
	
	if ($g_sec->hasAccess(permissions::$manage_municipal_entities, permissions::write))
	{
		$btnSettings = new jqUI_button("btnSettings", "Municipal Settings", "", "ui-icon-wrench");
		$btnSettings->onclick = "getSettingsEdit();";
		$btnSettings->breakAfter = true;
		
		$eBlock->addWrapperControl($btnSettings);
	}	
	
	$a->addControl($eBlock);
	$a->render();
}
?>