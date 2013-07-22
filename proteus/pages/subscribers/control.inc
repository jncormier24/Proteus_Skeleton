<?php
	proteus_core::includeAllModules();
	
	switch($action)
	{
		default:			
			showSubscriber();
			break;	
	}
	
function showSubscriber()
{
	global $config;
	
	$a = new adminPage("Subscriber Configuration");	
	$a->addPageModule(new subscriptionModule());
	$a->addPageModule(new jqPopupMenuModule());	
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new jqUpdateHelperModule());
	$a->addPageModule(new jqFilterHelperModule());
	
	$sBlock = new panelBlock("", "Configure Subscription Services");
	$sBlock->id = "subscriberCont";
	
	$mnu = new jqPopupMenu();
	
	$ctl = $mnu->addMenuItem("", "Email Subscribers", "", "ui-icon-mail-closed");
	$ctl->onclick = "getSubscriptionSendWin(0,0)";
	
	$ctl = $mnu->addMenuItem("", "Manual Signup", "", "ui-icon-person");
	$ctl->onclick = "showSubscribe(0)";

	$sBlock->header->addControl($mnu);	
	
	$tabs = new jqUI_tabControl("subscriberTabs", 0, "admin/subscribers");
	
	$tpOpt = new jqUI_tabPage("subscriber_options", "Subscription Channels", "ui-icon-signal-diag");
	$tabs->addTabPage($tpOpt);
	
	$tpSub = new jqUI_tabPage("subscriber_list", "Subscribers", "ui-icon-person");
	
	$tblFilter = new tableFilter("subscribersFilter", "#subscribersTable", "function() { getSubscribers(); }");
	$tblFilter->addSortFilter("sub_email", "th.email", "asc");
	$tblFilter->addSortFilter("login", "th.login");	
	$tblFilter->setPaging(25);
	
	$tblFilter->addColumnFilter("sub_email", tableFilter::filter_text_having, "th.email");	
	$tblFilter->addColumnFilter("login", tableFilter::filter_text, "th.login");		
	
	$ctl = $tblFilter->addCustomFilter("channel", tableFilter::filter_dropdown, "Channel:");
	$ctl->addListItem(new phpListItem());
	
	$opts = subscriber_core::getSubscriberOptions();
	foreach($opts as $optObj)
	{
		$oTitle = validation::truncateText($optObj->title, 40);			
		$ctl->addListItem(new phpListItem($oTitle, $optObj->id));
	}
	
	$ctl = $tblFilter->addCustomFilter("subscriber_status", tableFilter::filter_dropdown, "Subscriber Status:");
	$ctl->addListItem(new phpListItem("Active Subscribers", 0));
	$ctl->addListItem(new phpListItem("Inactive Subscribers (opted out)", 1));
	
	$dv = new phpDIV();
	$dv->id = "subscribers";		
	
	$tpSub->addControl($tblFilter);
	$tpSub->addControl($dv);
	
	$tabs->addTabPage($tpSub);
	
	$tpDraft = new jqUI_tabPage("email_drafts", "Drafts", "ui-icon-document");
	$tabs->addTabPage($tpDraft);
	
	$tpSent = new jqUI_tabPage("email_sent", "Sent", "ui-icon-check");
	$tabs->addTabPage($tpSent);	
	
	$sBlock->addControl($tabs);
	
	$a->addControl($sBlock);	
	$a->render();
}
?>