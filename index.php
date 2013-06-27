<?php 
	include("includes/template.inc");
	
	$config["pageIndex"] = $config["params"][0];
	if (!$config["pageIndex"]) $config["pageIndex"] = "index";
	
	array_shift($config["params"]);
	
	include("pages/{$config["pageIndex"]}/model.inc");
	include("pages/{$config["pageIndex"]}/control.inc");
?>