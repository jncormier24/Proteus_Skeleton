<?php
/*
 * Default custom error handler function. This can be tailored to display additional information, etc.
 */
function error_handler($errNo, $errStr, $errFile='', $errLine='', $type='')
{	
	global $config;	
	
	// error_reporting()==0 if the @ sign was used
	if ($errNo == E_NOTICE || error_reporting()==0) return;
	
	if ($errNo == E_WARNING)
	{	
		if ($config["debug_mode"])
		{			
			debug("Error: $errNo 
			       File: $errFile 
			       Desc: $errStr
			       Line: $errLine
			
			       StackTrace: ".var_export(debug_backtrace(), true));
			return;
		}
	}	
	
	// A different template can easily be used here, or even another extended pageClass to display custom error information / UI
	$a = new proteusPage("Error!!");
	
	$eCont = new phpDIV();
	$eCont->class = "errorContainer";
	
	$eCont->addControl(new phpHeader(1, "An error has occurred processing your request."));
	
	if ($errNo == 256)		
	{
		$eCont->addControl(new phpParagraph($errStr));		
	}
	else
	{
		if (!$config["debug_mode"] && $errNo != E_USER_ERROR && $config["debug_email_address"])
		{
			error_log("Error: $errNo \nFile: $errFile \n Desc: $errStr \nLine: $errLine \nStack Trace:".var_export(debug_backtrace(), true),1, $config["debug_email_address"]);			
		}
		else
		{			
			error_log($errStr);	 
		}
		
		if ($config["debug_mode"] || $errNo == E_USER_ERROR)
		{
			$eCont->addControl(new phpParagraph($errStr));
		}
		else
		{
			$eCont->addControl(new phpParagraph("We apologize for the inconvenience, but an unknown error has occured processing your request. Please try again, or contact the site administrator if the problem persists."));
		}		
	}	
	
	$cmdBack = new jqUI_button("", "Back to Previous Page", "", "ui-icon-seek-prev");
	$cmdBack->onclick = "history.back(1)";
	
	$a->addControl($eCont);
	$a->render();	
}
?>