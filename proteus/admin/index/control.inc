<?php 
	switch($action)
	{
		default:
			show();
	}	

function show()
{	
	$a = new adminPage("Proteus Admin Console");	
	
	$hdr = new phpHeader(2, "It worked!");
	$hdr->style = "font-size: 20px;";
	
	$a->addControl($hdr);
	
	$a->render();
}
?>