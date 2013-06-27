<?php 
	
	// The version of the installed Framework
	$config["version"] = "4.0";
	
	// Get the library installation directory
	$config["libraryDir"] = realpath("/proteus")."/";
	die($config["libraryDir"]);
	// Get the base application directory	
	$config["baseAppDir"] = realpath(dirname(__FILE__)."/..")."/";
	
	// Get the script URL
	$tmp = explode("/", $_SERVER["SCRIPT_NAME"]);
	
	$shiftCt = count($tmp);
	
	if ($shiftCt)
	{
		array_pop($tmp);
		
		// Allows installation within subdirectories
		$urlBase = implode("/", $tmp);
	}
	
	$config["scripturl"] = "http".($_SERVER['HTTPS'] == "on" ? "s" : "")."://".$_SERVER['HTTP_HOST'].$urlBase."/";
	
	// Store the URI in array format for processing
	$config["params"] = proteus_core::processURI($shiftCt-1);

	include("proteus/core/controls.inc");
	include("proteus/core/parser.inc");
	include("proteus/core/pageModules.inc");
	include("proteus/core/core.inc");	
	
	if ($config["dbhost"]) include("proteus/core/dal.inc");
	
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