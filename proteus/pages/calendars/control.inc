<?php
 
	showCalendars();
	
function showCalendars()
{
	$a = new adminPage("Manage Calendars");
	$a->addPageModule(new jqFileInputModule());
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new calendarModule());
	$a->addPageModule(new jqPopupMenuModule());
	$a->addPageModule(new customContentModule());
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->hasAccess(permissions::$manage_calendars, permissions::read) &&
		!$g_sec->hasAccess(permissions::$modify_calendar, permissions::read, 0)) $g_sec->security_error();	
	
	$dv = new phpDIV();
	$dv->id = "calendarsContainer";
		 
	$iBlock = new panelBlock("", "Current Calendars");
	
	if ($g_sec->hasAccess(permissions::$manage_calendars, permissions::insert))
	{
		$mnu = new jqPopupMenu();
		
		$ctl = $mnu->addMenuItem("", "Add a new Calendar", "", "ui-icon-plusthick");
		$ctl->onclick = "getCalendarWin(0, 0, 0)";
	
		$iBlock->header->addControl($mnu);
	}
	
	$iBlock->addControl($dv);
	$iBlock->addControl(new jqLoader("getCalendars(0, 0)"));	
		
	$a->addControl($iBlock);
	
	$a->render();
}
?>