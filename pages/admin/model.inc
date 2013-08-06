<?php
	/*
	 * Custom Admin Pages Model: This file is used to define security permissions, etc., for the custom administration pages located within
	 * this subfolder. For applications that need these security definitions outside the administration area, simply include this file
	 * from the fontend class (or individual page module)
	 * 
	 * All custom permission assignments must have an UI built to assign, since there's no way for the backend to know how to populate 
	 * id-associated entries that are outside the base Proteus admin modules
	 * 
	 * All folders in the /pages/admin/ folder are considered customAdminPages and should have the same directory structure as a regular page.
	 * When a request to /admin/pageX fails, the framework will attempt to fallback on the same pageIndex within the /pages/admin tree, and render
	 * that page/request in its place. All AJAX calls should be made directly to the .AJAX file and not the /ajax shortcut.
	 */ 

class customPermissions extends permission_base
{
	public static $manage_custom_items = array("key"=>"manage_custom_items", "max_perms"=>15, "description"=>"Manage Custom Items","standalone"=>true);
	public static $modify_custom_item = array("key"=>"modify_custom_item", "max_perms"=>15, "description"=>"Modify a Custom Item","standalone"=>false);
}
?>