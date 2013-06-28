<?php 
class pageModule
{
	public $requisiteModules = array();
	public $scriptObjects = array();
	public $styleSheets = array();
	public $rev;

	public function __construct()
	{

	}
	public function addScriptObject($javaControl)
	{
		global $config;

		/* if ($javaControl->src)
		{
			$javaControl->src = $config[scripturl].$javaControl->src;
		} */

		array_push($this->scriptObjects, $javaControl);
	}
	public function addStylesheet($stylesheet)
	{
		array_push($this->styleSheets, $stylesheet);
	}
	public function addRequisiteModule($module)
	{
		array_push($this->requisiteModules, $module);
	}
}
class baseHRefModule extends pageModule
{
	public function __construct($location="")
	{
		global $config;
		if (!$location) $location = $config[scripturl];
		
		$this->addScriptObject(new phpLiteral("<base href='$location' />\n"));
	}
}
class jQueryModule extends pageModule
{
	public function __construct()
	{
		global $config;

		$this->addScriptObject(new javascriptControl("", "proteus/jquery/core/jquery-1.10.1.min.js"));
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/core/jquery.core.supplement.js"));
	}
}
class jQueryUIModule extends pageModule
{
	public function __construct()
	{
		global $config;

		$this->addRequisiteModule(new jQueryModule());

		$this->addStyleSheet(new stylesheetControl("proteus/jquery/ui/smoothness/jquery-ui-1.10.3.custom.min.css"));
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/ui/jquery-ui-1.10.3.custom.min.js"));

		$this->addStyleSheet(new stylesheetControl("proteus/jquery/jgrowl/jquery.jgrowl.css"));
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/jgrowl/jquery.jgrowl.js"));
	}
}
?>