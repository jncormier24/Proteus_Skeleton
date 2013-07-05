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
	$config["pageDir"] = "$libraryDir/admin";
	
	// Remap the config setting, this will prevent framework.inc from reloading a new baseAppDir
	$config["baseAppDir"] = implode("/", $tmpArr)."/";
	
	// Include the main site settings file so we can get the DB access information
	include($config["baseAppDir"]."includes/settings.php");
	
	// Remove this config setting so that we can manually bind it below for the Proteus Admin interface
	unset($config["custom_error_handler"]);
	
	include("framework.inc");	
	include("admin/pageClass.inc");
	
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
	
	// Override the default theme
	$config[theme] = "default";
	
	// Shift away the "admin" from the url so we can get the admin-relative page index
	array_shift($config["params"]);
	
	// Set the pageIndex
	$config["pageIndex"] = array_shift($config["params"]);	
	
	switch($config["pageIndex"])
	{
		case "ajax":
			// Get the page index to know where to route the ajax request
			$config["pageIndex"] = array_shift($config["params"]);
			if (!$config["pageIndex"]) die("No request specified.");
				
			include("admin/{$config["pageIndex"]}/control.ajax");
				
			break;
		default:
			if (!$config["pageIndex"]) $config["pageIndex"] = "index";
				
			@include("admin/{$config["pageIndex"]}/model.inc");
			
			$controlFile = "admin/{$config["pageIndex"]}/control.inc";
			if (file_exists($controlFile))
			{
				include($controlFile);
			}
			else
			{
				header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found"); 
				include($config[baseAppDir]."404.php");			
			}
	}
	
	exit(0);
?>