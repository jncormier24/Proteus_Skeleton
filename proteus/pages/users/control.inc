<?php 
	switch($action)
	{
		default:
			showHome();
	}
	
function showHome()
{
	$a = new adminPage("Manage User Accounts");
	$a->addPageModule(new jqFilterHelperModule());
	$a->addPageModule(new jqPopupMenuModule());
	$a->addPageModule(new jqUpdateHelperModule());
	$a->addPageModule(new commentsModule());
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->hasAccess(permissions::$manage_users, permissions::read)) $g_sec->security_error();
	
	$uBlock = new panelBlock("", "Manage User Accounts");
	$uBlock->addClass("adminBlock");
	//$uBlock->addControl(new phpParagraph("Use the interface below to modify or add users, and grant individual access to functionality."));
		
	$tblFilter = new tableFilter("usersFilter", "#usersTable", "function() { getUsers(); }");
	$tblFilter->addSortFilter("fullName", "th.name", "asc");
	$tblFilter->addSortFilter("login", "th.login");	
	$tblFilter->setPaging(25);
	
	$tblFilter->addColumnFilter("fullName", tableFilter::filter_text_having, "th.name");	
	$tblFilter->addColumnFilter("login", tableFilter::filter_text, "th.login");		
	
	$tblFilter->addCustomFilter("status", tableFilter::filter_dropdown, "User Status:", array("Active"=>0, "Inactive"=>1));
	$tblFilter->addCustomFilter("email", tableFilter::filter_text, "Email:");
	
	$dv = new phpDIV();
	$dv->id = "users";		
	
	if ($g_sec->hasAccess(permissions::$manage_users, permissions::insert))
	{
		$mnu = new jqPopupMenu();	
		
		$ctl = $mnu->addMenuItem("", "Add new User", "", "ui-icon-plusthick");
		$ctl->onclick = "showEditUser()";
	
		$uBlock->header->addControl($mnu);	
	}
	
	$uBlock->addControl($tblFilter);
	$uBlock->addControl($dv);
	
	$a->addControl($uBlock);	
	$a->render();
}
?>