<?php 
	switch($action)
	{
		default:
			showHome();
	}
	
function showHome()
{
	$a = new adminPage("Welcome to the Proteus Administation Panel");
	//$a->secure();
	
	$a->addControl(new phpHeader(1, "We made it, yo!"));
	$a->render();
}
?>