<?php 
	include("includes/template.inc");
	
	// Set the pageIndex
	$config["pageIndex"] = array_shift($config["params"]);
	
	switch($config["pageIndex"])
	{
		/* case "admin":
			// Get the page index to know where to route the ajax request
			$config["pageIndex"] = array_shift($config["params"]);
			if (!$config["pageIndex"]) die("No request specified.");
				
			@include("proteus/admin/{$config["pageIndex"]}/model.inc");
			include("proteus/admin/{$config["pageIndex"]}/control.inc");
				
			break; */
		case "ajax":
			// Get the page index to know where to route the ajax request
			$config["pageIndex"] = array_shift($config["params"]);
			if (!$config["pageIndex"]) die("No request specified.");
			
			include("pages/{$config["pageIndex"]}/control.ajax");
			
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