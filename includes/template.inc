<?php

	error_reporting(E_ALL ^ E_NOTICE ^ E_STRICT);
	
	ini_set("ignore_repeated_errors", "1");
	ini_set("session.use_trans_sid", false);	
	
	include("includes/settings.php");
	include("proteus/core/framework.inc");	
	
	include("includes/common.inc");

class proteusPage extends pageClass
{	
	public $navigation;
	public $header;
	public $footer;
	
	public function __construct($title, $template="layout.tpl")
	{
		// To use a custom jQuery/UI, omit the packaged pageModule and extend the pageModule class on a new object to include the libraries
		$this->addPageModule(new jQueryUIModule());
		
		parent::__construct($title, $template);
		
		$this->navigation = new site_navigation();
		$this->header = new site_header();
		$this->footer = new site_footer();		
	}
	public function render()
	{		
		// Application specific additions to template		
		$this->parser->pushParam("NAV_CONTENT", $this->navigation->render());
		$this->parser->pushParam("HEADER_CONTENT", $this->header->render());
		$this->parser->pushParam("FOOTER_CONTENT", $this->footer->render());
		
		// All pages using MVC architecture need this
		$this->addPageModule(new baseHRefModule());
		
		// Render parent
		parent::render();
	}
}
class site_navigation extends phpDIV
{	
	
}
class site_header extends phpDIV
{
	
}
class site_footer extends phpDIV
{
	
}
?>