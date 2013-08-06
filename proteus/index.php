<?php 
	/** Admin Console Bootstrapper **/
	error_reporting(E_ALL ^ E_NOTICE ^ E_STRICT);
	
	ini_set("ignore_repeated_errors", "1");
	ini_set("session.use_trans_sid", false);	
	
	$config["baseAppDir"] = getcwd()."/";
	
	// Pop off the library dir to get the main baseAppDir
	$tmpArr = explode("/", rtrim($config[baseAppDir], "/"));
	$libraryDir = array_pop($tmpArr);
	
	// Override the default value so we can have an accurate base for the admin console pages structure
	$config["pageDir"] = "$libraryDir/pages";
	
	// Remap the config setting, this will prevent framework.inc from reloading a new baseAppDir
	$config["baseAppDir"] = implode("/", $tmpArr)."/";
	
	// Include the main site settings file so we can get the DB access information
	include($config["baseAppDir"]."includes/settings.php");
	
	// Remove this config setting so that we can manually bind it below for the Proteus Admin interface
	unset($config["custom_error_handler"]);
	
	include("framework.inc");	
	include("pages/common/pageClass.inc");
	
	$config["custom_error_handler"] = "admin_error_handler";
	
	// Set the error handler to the internal Proteus admin console error handler
	set_error_handler($config["custom_error_handler"]);
	
	$d = new DAL();	
	if (!$d->qryCount("show tables like 'config'"))
	{			
		proteus_core::syncAdminModules();		
	}

	// Configure the Admin specific locations - physical directory, and scripturl. This allows differentiation within administration modules
	// to manipulate assets on the main site	
	
	// Admin Base App Dir (not needed?)
	// $config["adminAppDir"] = $config[baseAppDir];
	
	// Admin Script Url - use the library dir popped off the baseAppDir above (not needed?)	
	// $config[adminScriptUrl] = $config[scripturl].$libraryDir."/";
	
	// Pop off the library folder to get the main baseAppDir
	/* $tmpArr = explode("/", rtrim($config[scripturl], "/"));
	array_pop($tmpArr);
	
	// Remap the config setting	
	$config[scripturl] = implode("/", $tmpArr)."/"; */
	
	// Override any default settings for the include folders
	$config["auto_include_folders"] = Array("scripts"=>Array("js"), "styles"=>Array("css"));	
	
	// Save the value for theme in a custom config entry so that admin scripts can enumerate files (templates) in front-end theme (emails, etc)
	$config["frontend_theme"] = $config["theme"];
	
	// Override the default theme
	$config["theme"] = "default";
	
	// Shift away the "admin" from the url so we can get the admin-relative page index
	array_shift($config["params"]);
	
	// Set the pageIndex
	$config["pageIndex"] = array_shift($config["params"]);	
	
	switch($config["pageIndex"])
	{
		case "module":
			// Get the page index to know where to route the ajax request
			$config["pageIndex"] = array_shift($config["params"]);
			if (!$config["pageIndex"]) die("No request specified.");
			
			include_once("modules/{$config["pageIndex"]}/model.inc");
			include("modules/{$config["pageIndex"]}/ajax/control.ajax");
			
			break;
		case "ajax":
			// Get the page index to know where to route the ajax request
			$config["pageIndex"] = array_shift($config["params"]);
			if (!$config["pageIndex"]) die("No request specified.");

			// By default, a pageIndex URL can be given, and control.ajax will be assumed. For subpages, or other reasons of needing 
			// multiple .ajax files, add the ajax filename to the end of the /ajax request; i.e., ajax/index/file.ajax
			$reqFile = $config["params"][count($config["params"])-1];
			
			$controlFile = preg_match("/\.ajax$/i", $reqFile) ? $reqFile : "control.ajax";
			
			@include("pages/{$config["pageIndex"]}/model.inc");
			include("pages/{$config["pageIndex"]}/ajax/$controlFile");
							
			break;
		default:
			if (!$config["pageIndex"]) $config["pageIndex"] = "index";
			
			$controlFile = "pages/{$config["pageIndex"]}/control.inc";
			$customControlFile = "../pages/admin/{$config["pageIndex"]}/control.inc";
			
			if (!$config["useCustomAdminIndex"] && file_exists($controlFile))
			{
				@include("pages/{$config["pageIndex"]}/model.inc");
				include($controlFile);
			}
			elseif ($config["useCustomAdmin"] && file_exists($customControlFile))
			{
				// Set the pageDir tp the frontend custom admin location so that scripts/css will be included automatically
				$config["pageDir"] = "pages/admin";
				
				// Model files are required here
				include("../pages/admin/model.inc");
				include("../pages/admin/{$config["pageIndex"]}/model.inc");
				
				include($customControlFile);
			}
			else
			{				
				header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found"); 
				include($config["baseAppDir"]."404.php");			
			}
	}
	
	exit(0);
?>