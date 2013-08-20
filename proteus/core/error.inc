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
function debug($obj, $throw=false)
{
	// Just a shortcut handler to print raw debug information out
	$content = nl2br(print_r($obj, true));
	
	if ($throw) throw new Exception($content);
	die($content);
}
function debug_a($obj)
{
	debug($obj, true);
}
?>