<?php 
	switch($action)
	{		
		default:
			show();
	}	

function show()
{		
	global $config;
	
	$a = new adminPage("System Settings &amp; Options");
	$a->addPageModule(new jqMaskedInputModule());
	$a->addPageModule(new jqUpdateHelperModule());
	
	$g_sec = $a->authUser;	
		
	if (!$g_sec->hasAccess(permissions::$site_configuration, permissions::read)) $g_sec->security_error();
	
	$conf = new siteConfig();
				
	$frm = new phpForm("post", "frmConfig");	
	$frm->addControl(new hiddenInput("action","updateConfig"));
			
	$txtTitle = new customText("data[defaultPageTitle]", validation::prepTextForInput($conf->defaultPageTitle), 50, 255);
	$txtTitle->setLabel("Default Page Title:");
		
	$txtBusName = new customText("data[businessName]", validation::prepTextForInput($conf->businessName), 50, 255);
	$txtBusName->setLabel("Business Name:");
		
	$txtAddr = new phpTextArea("data[businessAddr]", validation::prepTextForInput($conf->businessAddr), 3, 45);
	$txtAddr->setLabel("Business Address:");
		
	$txtPhone = new customText("data[contactPhone]", validation::prepTextForInput($conf->contactPhone), 14, 14);
	$txtPhone->setLabel("Contact Phone:");		
		
	jqMaskedInputModule::setMask($txtPhone, "'phone-us'");
		
	$txtEmail = new customText("data[sitemail]", validation::prepTextForInput($conf->sitemail), 50, 255);
	$txtEmail->setLabel("Site Email:");		
		
	$fsGen = new phpFormFieldset("General Options", "standard");
	$fsGen->addControl($txtTitle);
	$fsGen->addControl($txtBusName);
	$fsGen->addControl($txtAddr);
	$fsGen->addControl($txtPhone);
	$fsGen->addControl($txtEmail);		
		
	$frm->addControl($fsGen);		
		
	$writeAccess = $g_sec->hasAccess(permissions::$site_configuration, permissions::write) ? 1 : 0;
	
	$configBlock = new panelBlock("", "System Settings &amp; Options");
	$configBlock->addClass("configWin");
	$configBlock->addControl($frm);
	$configBlock->addControl(new jqLoader("initConfig()"));
	
	$modArr = proteus_core::getModulesArray();
	
	$fsMods = new phpFormFieldset("Available Modules");
	
	$ct = 0;
	
	foreach($modArr as $module)
	{
		$rd1 = new phpRadioButton("on_$ct", 1, $conf->moduleEnabled($module["siteType"]));
		$rd1->name = "radgroup_$ct";
		$rd1->setLabel("On");
		
		if ($module["required"])
		{
			$rd1->addParameter("checked");
			$rd1->addParameter("disabled");
			
			$rd2 = new phpFiller();
		}
		else
		{
			$rd2 = new phpRadioButton("off_$ct", 0, !$conf->moduleEnabled($module["siteType"]));
			$rd2->name = "radgroup_$ct";
			$rd2->setLabel("Off");
		}
		
		$dvSel = new phpDIV("","mod_select shine_solid");
		$dvSel->addAttribute("data-sitetype", $module["siteType"]);
		$dvSel->addControl(new phpHeader(4, $module["caption"]));

		$dv = new phpDIV();
		$dv->addControl($rd1);
		$dv->addControl($rd2);

		$dvSel->addControl($dv);
		
		$fsMods->addControl($dvSel);
		$ct++;
	}
	
	$configBlock->addControl($fsMods);
	
	$a->addControl($configBlock);
	$a->render();
}
?>