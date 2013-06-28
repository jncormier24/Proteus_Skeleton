<?php
function dbError($errMess)
{
	global $config;
	
	$msg = "Database Error Occurred: <br>".$errMess;
	
	if (!$config[debugMode])
	{
		//error_log("Site: {$_SERVER['PHP_SELF']}\n\n".$errMess."\n\n".var_export(debug_backtrace(), true), 1, "scott@mstech.com");
		error_log("Site: {$_SERVER['PHP_SELF']}\n\n".$errMess, 1, "scott@mstech.com");
		error("A Database error has occured, and an administrator has been notified. We are sorry for the inconvenience.");
		//error_reporting(E_ERROR);
		//trigger_error("A Database error has occured, and an adminstrator has been notified. We are sorry for the inconvenience.", E_ERROR);					
	}
	else
	{
		error_log($errMess);
		die($msg); 
	}
}
function error($errMess, $prevPage="history.go(-1)", $showFeedback = true)
{	
	if (!$prevPage) $prevPage="history.go(-1)";
	
	// This is so any buried "error" calls within ajax calls throw an exception instead of trying to print out an error
	// Borderline "hack"	
	if (preg_match("/\.ajax$/i", $_SERVER['PHP_SELF'])) throw new Exception($errMess);
	
	error_handler(E_USER_ERROR, $errMess, '','','',$prevPage);	
}
function debug_string_backtrace() 
{
        ob_start();
        
        debug_print_backtrace();
        $trace = ob_get_contents();
        
        ob_end_clean();

        // Remove first item from backtrace as it's this function which
        // is redundant.
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
function error_handler($errNo, $errStr, $errFile='', $errLine='', $type='',$prevPage="history.go(-1)")
{	
	global $config;	
	
	//error_reporting()==0 if the @ sign was used
	if ($errNo == E_NOTICE || error_reporting()==0) return;
	
	if($errNo == E_WARNING)
	{	
		if ($config[debugMode])
		{			
			debug("Error: $errNo <br>File: $errFile <br> Desc: $errStr <br>Line: $errLine<br />StackTrace: ".var_export(debug_backtrace(), true), null, false);
			return;
		}
		/*
		else
		{			
			error_log("Error: $errNo \nFile: $errFile \nDesc: $errStr \nLine: $errLine \nStack Trace:".var_export(debug_backtrace(), true));
		}
		
		return;*/
	}	
	
	$a = new pTemplate("Error!!");
	$a->renderedError = true;
	
	$t = new tblBuilder('center',0,0,10,'100%');	
	
	$r = $t->addRow();
	
	if ($errNo == 256)
	{
		$r->addCell(new tblCell($errStr,'newsCell'));
	}
	else
	{
		if (!$config[debugMode] && $errNo != E_USER_ERROR)
		{
			error_log("Site: $config[scripturl]\n Error: $errNo \nFile: $errFile \n Desc: $errStr \nLine: $errLine \nStack Trace:".var_export(debug_backtrace(), true),1,'scott@mstech.com');			
		}
		else
		{			
			error_log($errStr);	 
		}
		
		$eContent = ($config[debugMode] || $errNo == E_USER_ERROR ? $errStr : "An error has occurred processing your request. This has been logged and a global admin will look into it shortly.<br>
												  We apologize for the inconvenience.");	
	
		$r->addCell(new tblCell($eContent,'newsCell'));		
	}	
	
	$cmdBack = new customButton('btnBack', 'button', '&lt;&lt; Back');
	$cmdBack->addAttribute('onClick',$prevPage);
	
	$r = $t->addRow();
	
	$r->addCell(new tblCell($cmdBack->render(),'','center'));
	
	$errBlock = new panelBlock('','An error has occured processing your request:','550px');		
	$errBlock->addControl($t);	

	@mysqli_rollback($config[dbLink]);	
	
	$a->addControl($errBlock);
	$a->render();	
}
?>