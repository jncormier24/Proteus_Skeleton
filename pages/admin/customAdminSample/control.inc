<?php 
	$a = new adminPage("test");
	$a->addControl(new phpHeader(1, "It worked, yo!"));
	
	$a->render();
?>