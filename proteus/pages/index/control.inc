<?php 
	switch($action)
	{
		default:
			show();
	}	

function show()
{	
	global $config;
	
	$a = new adminPage("Proteus Admin Console");	
	$d = new DAL();
	
	$conf = new siteConfig();
	
	$iBlock = new panelBlock("", "Welcome to the Proteus Management Interface Home");
	$iBlock->addClass("configWin");
	$iBlock->id = "indexBlock";
	
	$usersCt = $d->qryCount("select count(*) from users where inactive = 0");
	$authCt = $d->qryCount("select count(*) from users where lastLogin > NOW() - INTERVAL 15 MINUTE");
	$guestCt = $d->qryCount("select count(*) from sessions where userID = 0 and lastAccess > NOW() - INTERVAL 15 MINUTE");
	
	$modCt = count(explode("::", $conf->siteType));
	
	$cacheCt = $d->qryCount("select count(*) from cache");
	
	$fs = new phpFormFieldset("System Information");
	
	$fs->addControl(new phpSpan("<strong>Proteus Version:</strong> v".$config["version"], "label"));		
	$fs->addControl(new phpSpan("<strong>Server OS:</strong>".$_SERVER["SERVER_SOFTWARE"], "label"));
	$fs->addControl(new phpSpan("<strong>Configured Modules:</strong>".$modCt, "label"));
	$fs->addControl(new phpSpan("<strong>Cache Entries:</strong>".$cacheCt, "label"));
	$fs->addControl(new phpSpan("<strong>Site Pagereads:</strong>".$conf->hitCount, "label"));
	$fs->addControl(new phpSpan("<strong>User Accounts:</strong>".$usersCt, "label"));
	$fs->addControl(new phpSpan("<strong>Users Logged in:</strong>".$authCt, "label"));
	$fs->addControl(new phpSpan("<strong>Guests:</strong>".$guestCt, "label"));
	$fs->addControl(new phpSpan("<strong>Logged in as:</strong>".$a->authUser->login, "label"));
	
	$p = new phpParagraph(nl2br("Lorem ipsum dolor sit amet, velit sed at et ante vestibulum morbi, eu fringilla sapien vitae sem, elit luctus. Duis nisl morbi, interdum aliquam ut magna ipsum. Amet vestibulum aliquam, etiam nunc, facilisis nec amet dignissim mollis, sed massa risus, mattis mauris. Velit nunc eu tellus sed. Ut eu lectus et vestibulum. Ligula aliquam vehicula est in, maecenas libero dolor lacinia malesuada, euismod quis tellus massa rhoncus nam pulvinar. Quam ut quam elit fusce blandit risus, at non, lobortis vehicula scelerisque adipiscing. Felis congue amet sagittis. Semper magna elit, quisque etiam, modi a id scelerisque sed enim, adipiscing donec nibh vestibulum mauris eget, tortor nulla qui ut cursus amet arcu. Suspendisse in sed.

Maecenas et. Nunc semper, id rhoncus primis elit velit vel tristique. Praesent urna nibh suscipit, suspendisse at, vel lectus orci, velit suspendisse tortor est. Bibendum ut sed wisi, cras imperdiet sapien turpis aliquam, sem libero elementum penatibus in sit, purus sit arcu lobortis et rutrum, suscipit turpis sunt. Erat tincidunt mauris augue molestie at. Et sit, et fringilla vel ante, magna rhoncus ultrices vestibulum dui eget, massa arcu parturient in suspendisse. Pulvinar convallis, velit aliquam vivamus donec rhoncus, tempus qui non facilisis a, vel fringilla. Et nostra metus lacus interdum. Nisl porttitor, lacus non amet, sed integer sed quam, natoque euismod pede habitasse at feugiat. Morbi nunc nulla id class scelerisque in, hymenaeos cras, duis ultricies dignissim, ut justo penatibus quam.

Consequat lectus in quibusdam fermentum eget et, libero incidunt et penatibus, pellentesque risus et erat, curae vestibulum vivamus. A arcu turpis felis, mattis tortor consequat erat metus, lacus at sodales a ac feugiat, arcu metus nunc lacinia accumsan sollicitudin. Rutrum libero aliquet, risus in eget massa. Risus eu nam, montes dolor facilisi placerat amet vitae. Consectetuer lacus quis, sed tincidunt curabitur velit wisi, neque magnis.

Vestibulum tempor. In libero nec ante. Nullam diam ac dui, labore etiam ante ac, metus viverra eleifend pharetra tellus orci sociis, tellus et varius dui sodales amet aenean, imperdiet dapibus. Proin dui nulla adipiscing ut urna, in tristique dui accumsan sit, penatibus mi, vel laoreet viverra, tristique risus enim amet mauris. Aenean in, consectetuer sapien venenatis cras, congue eleifend arcu etiam lacus, nunc non turpis. Sapien platea id sed, at conubia, praesent sem euismod sed magna rhoncus. Nec risus, vulputate orci, aliquam nisl, cursus pulvinar velit vulputate nam iaculis. Aliquam accumsan qui amet aenean bibendum suspendisse, erat egestas felis proin quis, risus eros nibh mi integer et consectetuer, a sed eros non suspendisse, id lacus felis donec quam posuere. Imperdiet pretium velit donec, sem nam a, vulputate egestas venenatis rhoncus ullamcorper. Erat proin amet orci nec tristique. Etiam a metus velit non, erat sed wisi at, justo placerat amet eget ut imperdiet. Nulla vestibulum urna augue sit hac, volutpat placerat donec maecenas sociis commodo amet, volutpat diam elit a ipsum fringilla, fringilla ultricies risus wisi sed sapien, nec et molestie."));
	
	$iBlock->addControl($fs);
	$iBlock->addControl($p);
	
	$a->addControl($iBlock);
	
	$a->render();
}
?>