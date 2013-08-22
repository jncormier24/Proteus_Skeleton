<?php
class pageClass extends pageContent
{		
	public $parser;
	
	public $title;
	
	public $head_objects;
	public $stylesheet_objects;
	public $script_objects;
	public $post_script_objects;
	
	public $headers = array();
	
	private $pageModules = array();
	private $renderedModules = array();
	
	public $authUser;
		
	public function __construct($title="Default Title", $templateFile="")
	{
		global $config;
		
		if (!strlen($templateFile))
		{
			$templateFile = "themes/".$config["theme"]."/layout.tpl";			
		}
		
		$this->parser = new fileParser($templateFile);
		$this->title = $title;		
		
		$this->head_objects = new pageContent();
		$this->stylesheet_objects = new pageContent();
		$this->script_objects = new pageContent();
		$this->post_script_objects = new pageContent();
		
		// Attempt to load any logged-in user information
		try
		{
			$this->authUser = permissions::getAuthorizedUser();
			
			// Set the last access here because we only case about access to pages; not to ajax, etc.
			customSession::setLastAccess($this->authUser);
		}
		catch (Exception $ex)
		{
			// Catch any DB errors, etc.; have to stop here or a loop will be created (cannot use error())
			debug($ex);
		}
	}
	public function setCustomView($templateFile="view.tpl")
	{
		// Use this shortcut function to automatically include the specified template file within the page directory structure as the main
		// view template.		
		global $config;		
		
		$fileLoc = $config["pageDir"]."/{$config["pageIndex"]}/views/".$templateFile;
		
		$this->parser = new fileParser($fileLoc);
	}	
	public function addStylesheetObject($ctl)
	{
		$this->stylesheet_objects->addControl($ctl);
	}
	public function addScriptObject($ctl)
	{
		$this->script_objects->addControl($ctl);
	}
	public function addPostScriptObject($ctl)
	{
		$this->post_script_objects->addControl($ctl);
	}
	public function addHeadObject($webControl, $insert=false)
	{
		$webControl->postContent .= "\n";
	
		if ($insert)
		{
			$this->headObjects->insertControl($webControl);
		}
		else
		{
			$this->headObjects->addControl($webControl);
		}
	}
	public function addPageHeader($header)
	{
		$tArr = array(count($this->headers)=>$header);
		$this->headers = array_merge($this->headers, $tArr);
	}
	public function render()
	{			
		global $config; 
		
		foreach($this->headers as $hdr)
		{
			header($hdr);
		}
		
		$this->parser->pushParam("TITLE", $this->title);		
		
		// Render the pageModules and all requisites before including the autoInclude items
		$this->renderPageModules();
		
		/* foreach($config["auto_include_folders"] as $folder=>$exts)
		{
			foreach($exts as $ext)
			{				
				//echo getcwd()."/".$config["pageDir"]."/".$config["pageIndex"]."/$folder<br />";
				foreach(glob($config["baseAppDir"].$config["pageDir"]."/".$config["pageIndex"]."/$folder/*.$ext") as $file)
				{					
					$baseFile = basename($file);
		
					$fLoc = $config["pageDir"]."/".$config["pageIndex"]."/$folder/$baseFile";
		
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
		} */
		$pageBase = $config["pageDir"]."/".$config["pageIndex"]."/";
		$searchBase = $config["baseAppDir"].$pageBase;
		
		// Automatically include the view.css and view.js files for the page
		if (file_exists($searchBase."scripts/view.js")) $this->addScriptObject(new javascriptControl("", $pageBase."scripts/view.js"));
		if (file_exists($searchBase."styles/view.css")) $this->addStylesheetObject(new stylesheetControl($pageBase."styles/view.css"));
		
		$this->parser->pushParam("PAGE_CONTENT", parent::render());
		$this->parser->pushParam("HEAD_OBJECTS", $this->head_objects->render());
		$this->parser->pushParam("STYLESHEET_OBJECTS", $this->stylesheet_objects->render());
		$this->parser->pushParam("SCRIPT_OBJECTS", $this->script_objects->render());
		$this->parser->pushParam("POST_SCRIPT_OBJECTS", $this->post_script_objects->render());		
		
		echo $this->parser->render();
		ob_end_flush();
		
		exit(0);
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
		//Instantly expire all pages with a notification entry...
		$this->addPageHeader("Pragma: no-cache");
		$this->addPageHeader("Cache-Control: no-cache, must-revalidate");
		$this->addPageHeader("Location: $redirect");
		
		$this->render();
	}
	public static function setNotify($message, $title='')
	{
		$_SESSION['updated_message'] = $message;
		$_SESSION['updated_title'] = $title;
	
		session_write_close();
	}
	public static function clearNotify()
	{
		// Use this to clear any previously stored message (such as a master insert succeeded, but descendant data failed)
		pageClass::setNotify("");
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
	public function secure($loginPageIndex="login")
	{
		global $config;
		
		if (!$this->authUser) $this->authUser = permissions::getAuthorizedUser();
		
		if (!$this->authUser->id)
		{
			$ref = $_SERVER["SCRIPT_URL"];
			$this->redirect($config["scripturl"].$loginPageIndex."?ref=$ref");	
		}		
	}	
}
class panelBlock extends phpDIV
{		
	public $wrapperControls = array();
	public $footerControls = array();
	
	public $header;
	public $content;
	
	public $overrideClass = "";

	public function __construct($content="", $title="", $width="",$height="")
	{
		$this->header = new phpHeader(1, $title);
		
		$this->content = new phpDIV($content, "actual");
		
		$this->width = $width;
		$this->height = $height;

		parent::__construct($content, "panelBlockContainer");
	}
	public function addControl($ctl)
	{
		$this->content->addControl($ctl);
	}
	public function addWrapperControl($ctl)
	{
		array_push($this->wrapperControls, $ctl);
	}
	public function addFooterControl($ctl)
	{
		array_push($this->footerControls, $ctl);
	}
	public function render()
	{			
		if ($this->header) parent::addControl($this->header);
		
		//Add any wrapper controls
		foreach($this->wrapperControls as $ctl)
		{
			parent::addControl($ctl);
		}

		if ($this->overrideClass) $this->content->addClass($this->overrideClass);
		parent::addControl($this->content);		
		
		foreach($this->footerControls as $ctl)
		{
			parent::addControl($ctl);
		}

		return parent::render();		
	}
}
?>