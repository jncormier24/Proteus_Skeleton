<?php 
	
	// The version of the installed Framework
	$config["version"] = "4.0";
	 
	// Get the base application directory	
	if (!$config["baseAppDir"]) $config["baseAppDir"] = getcwd()."/";	
	
	// Get the library installation directory
	$config["libraryDir"] = $config["baseAppDir"]."proteus/"; //dirname(__FILE__)."/";
	
	// This will be overidden by the administration panel
	if( (!$config["pageDir"])) $config["pageDir"] = "pages";
	
	// Set the bundled UI theme
	$config["jquery_ui_theme"] = "overcast";
	
	if ($config["application_root"])
	{		
		// Get the script URL		
		$tmp = explode("/", $config["application_root"]);				
		
		$shiftCt = count($tmp);		
	}
	
	$config["scripturl"] = "http".($_SERVER["HTTPS"] == "on" ? "s" : "")."://".$_SERVER["HTTP_HOST"]."/".$config["application_root"];	

	include($config["libraryDir"]."core/controls.inc");
	include($config["libraryDir"]."core/parser.inc");			
	include($config["libraryDir"]."core/error.inc");	
	include($config["libraryDir"]."core/cache.inc");	
	
	include($config["libraryDir"]."core/dal.inc");
	include($config["libraryDir"]."core/db_sync.inc");		
	include($config["libraryDir"]."core/security.inc");		
		
	include($config["libraryDir"]."core/core.inc");
	include($config["libraryDir"]."core/pageControls.inc");
	include($config["libraryDir"]."core/pageModules.inc");
	include($config["libraryDir"]."core/helpers.inc");
	include($config["libraryDir"]."core/mail.inc");	
	
	// Use PHPMailer project for sending emails (https://github.com/PHPMailer/PHPMailer)
	include($config["libraryDir"]."core/phpMailer/class.phpmailer.php");
	
	// Find all the required modules (required=true) and include their models
	// Some modules will still require adding a pageModule to access CSS/JS functionality, this is just to include the object models
	$mods = proteus_core::getModulesArray();
	
	foreach($mods as $mod)
	{		
		if ($mod["required"] && $mod["location"]) proteus_core::includeModule($mod);		
	}
	
	// Store the URI in array format for processing
	$config["params"] = proteus_core::processURI($shiftCt);	
	
	define("START_TIME", proteus_core::getMicroTime());
	
	if ($config["custom_error_handler"]) set_error_handler($config["custom_error_handler"]);	
	if ($config["use_zlib_compression"]) ini_set("zlib.output_compression","1");	
	
	// Set the server timezone. This is the same zone used in every connection to set the timezone. (keeps cloud servers in sync)
	date_default_timezone_set($config["server_timezone"]);	
		
	//Create globals
	//Get VARS must be alpha-numeric only!
	$cur = current($_GET);
	while(!($cur===false))
	{
		if (is_array($cur))
		{
			array_walk_rec($cur, 'walkEreg');
			reset($cur);
		}
		else
		{
			walkEreg($cur);
		}
	
		$key = key($_GET);
		$$key = $cur;
	
		$cur = next($_GET);
	}
	
	//Override get vars with post vars
	$cur = current($_POST);
	while(!($cur===false))
	{
		if (is_array($cur))
		{
			array_walk_rec($cur, 'makeSafe');
			reset($cur);
		}
		else
		{
			makeSafe($cur);
		}
	
		$key = key($_POST);
		$$key = $cur;
	
		$cur = next($_POST);
	}
	
	//application/json, text/javascript filtering - runs after the includes so that $action can be properly allocated
	if (preg_match("'/ajax|modules/'i", $_SERVER['PHP_SELF']) && isset($action) && !preg_match("/^application\/json\, text\/javascript/i", $_SERVER['HTTP_ACCEPT']))
	{
		//Silently discard, but allow for image upload requests (rework the plugin later to not need this!!)
		if (!preg_match("/image\//i", $_SERVER[HTTP_ACCEPT]) && !preg_match("/upload/i", $action))
		{			
			die('Unauthorized');
		}
	}
	
	session_start();		

function walkEreg(&$str)
{
	$str = preg_replace("/[^a-zA-Z0-9\.\-\_\/]/", "", $str);
}
function makeSafe(&$str)
{
	global $config;

	//Make sure string is mySQL safe...
	if ($config[dbLink])
	{
		$str = mysqli_real_escape_string($config[dbLink], $str);
	}	
}
function array_walk_rec(&$input, $funcname, $userdata = "")
{
	if (!is_callable($funcname))
	{
		return false;
	}

	if (!is_array($input))
	{
		return false;
	}

	foreach ($input AS $key => $value)
	{
		if (is_array($input[$key]))
		{
			array_walk_rec($input[$key], $funcname, $userdata);
		}
		else
		{
			if (!empty($userdata))
			{
				$funcname($value, $key, $userdata);
			}
			else
			{
				$funcname($value, $key);
			}

			$input[$key] = $value;
		}
	}

	return true;
}
?>