<?php 
class adminPage extends pageClass
{
	public $navigation;
	public $header;
	public $footer;
	
	public function __construct($title)
	{
		$this->addPageModule(new jQueryUIModule());
		
		parent::__construct($title);
		
		$this->navigation = new site_navigation();
		$this->header = new site_header();
		$this->footer = new site_footer();		
	}
	public function render()
	{		
		global $config;
		
		// Application specific additions to template		
		$this->parser->pushParam("NAV_CONTENT", $this->navigation->render());
		$this->parser->pushParam("HEADER_CONTENT", $this->header->render());
		$this->parser->pushParam("FOOTER_CONTENT", $this->footer->render());
		
		// All pages using MVC architecture need this
		//$this->addPageModule(new baseHRefModule($config[adminScriptUrl]));
		
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