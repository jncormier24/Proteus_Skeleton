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
	
	$writeAccess = $g_sec->hasAccess(permissions::$site_configuration, permissions::write) ? 1 : 0;
	
	$conf = new siteConfig();

	$tabs = new jqUI_tabControl("configTabs", 0, "admin/config");
	
	$frm = new phpForm("post", "frmConfig");	
	$frm->addControl(new hiddenInput("action","updateConfig"));
			
	$txtTitle = new customText("data[defaultPageTitle]", validation::prepTextForInput($conf->defaultPageTitle), 50, 255);
	$txtTitle->setLabel("Default Page Title:");
		
	$txtBusName = new customText("data[businessName]", validation::prepTextForInput($conf->businessName), 50, 255);
	$txtBusName->setLabel("Business Name:");
		
	$txtAddr = new phpTextArea("data[businessAddr]", validation::prepTextForInput($conf->businessAddr), 3, 45);
	$txtAddr->setLabel("Business Address:");
		
	$txtPhone = new customText("data[contactPhone]", validation::prepTextForInput($conf->contactPhone), 18, 14);
	$txtPhone->setLabel("Contact Phone:");		
		
	jqMaskedInputModule::setMask($txtPhone, jqMaskedInputModule::mask_phone);
		
	$txtEmail = new customText("data[sitemail]", validation::prepTextForInput($conf->sitemail), 50, 255);
	$txtEmail->setLabel("Site Email:");		
	
	$ddlTemplate = new phpDDL("data[defaultEmailTemplate]");
	$ddlTemplate->selected = $conf->defaultEmailTemplate;
	$ddlTemplate->setLabel("System Email Template:");
	
	$ddlTemplate->addListItem(new phpListItem());
	
	$baseDir = $config["baseAppDir"]."themes/".$config["frontend_theme"]."/emails";
	
	foreach(glob($baseDir."/*.tpl") as $file)
	{
		$fName = basename($file);
		$ddlTemplate->addListItem(new phpListItem($fName, $fName));
	}
		
	$fsGen = new phpFormFieldset("Site Detail", "standard");
	$fsGen->addControl($txtTitle);
	$fsGen->addControl($txtBusName);
	$fsGen->addControl($txtAddr);
	$fsGen->addControl($txtPhone);
	$fsGen->addControl($txtEmail);		

	$fsSys = new phpFormFieldset("System", "standard");
	$fsSys->addControl($ddlTemplate);
	
	$frm->addControl($fsGen);		
	$frm->addControl($fsSys);

	$tpGeneral = new jqUI_tabPage("general_options", "General Options", "ui-icon-wrench");
	$tpGeneral->addControl($frm);
	
	$tabs->addTabPage($tpGeneral);
	
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
	
	$tpMods = new jqUI_tabPage("modules", "Module Configuration", "ui-icon-tag");
	$tpMods->addControl($fsMods);
	
	$tabs->addTabPage($tpMods);
	
	$configBlock = new panelBlock("", "System Settings &amp; Options");
	$configBlock->addClass("configWin");
	$configBlock->addControl($tabs);
	$configBlock->addControl(new jqLoader("initConfig()"));
	
	$a->addControl($configBlock);
	$a->render();
}
?>