<?php 
/*
 * 
	This file contains anything that needs to be included prior to loading control.inc; prerequisites (includes), custom PageClass, custom class definitions, etc.	
 *
*/

class indexPage extends proteusPage
{
	public $pageIndex = "index";
	
	// Default to standard
	public function __construct($title="")
	{		
		parent::__construct($title);
	}	
}
?>