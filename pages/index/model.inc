<?php 
/*
 * 
	This file contains anything that needs to be included prior to loading control.inc; prerequisites (includes), custom PageClass, custom class definitions, etc.	
 *
*/

class indexPage extends proteusPage
{	
	// Default to standard
	public function __construct($title="")
	{		
		// To use the standard custom view template (/pages/pageIndex/view.tpl), use the built-in shortcut option from pageClass:
		// $this->setCustomView()
		
		parent::__construct($title);
	}	
}
?>