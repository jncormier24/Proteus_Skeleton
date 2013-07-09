<?php 
class subscriber_core
{
	const type_generic = 0;
	const type_municipal_posting = 1;
	const type_municipal_file = 2;
	const type_municipal_agenda = 3;
	 
	public $files = array("subscribers.php", "subscribers.ajax", "subscriber.ajax");	
	public $tables = array("subscribers"=>"CREATE TABLE `subscribers` (
									 `id` int(11) NOT NULL AUTO_INCREMENT,
									 `email` varchar(255) NOT NULL,
									 `inactive` int(4) NOT NULL,
									 `optoutGeneral` int(4) NOT NULL,									 
									 `createdDateTime` datetime NOT NULL,
									 `updatedDateTime` datetime NOT NULL,
									 PRIMARY KEY (`id`)
									) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"subscriberOptions"=>"CREATE TABLE `subscriberOptions` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `typeID` int(11) NOT NULL DEFAULT 0,
											 `dataID` int(11) NOT NULL DEFAULT 0,
											 `inactive` int(4) NOT NULL DEFAULT 0,
											 `createdDateTime` datetime NOT NULL,											 
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"subscriberOptionLink"=>"CREATE TABLE `subscriberOptionLink` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `subscriberID` int(11) NOT NULL DEFAULT 0,
												 `optionID` int(11) NOT NULL DEFAULT 0,	
												 `createdDateTime` datetime NOT NULL,											 
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"subscriptionHistory"=>"CREATE TABLE `subscriptionHistory` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `subject` varchar(255) NOT NULL,
												 `body` text NOT NULL,	
												 `options` varchar(255) NOT NULL,
												 `completed` int(4) NOT NULL DEFAULT 0,
												 `updatedDateTime` datetime NOT NULL,	
												 `updatedUserID` int(11) NOT NULL DEFAULT 0,
												 `lastSentDateTime` datetime NOT NULL,										 
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM DEFAULT CHARSET=utf8");
	
	public static function getSubscriberOptions($dataID='', $typeID='')
	{
		validation::ensureInt($dataID);
		validation::ensureInt($typeID);
		
		$d = new DAL(true);
		$wh = array();
		$results = array();
		
		if ($dataID) $wh[] = "dataID=$dataID";
		if ($typeID) $wh[] = "typeID=$typeID";
		
		$whCl = implode(",", $wh);
		
		$qry = "select id
				from subscriberOptions
				where inactive = 0 $whCl
				order by id asc";
		$sQry = $d->qry($qry);
		
		while($subscr = $sQry->fetch_assoc())
		{
			$obj = new subscriberOption($subscr[id]);
			$results[$obj->title] = $obj; 
		}		
		
		ksort($results);
		
		return $results;
	}
	public static function getEmailParser()
	{
		global $config;
		
		$templateOverride = "$config[baseAppDir]/themes/$config[defaultTheme]/admin/subscription.tpl";
			
		if (file_exists($templateOverride))
		{
			$parserFile = $templateOverride;
			$absolute = true;
		}
		else
		{
			$parserFile = "subscription.tpl";				
		}
		
		return new parser($parserFile, $absolute);
	}
}
class subscriberOption extends phpDataset 
{
	public function __construct($id=0)
	{
		parent::__construct("subscriberOptions", $id, $id ? true : false);
	}	 
	public function __get($key)
	{
		if ($key == "title")
		{
			switch($this->typeID)
			{
				case proteus_core::module_blogs:					
					return validation::prepForDisplay($this->linkedObject->title, false);
					
				case proteus_core::module_categoryItems:					
					return validation::prepForDisplay($this->linkedObject->name, false);
				
				case proteus_core::module_municipal:					
					return validation::prepForDisplay($this->linkedObject->title, false);
			}
		}
		
		if ($key == "linkedObject")
		{
			switch($this->typeID)
			{
				case proteus_core::module_blogs:
					return new customBlog($this->dataID, false);					
					
				case proteus_core::module_categoryItems:
					return new customCategory($this->dataID);					
				
				case proteus_core::module_municipal:
					return new pmm_entity($this->dataID);					
			}
		}
		
		return parent::__get($key);	
	}
	public function hasAccess($sec)
	{	
		if ($sec->hasAccess(permissions::$manage_subscribers, permissions::write)) return true;
		
		switch($this->typeID)
		{
			case proteus_core::module_blogs:				
				return ($sec->hasAccess(permissions::$manage_blogs, permissions::write) || $sec->hasAccess(permissions::$modify_blog_category, permissions::write, $this->linkedObject->id));
			case proteus_core::module_categoryItems:
				return ($sec->hasAccess(permissions::$manage_category_items, permissions::write) || $sec->hasAccess(permissions::$modify_category_type, permissions::write, $this->linkedObject->id));
			case proteus_core::module_municipal:
				return ($sec->hasAccess(permissions::$manage_municipal_entities, permissions::write) || $sec->hasAccess(permissions::$municipal_entity, permissions::write, $this->linkedObject->id));
		}
		
		return false;
	}
	public function getSubscribers($inactive = 0)
	{		
		$d = new DAL(true);
		$sArr = array();

		validation::ensureInt($inactive, 0);
		
		$qry = "select subscribers.id
				from subscribers inner join subscriberOptionLink on subscriberOptionLink.subscriberID = subscribers.id
				where inactive = $inactive and subscriberOptionLink.optionID = $this->id
				order by subscribers.email";
		$sQry = $d->qry($qry);
		
		while($subscr = $sQry->fetch_assoc())
		{
			$sArr[] = new subscriber($subscr[id]);	
		}
		
		return $sArr;
	}
	public static function getSubscriberOption($typeID, $dataID)
	{
		if (is_array($typeID)) $typeID = $typeID["siteType"];
		
		validation::ensureInt($typeID);
		validation::ensureInt($dataID);
		
		if (!$typeID || !$dataID) throw new Exception("Error, missing required parameters.");
		
		$d = new DAL(true);
		return $d->qryArray("select * from subscriberOptions where typeID=$typeID and dataID=$dataID");			
	}
}
class subscriber extends phpDataset
{
	public function __construct($id=0)
	{
		parent::__construct("subscribers", $id, $id ? true : false);
	}
	public function linkOption($optionID)
	{
		$d = new DAL(true);
		$optObj = new subscriberOption($optionID);
		
		$cv[optionID] = $optObj->id;
		$cv[subscriberID] = $this->id;
		$cv[createdDateTime] = "now()";

		return $d->qryInsertByArray("subscriberOptionLink", $cv);
	}
	public static function getSubscriberID($email)
	{
		validation::makeSafe($email);
		
		$d = new DAL(true);
		return $d->qryCount("select id from subscribers where email='$email'");			
	}
}
?>