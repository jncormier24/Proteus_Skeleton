<?php 
	include("includes/template.inc");
	
	// Set the pageIndex
	$config["pageIndex"] = array_shift($config["params"]);
	
	switch($config["pageIndex"])
	{		
		case "ajax":
			// Get the page index to know where to route the ajax request
			$config["pageIndex"] = array_shift($config["params"]);
			if (!$config["pageIndex"]) die("No request specified.");
			
			// By default, a pageIndex URL can be given, and control.ajax will be assumed. For subpages, or other reasons of needing 
			// multiple .ajax files, add the ajax filename to the end of the /ajax request; i.e., ajax/index/file.ajax			
			$reqFile = $config["params"][count($config["params"])-1];
			
			$controlFile = preg_match("/\.ajax$/i", $reqFile) ? $reqFile : "control.ajax";
			
			include("pages/{$config["pageIndex"]}/ajax/$controlFile");
			
			break;			
		default:	
			if (!$config["pageIndex"]) $config["pageIndex"] = "index";
			
			@include("pages/{$config["pageIndex"]}/model.inc");
			
			$controlFile = "pages/{$config["pageIndex"]}/control.inc";
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