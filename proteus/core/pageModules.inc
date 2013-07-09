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
class jqAutoGrowModule extends pageModule
{
	public function __construct()
	{
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/jquery.autogrow.js"));
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

		$this->addStyleSheet(new stylesheetControl("proteus/jquery/ui/$config[jquery_ui_theme]/jquery-ui-1.10.3.custom.min.css"));
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/ui/jquery-ui-1.10.3.custom.min.js"));

		$this->addStyleSheet(new stylesheetControl("proteus/jquery/jgrowl/jquery.jgrowl.css"));
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/jgrowl/jquery.jgrowl.js"));
	}
}
class thickboxModule extends pageModule
{
	public function __construct()
	{
		global $config;		
		
		$this->addRequisiteModule(new jqueryUIModule());
		
		$this->addScriptObject(new javaControl("", "proteus/jquery/jquery.ui.thickbox.js"));
	}	
}
class jqFilterHelperModule extends pageModule 
{
	public function __construct()
	{	
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/filterHelper/jquery.filterHelper.js"));
		$this->addStylesheet(new stylesheetControl("proteus/jquery/filterHelper/jquery.filterHelper.css"));			
	}
}
class jqDatePickerModule extends pageModule
{
	// SWB 7/13: This module is deprecated in favor of setting options using the jqUI_datepicker object ::setOption()
	public function __construct($elements='.datePicker', $suppressInit=false)
	{
		/* $init = "$(initProteusDatePickers);
		
		function initProteusDatePickers() 
		{
			$('$elements').datepicker({showOn: 'button', buttonImage: 'proteus/jquery/ui/supplement/calendar.gif', buttonImageOnly: true}).attr('readonly','readonly');
		}";		
		
		$this->addRequisiteModule(new jQueryUIModule());
		if (!$suppressInit) $this->addScriptObject(new javascriptControl($init)); */		
	}	
}
class jqPopupMenuModule extends pageModule 
{
	public function __construct()
	{	
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/jquery.popupMenu.js"));			
	}
}
class jqUpdateHelperModule extends pageModule 
{
	public function __construct()
	{	
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/jquery.updateHelper.js"));			
	}
}
class jqMaskedInputModule extends pageModule
{
	public function __construct()
	{		
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/jquery.maskedinput-1.2.2.js"));				
	}	
	public static function setMask(&$webControl, $mask)
	{
		$loader = new jqLoader("$(\"[name='$webControl->name']\").setMask($mask);");
		$webControl->postContent .= $loader->render();			
	}
}
class jqUploadModule extends pageModule
{
	public function __construct()
	{						
		$this->addRequisiteModule(new jqFileInputModule());
		
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/uploads/ajaxfileupload.js"));				
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/uploads/jquery.form.js"));
	}	
}
class jqFileInputModule extends pageModule 
{
	public function __construct()
	{
		$this->addRequisiteModule(new jQueryModule());
		
		$this->addScriptObject(new javascriptControl("", "proteus/jquery/fileinput/jquery.fileinput.min.js"));
		$this->addStylesheet(new stylesheetControl("proteus/jquery/fileinput/fileinput.css"));
		
		$this->addScriptObject(new jqLoader("$('input[type=file]').fileinput();"));
	}
}
?>