<?php
class pageClass extends pageContent
{		
	public $parser;
	
	public $title;
	
	public $head_objects;
	public $stylesheet_objects;
	public $script_objects;
	public $post_script_objects;
	
	private $pageModules = array();
	private $renderedModules = array();
		
	public function __construct($title="Default Title", $template="layout.tpl")
	{
		global $config;
		
		$this->parser = new fileParser($template);
		$this->title = $title;		
		
		$this->head_objects = new pageContent();
		$this->stylesheet_objects = new pageContent();
		$this->script_objects = new pageContent();
		$this->post_script_objects = new pageContent();
	}	
	public function addStylesheetObject($ctl)
	{
		$this->stylesheet_objects->addControl($ctl);
	}
	public function addScriptObject($ctl)
	{
		$this->script_objects->addControl($ctl);
	}
	public function render()
	{			
		global $config; 
		
		$this->parser->pushParam("TITLE", $this->title);
		
		foreach($this->controls as $control)
		{
			$content .= $control->render();			
		}
		
		// Render the pageModules and all requisites before including the autoInclude items
		$this->renderPageModules();
		
		foreach($config["autoIncludeFolders"] as $folder=>$exts)
		{
			foreach($exts as $ext)
			{
				foreach(glob(getcwd()."/pages/".$config[pageIndex]."/$folder/*.$ext") as $file)
				{
					$baseFile = basename($file);
		
					$fLoc = "pages/$config[pageIndex]/$folder/$baseFile";
		
					switch($ext)
					{
						case "js":
							$this->addScriptObject(new javascriptControl("",$fLoc));
							break;
		
						case "css":
							$this->addStylesheetObject(new stylesheetControl($fLoc));
							break;
					}
				}
			}
		}
		
		$this->parser->pushParam("PAGE_CONTENT", $content);
		$this->parser->pushParam("HEAD_OBJECTS", $this->head_objects->render());
		$this->parser->pushParam("STYLESHEET_OBJECTS", $this->stylesheet_objects->render());
		$this->parser->pushParam("SCRIPT_OBJECTS", $this->script_objects->render());
		$this->parser->pushParam("POST_SCRIPT_OBJECTS", $this->post_script_objects->render());		
		
		echo $this->parser->render();
	}	
	public function addPageModule($module)
	{
		foreach($this->pageModules as $exModule)
		{
			if (get_class($module) == get_class($exModule)) return;
		}
		
		$this->pageModules[] = $module;
	}
	public function renderPageModules()
	{
		foreach($this->pageModules as $module)
		{
			$this->renderPageModule($module);
		}
	}
	function renderPageModule($module)
	{
		//Only allow a module to be rendered once...		
		if (in_array(get_class($module), $this->renderedModules)) return;
	
		//Call the initiate first before rendering anything else, if it exists
		if (method_exists($module, "initiate"))
		{
			$module->initiate();
		}
	
		foreach($module->requisiteModules as $req)
		{
			$this->renderPageModule($req);
		}
		foreach($module->scriptObjects as $sObj)
		{
			$this->addScriptObject($sObj);
		}
		foreach($module->styleSheets as $sSheet)
		{
			$this->addStylesheetObject($sSheet);
		}
	
		$this->renderedModules[] = get_class($module);
	}
	public function redirect($redirect="index.php")
	{
		session_write_close();
			
		//Instantly expire all pages with a notification entry...
		$this->setHeader("Pragma: no-cache");
		$this->setHeader("Cache-Control: no-cache, must-revalidate");
		$this->setHeader("Location: $redirect");
		
		$this->render();
	}
	public static function setNotify($message, $title='')
	{
		$_SESSION['updated_message'] = $message;
		$_SESSION['updated_title'] = $title;
	
		session_write_close();
	}
	public static function getNotify()
	{
		$arr[] = $_SESSION['updated_message'];
		$arr[] = $_SESSION['updated_title'];
	
		$_SESSION['updated_message'] = "";
		$_SESSION['updated_title'] = "";
	
		unset($_SESSION['updated_message']);
		unset($_SESSION['updated_title']);
	
		return $arr;
	}
}
?>