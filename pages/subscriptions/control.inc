<?php 

	proteus_core::includeAllModules();
	
	switch($config["params"][0])
	{
		case "manage":
			manageSubscription($key);
			break;
			
		case "confirmSubscription":
			confirmSubscription($key);			
			break;
			
		default:
			die("Unauthorized");
	}		
	
function confirmSubscription($key)
{
	global $config;
	
	$a = new proteusPage("Subscription Confirmation");
	$a->addPageModule(new subscriptionModule());
	
	$d = new DAL(true);
	
	try
	{			
		$sObj = subscriber::getSubscriberFromKey($key);
		
		if (!$sObj->id) throw new Exception("Error, that subscription entry could not be found!");
		if (!$sObj->inactive) throw new Exception("Error, this subscription entry has already been verified. Thank you!");
		
		// If this person is logged in (and email addresses match), tie their user account to this subscription entry
		if ($a->authUser && !$sObj->userID && $sObj->email == $a->authUser->email)
		{
			$sObj->userID = $a->authUser->id;
			$sObj->email = "";			
		}
		
		$sObj->inactive = 0;
		$sObj->save();
		
		$sWin = new jqUI_dialog("confirmSuccessWin", false, 800, 400, "Subscription Confirmation Successful!");
		$sWin->setOption("autoOpen", 1);
		$sWin->setOption("autoResize", 1);
		$sWin->setOption("beforeClose", "function() { $.showLoading(); window.location='{$config["siteurl"]}'; return false; }");
		
		$sWin->addCloseButton("Home");
		$sWin->addButton("Manage Subscriptions Now", "function() { window.location='subscriptions/manage?key=$key'; }");

		$sWin->addControl(new phpParagraph("Successfully verified Subscription email! To change subscription options in the future, or remove 
		yourself entirely, click the link at the bottom of any subscription email to go to the subscription management interface; or use the 
		button below to manage now.<br /><br />
		Thank you!"));
		
		$a->addControl($sWin);
		$a->render();
	}
	catch (Exception $ex)
	{
		error($ex->getMessage());
	}
}
function manageSubscription($key)
{
	global $config;
	
	$a = new proteusPage("Subscription Management");
	$a->addPageModule(new subscriptionModule());
	
	$d = new DAL(true);
	
	try
	{			
		$sObj = subscriber::getSubscriberFromKey($key);
		
		if (!$sObj->id) throw new Exception("Error, that subscription entry could not be found!");
		
		$a->addControl(new jqLoader("showSubscriptionManage($sObj->id)"));
		$a->render();
	}
	catch (Exception $ex)
	{
		error($ex->getMessage());
	}
}
?>