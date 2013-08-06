<?php
/*
 * Sample Custom Admin Page
 * 
 * This model is included when the default location within an /admin/ url cannot be found. It defines the specifics of this custom admin page
 * by implementing the customAdminPage interface.
 * 
 */
class customAdminSample_core implements customAdminPage
{
	// Set this to TRUE to disable this page from being accessible in the administration console
	const disabled = false;
	
	public static function checkMenuAccess(security $sec)
	{
		/* 
		 * customPermissions is defined in /pages/admin/model.inc - provides a unified model for all custom admin pages
		 * Checks ::is() since read access implies menu availability
		 */
		return $sec->is(customPermissions::$manage_custom_items) ||
			   $sec->is(customPermissions::$modify_custom_item);
	}
	public static function getCaption()
	{
		return "Manage Custom Items";
	}
	public static function getIconClass()
	{
		return "ui-icon-tag";
	}	
	public static function getMenuKey()
	{
		// Can return a plain text key, or an array to add a new menu dropdown; caption, key, iconclass (for re-use within other admin pages)
		return array("Custom Admin", "custom_admin", "ui-icon-lightbulb");
	}
}
?>