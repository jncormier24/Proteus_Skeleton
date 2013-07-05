<?php
function error($errMess)
{	
	global $config;
	
	// This is so any buried "error" calls within ajax calls throw an exception instead of trying to print out an error
	// -- Borderline "hack" --	
	if (preg_match("/\.ajax$/i", $_SERVER['PHP_SELF'])) throw new Exception($errMess);
	
	if (!$config["custom_error_handler"]) die("** No custom error handler defined; rendering raw error message output ** <br />".$errMess);
	//call_user_func($config["custom_error_handler"], array(0, E_USER_ERROR, $errMess));
	
	$func = new ReflectionFunction($config["custom_error_handler"]);
	$func->invoke(E_USER_ERROR, $errMess);
	
	die($errMess);		
}
function debug_string_backtrace() 
{
        ob_start();
        
        debug_print_backtrace();
        $trace = ob_get_contents();
        
        ob_end_clean();

        // Remove first item from backtrace as it's this function which is redundant.
        $trace = preg_replace ('/^#0\s+' . __FUNCTION__ . "[^\n]*\n/", '', $trace, 1);

        // Renumber backtrace items.
        $trace = preg_replace ('/^#(\d+)/me', '\'#\' . ($1 - 1)', $trace);

        return $trace;
} 
function debug($errMess, $pageClassObj = '', $limitAdmin=true)
{
	global $g_sec;
	
	if ($g_sec->isAdmin || !$limitAdmin)
	{	
		if ($pageClassObj && method_exists($pageClassObj, "render"))
		{
			$dbg = $errMess."<div style='text-align: left;'><hr>Stack Trace:<br /><pre>".var_export(debug_backtrace(), true)."</pre><hr></div>";
			
			$pageClassObj->addControl(new phpLiteral($dbg));			
		}
		else
		{			
			print_r($errMess);
			echo "<hr>";			
			//No stack trace if the pageobject isn't defined (ie unplanned debug from error handler)
			//echo "Stack Trace:<br />".print_r(debug_backtrace())."<hr></span>";			
		}
	}
}
?>