<?php 
	switch($action)
	{	
		default:
			show($ref);
	}
	
function show($ref)
{
	$a = new adminPage("Please Log In", false);	
	
	$txtName = new customText("login", validation::prepTextForInput($_SESSION["username"]), 25, 20);
	$txtName->setLabel("Login Name:");					
	
	$txtPass = new phpPassword("password",'',25,25);
	$txtPass->setLabel("Password:");			
	
	$btnSubmit = new jqUI_button("loginButton", "Log In", "", "ui-icon-key");
	$btnSubmit->onclick = "validateLogin('$ref')";		
	
	$lForm = new phpForm("post", "frmLogin");			
	$lForm->addControl($btnSubmit);
	$lForm->addControl($txtName);
	$lForm->addControl($txtPass);
	
	$lBlock = new panelBlock("", "Please log in to continue");
	$lBlock->id = "loginBlock";
	
	$lBlock->addControl($lForm);
	
	$ffImg = new image("pages/login/assets/firefox.png");
	$ffImg->setAltTag("Get FireFox now!");
	
	$ffLink = new phpHyperlink("http://www.mozilla.org/en-US/firefox/new/", $ffImg->render());
	
	$chImg = new image("pages/login/assets/chrome.png");
	$chImg->setAltTag("Get Google Chrome now!");
	
	$chLink = new phpHyperlink("https://www.google.com/intl/en/chrome/browser/", $chImg->render());
	
	$bBlock = new phpDIV();
	$bBlock->id = "browserSelect";
	$bBlock->addControl(new phpParagraph("This application has been designed and developed to run on W3C compliant browsers, so it is highly recommended that FireFox, Google Chrome, or as a last resort Internet Explorer 8+ be used. You can use the links located below to quickly install FireFox or Chrome for the best possible browsing experience."));
	$bBlock->addControl($ffLink);
	$bBlock->addControl($chLink);	
	
	$lBlock->addControl($bBlock);	

	$a->addControl($lBlock);
	$a->render();
}
?>