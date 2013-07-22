<?php 
class adminPage extends pageClass
{
	public $navigation;
	public $header;
	public $footer;
	
	public function __construct($title, $secure = true)
	{
		$this->addPageModule(new jQueryUIModule());
		
		parent::__construct($title);
		
		$this->navigation = new admin_navigation($this->authUser);
		$this->header = new admin_header($this->authUser);
		$this->footer = new admin_footer();

		// Default to secure all admin pages
		if ($secure) $this->secure("admin/login");
	}
	public function render()
	{		
		global $config;
		
		// Application specific additions to template		
		$this->parser->pushParam("NAV_CONTENT", $this->navigation->render());
		$this->parser->pushParam("HEADER_CONTENT", $this->header->render());
		$this->parser->pushParam("FOOTER_CONTENT", $this->footer->render());		
		
		$this->addPostScriptObject(new javascriptControl("","proteus/pages/common/scripts/view.js"));
		
		// Render parent
		parent::render();
	}
}
class admin_navigation extends phpDIV
{	
	public $menu;
	
	public function __construct($secObj)
	{
		$this->id = "navContent";
		parent::__construct();
	
		$this->menu = new phpUnOrderedList();
	
		if ($secObj->id)
		{
			$conf = new siteConfig();
			
			$lnk = new jqUI_hyperlink("admin", "Admin Home", "", "ui-icon-home");
			$this->menu->addListItem(new htmlListItem($lnk->render()));			
			
			$lnk = new jqUI_hyperlink("javascript:void(0)", "Content", "", "ui-icon-star");
			$li = new htmlListItem($lnk->render());
			
			$ul = new phpUnOrderedList();
			
			if ($conf->moduleEnabled(proteus_core::$module_municipal["siteType"]) && $secObj->is(permissions::$manage_municipal_entities))
			{
				$lnk = new jqUI_hyperlink("admin/municipal", "Municipal", "", "ui-icon-flag");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}
			
			if ($secObj->is(permissions::$manage_custom_content))
			{	
				$lnk = new jqUI_hyperlink("admin/customContent", "Content Pages", "", "ui-icon-document");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}			
			
			if ($conf->moduleEnabled(proteus_core::$module_blogs["siteType"]) && ($secObj->is(permissions::$manage_blogs) || $secObj->is(permissions::$modify_blog_category)))
			{	
				$lnk = new jqUI_hyperlink("admin/blogs", "Blogs", "", "ui-icon-signal-diag");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}

			if ($conf->moduleEnabled(proteus_core::$module_showcases["siteType"]) && $secObj->is(permissions::$manage_showcases))
			{	
				$lnk = new jqUI_hyperlink("admin/showcases", "Custom Showcases", "", "ui-icon-image");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}
			
			$li->addControl($ul);
			
			if (count($ul->listItems)) $this->menu->addListItem($li);
			
			///////////////////////////////////////////
			
			$lnk = new jqUI_hyperlink("javascript:void(0)", "Management", "", "ui-icon-gear");
			$li = new htmlListItem($lnk->render());
			
			$ul = new phpUnOrderedList();
			
			if ($secObj->is(permissions::$site_configuration))
			{	
				$lnk = new jqUI_hyperlink("admin/config", "System Settings", "", "ui-icon-wrench");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}
			
			if ($conf->moduleEnabled(proteus_core::$module_subscribers["siteType"]) && $secObj->is(permissions::$manage_subscribers))
			{	
				$lnk = new jqUI_hyperlink("admin/subscribers", "Subscriptions Management", "", "ui-icon-mail-closed");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}
			
			if ($conf->moduleEnabled(proteus_core::$module_municipal["siteType"]) && ($secObj->is(permissions::$manage_calendars) || $secObj->is(permissions::$modify_calendar)))
			{	
				$lnk = new jqUI_hyperlink("admin/calendars", "Calendars & Events", "", "ui-icon-calendar");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}
			
			if ($secObj->is(permissions::$manage_users))
			{				
				$lnk = new jqUI_hyperlink("admin/users", "Manage Users", "", "ui-icon-person");
				$ul->addListItem(new htmlListItem($lnk->render()));
			}
			
			$li->addControl($ul);
			
			if (count($ul->listItems)) $this->menu->addListItem($li);
		}
		
		$lnk = new jqUI_hyperlink("index", "Back to Site", "", "ui-icon-seek-prev");
		$lnk->title = "Exit administration";
		
		$li = new htmlListItem($lnk->render());
		$li->class = "back";
		
		$this->menu->addListItem($li);
	
		$this->addControl($this->menu);
	}
}
class admin_header extends phpDIV
{
	public function __construct($secObj)
	{
		$this->id = "headerContent";
		parent::__construct();
	
		$this->addControl(new phpHeader(1, "Proteus IV Content Management Framework"));
		
		if ($secObj->id)
		{
			$dv = new phpDIV("", "authUser");
			
			$dv->addControl(new phpSpan("Logged in as: <strong>{$secObj->login}</strong>&nbsp;"));
			
			$lLink = new jqUI_hyperlink("javascript:logout('admin')", "&nbsp;", "logout", "ui-icon-power");
			$lLink->title = "Log out of your account";
			
			$dv->addControl($lLink);
			
			$this->addControl($dv);
		}
	}	
}
class admin_footer extends phpDIV
{
	public function __construct()
	{
		$this->id = "footerContent";
		parent::__construct();
	}
}
function admin_error_handler($errNo, $errStr, $errFile='', $errLine='', $type='')
{
	global $config;

	// error_reporting()==0 if the @ sign was used
	if ($errNo == E_NOTICE || error_reporting()==0) return;

	if($errNo == E_WARNING)
	{
		if ($config["debug_mode"])
		{
			debug("Error: $errNo 
			       File: $errFile 
			       Desc: $errStr
			       Line: $errLine
			
			       StackTrace: ".var_export(debug_backtrace(), true));
			return;			
		}
	}
	
	// A different template can easily be used here, or even another extended pageClass to display custom error information / UI
	$a = new adminPage("Error!!");

	$eCont = new phpDIV();
	$eCont->class = "errorContainer";

	$eCont->addControl(new phpHeader(1, "An error has occurred processing your request."));

	if ($errNo == 256)
	{
		$eCont->addControl(new phpParagraph($errStr));
	}
	else
	{
		if (!$config["debug_mode"] && $errNo != E_USER_ERROR && $config["debug_email_address"])
		{
			error_log("Error: $errNo \nFile: $errFile \n Desc: $errStr \nLine: $errLine \nStack Trace:".var_export(debug_backtrace(), true),1, $config["debug_email_address"]);
		}
		else
		{
			error_log($errStr);
		}

		if ($config["debug_mode"] || $errNo == E_USER_ERROR)
		{
			$eCont->addControl(new phpParagraph($errStr));
		}
		else
		{
			$eCont->addControl(new phpParagraph("We apologize for the inconvenience, but an unknown error has occured processing your request. Please try again, or contact the site administrator if the problem persists."));
		}
	}

	$cmdBack = new jqUI_button("", "Back to Previous Page", "", "ui-icon-seek-prev");
	$cmdBack->onclick = "history.back(1)";
	
	$eCont->addControl($cmdBack);

	$a->addControl($eCont);
	$a->render();	
}
class commentsModule extends pageModule 
{
	/*
	 * This module is intended for use in the administration panel only. For front-end use of comments, see the bundled 
	 * "root/modules/comments" which has template module for front-end comments
	 */
	public function __construct()
	{			
		$this->addRequisiteModule(new jqAutoGrowModule());
		$this->addScriptObject(new javascriptControl("", "proteus/modules/comments/scripts/view.js"));
		$this->addStylesheet(new stylesheetControl("proteus/modules/comments/styles/view.css"));			
	}	
}
class calendarModule extends pageModule 
{
	public function __construct()
	{			
		$this->addScriptObject(new javascriptControl("", "proteus/modules/calendars/scripts/view.js"));
		$this->addStylesheet(new stylesheetControl("proteus/modules/calendars/styles/view.css"));			
	}	
}
class customContentModule extends pageModule 
{
	public function __construct()
	{			
		$this->addScriptObject(new javascriptControl("", "proteus/modules/customContent/scripts/view.js"));
		$this->addStylesheet(new stylesheetControl("proteus/modules/customContent/styles/view.css"));			
	}	
}
?>