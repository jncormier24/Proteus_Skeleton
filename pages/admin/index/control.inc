<?php 
	$a = new adminPage("test");
	$a->addControl(new phpHeader(1, "Index functionality replaced!"));
	
	$a->render();
?>