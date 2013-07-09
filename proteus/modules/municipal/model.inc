<?php
class pmm_core
{	
	public $asset_folders = array("municipal");	
	public $tables = array("pmm_entities"=>"CREATE TABLE `pmm_entities` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `typeID` tinyint(4) NOT NULL,
											 `title` varchar(75) CHARACTER SET utf8 NOT NULL,
											 `blurb` text CHARACTER SET utf8 NOT NULL,
											 `description` text CHARACTER SET utf8 NOT NULL,
											 `address` text CHARACTER SET utf8 NOT NULL,
											 `inactive` tinyint(4) NOT NULL,
											 `updatedUserID` int(11) NOT NULL,
											 `updatedDateTime` datetime NOT NULL,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8",
							"pmm_entityAgendas"=>"CREATE TABLE `pmm_entityAgendas` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `entityID` int(11) NOT NULL,
												 `title` varchar(255) CHARACTER SET utf8 NOT NULL,
												 `location` text CHARACTER SET utf8 NOT NULL,
												 `body` text CHARACTER SET utf8 NOT NULL,
												 `meetingDateTime` datetime NOT NULL,
												 `updatedUserID` int(11) NOT NULL,
												 `updatedDateTime` datetime NOT NULL,
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8",
							"pmm_entityFaq"=>"CREATE TABLE `pmm_entityFaq` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `entityID` int(11) NOT NULL,
											 `question` varchar(255) CHARACTER SET utf8 NOT NULL,
											 `answer` text CHARACTER SET utf8 NOT NULL,
											 `position` int(11) NOT NULL,
											 `updatedUserID` int(11) NOT NULL,
											 `updatedDateTime` datetime NOT NULL,
											 PRIMARY KEY (`id`),
											 KEY `entityID` (`entityID`)
											) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8",
							"pmm_entityFiles"=>"CREATE TABLE `pmm_entityFiles` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `dataID` int(11) NOT NULL,
											 `typeID` tinyint(4) NOT NULL,
											 `fileName` varchar(255) CHARACTER SET utf8 NOT NULL,
											 `caption` varchar(75) CHARACTER SET utf8 NOT NULL,
											 `position` tinyint(4) NOT NULL,
											 `inactive` tinyint(4) NOT NULL,
											 `updatedUserID` int(11) NOT NULL,
											 `updatedDateTime` datetime NOT NULL,
											 PRIMARY KEY (`id`),
											 KEY `entityID` (`dataID`)
											) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8",
							"pmm_entityLinks"=>"CREATE TABLE `pmm_entityLinks` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `typeID` int(11) NOT NULL,
											 `dataID` int(11) NOT NULL,
											 `url` varchar(255) CHARACTER SET utf8 NOT NULL,
											 `caption` varchar(255) CHARACTER SET utf8 NOT NULL,
											 `position` int(11) NOT NULL,
											 `updatedUserID` int(11) NOT NULL,
											 `updatedDateTime` datetime NOT NULL,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8",
							"pmm_entityPostings"=>"CREATE TABLE `pmm_entityPostings` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `entityID` int(11) NOT NULL,
												 `typeID` tinyint(4) NOT NULL,
												 `title` varchar(255) CHARACTER SET utf8 NOT NULL,
												 `blurb` text CHARACTER SET utf8 NOT NULL,
												 `body` text CHARACTER SET utf8 NOT NULL,
												 `publishDate` date NOT NULL,
												 `expireDate` date NOT NULL,
												 `inactive` tinyint(4) NOT NULL,
												 `updatedUserID` int(11) NOT NULL,
												 `updatedDateTime` datetime NOT NULL,
												 PRIMARY KEY (`id`),
												 KEY `entityID` (`entityID`),
												 KEY `entityID_2` (`entityID`)
												) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8",
							"pmm_entityStaff"=>"CREATE TABLE `pmm_entityStaff` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `entityID` int(11) NOT NULL,
												 `title` varchar(75) CHARACTER SET utf8 NOT NULL,
												 `description` text CHARACTER SET utf8 NOT NULL,
												 `term` varchar(50) CHARACTER SET utf8 NOT NULL,
												 `address` text CHARACTER SET utf8 NOT NULL,
												 `assigned` varchar(50) CHARACTER SET utf8 NOT NULL,
												 `isElected` tinyint(4) NOT NULL,
												 `position` int(11) NOT NULL,
												 `updatedUserID` int(11) NOT NULL,
												 `updatedDateTime` datetime NOT NULL,
												 `inactive` tinyint(4) NOT NULL,
												 PRIMARY KEY (`id`),
												 KEY `entityID` (`entityID`)
												) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8");
	
	public static function getConfigObject()
	{
		return new customConfig(11, 0, array("iconDim"=>"250x150", "staffIconDim"=>"250x150", "postIconDim"=>"150x100", "defaultMeetingTime"=>"19:00"));
	}
	public static function getEntityTypes($plural = false, $showAll = false)
	{
		if ($showAll) $arr["All Entities"] = 0;
		$arr["Department".($plural ? "s" : "")] = 1;
		$arr["Board".($plural ? "s" : "")] = 2;
		$arr["Committee".($plural ? "s" : "")] = 3;
		
		return $arr;
	}
	public static function getPostTypes($plural = false, $showAll = false)
	{
		if ($showAll) $arr["All Postings"] = 0;
		$arr["News"] = 1;
		$arr["Notice".($plural ? "s" : "")] = 2;		
		
		return $arr;
	}
	public static function getLinkedFiles($typeID, $dataID='', $sort='position asc', $limitFragment = 300 )
	{
		$d = new DAL(true);
		$arr = array();		
		
		if ($limitFragment) $limit = "limit $limitFragment";
		$wh = ($dataID ? " and dataID=$dataID" : "");
		
		$qry = "select id
				from pmm_entityFiles
				where typeID=$typeID $wh
				order by $sort
				$limit";				
				
		$fQry = $d->qry($qry);
			
		while($file = $fQry->fetch_assoc())
		{
			$fileObj = new pmm_entityFile($file["id"]);
			$arr[] = $fileObj;			
		}
		
		return $arr;
	}
}
class pmm_entity extends phpDataset 
{	
	public $settings;
	private $p_staff = array();
	private $p_faq = array();
	private $p_posts = array();	
		
	public function __construct($entityID = 0)
	{
		global $config;
		
		validation::ensureInt($entityID);		
		parent::__construct("pmm_entities", $entityID, $entityID ? true : false);
		
		//Create a custom config object for the base entity configuration (no associated ID)
		$this->settings = pmm_core::getConfigObject();

		if ($entityID)
		{
			$folder = "assets/municipal/{$this->id}/";
			$absFolder = $config["baseAppDir"].$folder;
			
			if (!file_exists($absFolder))
			{
				mkdir($absFolder, 0770, true);
			}
		}
	}	
	public function getIconObject(&$fileLocation='', &$dimensions='')
	{
		global $config;
		
		//Shouldn't ever be getting an icon object for a non-existent entity.
		if (!$this->id) return false;
		
		$folder = "assets/municipal/{$this->id}/";		
		$absFolder = $config["baseAppDir"].$folder;		
			
		$file = "icon.png";		

		$dimensions = explode("x", $this->settings->config("iconDim"));
		
		if (file_exists($absFolder.$file))
		{			
			$iconObject = new image($folder.$file."?rnd=".rand(0, 100), validation::prepForDisplay($this->title), "entityIcon");
		}
		else
		{
			$iconObject = new phpDIV("<p>No Image Available<p>", "entityIcon");
		}
		
		$iconObject->style = "width: {$dimensions[0]}px; height: {$dimensions[1]}px;";

		return $iconObject;
	}
	public function getTypeDisplay()
	{
		if (!$this->id) return;

		switch($this->typeID)
		{
			case 1:
				return "Department";
			case 2:
				return "Board / Commission";
			default:
				return "Unknown Type";
		}
	}
	public function getStaff($filter=0, $refresh = false)
	{
		// Filter:
		// 0: All
		// 1: Active
		// 2: Inactive
		
		if ($refresh || !count($this->p_staff))
		{
			$d = new DAL(true);
			
			if ($filter) $whCl = " and inactive = ".($filter-1);			
			
			$sQry = pmm_entityStaff::getQryObject("entityID = $this->id $whCl");
			
			while($staff = $sQry->fetch_assoc())
			{
				$staffObj = new pmm_entityStaff($staff["id"]);
				$this->p_staff[] = $staffObj;	
			}
		}
				
		return $this->p_staff;
	}
	public function getFaqEntries($refresh = false)
	{
		if ($refresh || !count($this->p_staff))
		{
			$d = new DAL(true);			
				
			$fQry = pmm_entityFaq::getQryObject("entityID=$this->id");
				
			while($faq = $fQry->fetch_assoc())
			{
				$faqObj = new pmm_entityFaq($faq["id"]);
				$this->p_faq[] = $faqObj;			
			}
		}
			
		return $this->p_faq;
	}
	public function getPostEntries($typeID='', $filter='', $limit='', $excludeID = 0)
	{
		//Limit is a string in "limit" format; "0,50"
		validation::ensureInt($typeID);
		
		$d = new DAL(true);			

		$wh = array();
		
		switch($filter)
		{
			case 1:
				//Active Only
				$wh[] = "inactive <> 1 and (publishDate = 0000-00-00 or publishDate <= now()) and (expireDate = 0000-00-00 or expireDate >= now())";
				break;
			case 2:
				//Pending Only
				$wh[] = "inactive <> 1 and publishDate <> 0000-00-00 and publishDate > now()";
				break;
			case 3:
				//Expired Only	
				$wh[] = "inactive <> 1 and expireDate <> 0000-00-00 and expireDate < now()";
				break;
			case 4:
				$wh[] = "inactive = 1";
				break;
		}
		
		if ($typeID) $wh[] = "typeID = $typeID";
		if ($excludeID) $wh[] = "id <> $excludeID";
		if (count($wh)) $whCl = "and ".implode(" and ", $wh);
		if ($limit) $lmt = "limit $limit";
		
		$qry = "select id
				from pmm_entityPostings
				where entityID = $this->id $whCl
				order by case publishDate when '0000-00-00' then updatedDateTime else publishDate end desc
				$lmt";
				
		$pQry = $d->qry($qry);
			
		while($post = $pQry->fetch_assoc())
		{
			$postObj = new pmm_entityPosting($post["id"]);
			$this->p_posts[$typeID][] = $postObj;			
		}		
			
		return $this->p_posts[$typeID];
	}
	public function getFiles()
	{
		if (!$this->id) return false;
		return pmm_core::getLinkedFiles(1, $this->id);
	}
	public function getLinks()
	{
		if (!$this->id) return false;
		
		$d = new DAL(true);
		$arr = array();		

		$lQry = pmm_entityLink::getQryObject("typeID=1 and dataID=$this->id");
			
		while($link = $lQry->fetch_assoc())
		{
			$linkObj = new pmm_entityLink($link["id"]);
			$arr[] = $linkObj;			
		}		
			
		return $arr;	
	}
	public function getAgendas($showLapsed = true)
	{
		if (!$this->id) return false;
		
		$d = new DAL(true);
		$arr = array();
		
		$whCl = "entityID = $this->id";
		
		if (!$showLapsed)
		{
			$whCl .= " and meetingDateTime >= now()";
			$ord = "meetingDateTime asc";
		}
		else 
		{ 
			$ord = "meetingDateTime desc";
		}
		
		$aQry = pmm_entityAgenda::getQryObject($whCl, $ord);
			
		while($agenda = $aQry->fetch_assoc())
		{
			$agendaObj = new pmm_entityAgenda($agenda["id"]);
			$arr[] = $agendaObj;			
		}		
			
		return $arr;	
	}
	public function getContentEntries($startPos = 0, $length = 0, $totalCount = '')
	{
		if (!$this->id) return false;		
		return customContent_core::getContentEntries(customContent_core::content_municipal_entity, $this->id, $startPos, $length, $totalCount);	
	}
	public static function getQryObject($whereFragment="", $orderFragment="title asc", $limitFragment="")
	{
		$d = new DAL(true);
		
		if ($whereFragment) $wh = "where $whereFragment";
		if ($orderFragment) $order = "order by $orderFragment";
		if ($limitFragment) $limit = "limit $limitFragment";
		
		$qry = "select id
				from pmm_entities
				$wh
				$order
				$limit";
				
		return $d->qry($qry, false);	
	}	
	public static function getCalendars()
	{
		if (!$this->id) return;
		return customCalendar::getCalendars(customCalendar::calendar_municipal_entity, $this->id);
	}
}
interface entityChild
{
	//Force all the entity children declare this method since multiple inheritance doesn't work yet (pre php5.4)
	public function getParentEntity();
}
class pmm_entityStaff extends phpDataset implements entityChild
{
	public function __construct($staffID)
	{
		validation::ensureInt($staffID);		
		parent::__construct("pmm_entityStaff", $staffID, $staffID ? true : false);		
	}
	public function getIconObject(&$fileLocation='', &$dimensions='')
	{
		global $config;
		
		//Shouldn't ever be getting an icon object for a non-existent entity.
		if (!$this->id) return false;
		
		$folder = "assets/municipal/{$this->entityID}/";
		$absFolder = $config["baseAppDir"].$folder;
		
		$file = "stafficon_{$this->id}.png";
		
		$cfg = pmm_core::getConfigObject();
		$dimensions = explode("x", $cfg->config("staffIconDim"));
		
		if (file_exists($absFolder.$file))
		{
			$iconObject = new image($folder.$file."?rnd=".rand(0, 100), validation::prepForDisplay($this->title), "entityIcon");
		}
		else
		{
			$iconObject = new phpDIV("<p>No Image Available<p>", "entityIcon");
		}		

		return $iconObject;
	}	
	public function getParentEntity()
	{
		if (!$this->id) return false;
		return new pmm_entity($this->entityID);	
	}
	public static function getQryObject($whereFragment="", $orderFragment="position asc", $limitFragment="")
	{
		$d = new DAL(true);
		
		if ($whereFragment) $wh = "where $whereFragment";
		if ($orderFragment) $order = "order by $orderFragment";
		if ($limitFragment) $limit = "limit $limitFragment";
		
		$qry = "select id
				from pmm_entityStaff
				$wh
				$order
				$limit";
				
		return $d->qry($qry, false);	
	}	
}
class pmm_entityFaq extends phpDataset implements entityChild 
{
	public function __construct($faqEntryID)
	{
		validation::ensureInt($faqEntryID);		
		parent::__construct("pmm_entityFaq", $faqEntryID, $faqEntryID ? true : false);		
	}
	public function getParentEntity()
	{
		//Multiple inheritance would be nice here...
		if (!$this->id) return false;
		return new pmm_entity($this->entityID);	
	}
	public static function getQryObject($whereFragment="", $orderFragment="position asc", $limitFragment="")
	{
		$d = new DAL(true);
		
		if ($whereFragment) $wh = "where $whereFragment";
		if ($orderFragment) $order = "order by $orderFragment";
		if ($limitFragment) $limit = "limit $limitFragment";
		
		$qry = "select id
				from pmm_entityFaq
				$wh
				$order
				$limit";
				
		return $d->qry($qry, false);	
	}		
}
class pmm_entityPosting extends phpDataset implements entityChild
{
	public function __construct($postEntryID)
	{
		validation::ensureInt($postEntryID);		
		parent::__construct("pmm_entityPostings", $postEntryID, $postEntryID ? true : false);		
	}
	public function getIconObject(&$fileLocation='', &$dimensions='')
	{
		global $config;
		
		//Shouldn't ever be getting an icon object for a non-existent entity.
		if (!$this->id) return false;
		
		$folder = "assets/municipal/{$this->entityID}/";
		$absFolder = $config["baseAppDir"].$folder;
			
		$file = "posticon_{$this->id}.png";
		
		$cfg = pmm_core::getConfigObject();
		$dimensions = explode("x", $cfg->config("postIconDim"));
		
		if (file_exists($absFolder.$file))
		{			
			$iconObject = new image($folder.$file."?rnd=".rand(0, 100), validation::prepForDisplay($this->title), "entityIcon");
		}
		else
		{
			$iconObject = new phpDIV("<p>No Image Available<p>", "entityIcon");
		}
		
		$iconObject->style = "width: {$dimensions[0]}px; height: {$dimensions[1]}px;";

		return $iconObject;
	}	
	public function __get($key)
	{
		global $config;		
		
		if ($key == "icon")
		{			
			$folder = "assets/municipal/{$this->entityID}/";
			$absFolder = $config[baseAppDir].$folder;
			
			$file = "posticon_{$this->id}.png";
			
			if (!$this->id || !file_exists($absFolder.$file)) return false;

			return $folder.$file."?rnd=".rand(0, 100);
		}
		
		return parent::__get($key);	
	}		
	public function getParentEntity()
	{
		//Multiple inheritance would be nice here...
		if (!$this->id) return false;
		return new pmm_entity($this->entityID);	
	}
	public function getTypeDisplay()
	{
		if (!$this->id) return false;
		
		switch($this->typeID)
		{
			case 1:
				return "News Entry";
			case 2:
				return "Notice";				
		}
	}	
}
class pmm_entityFile extends phpDataset implements entityChild
{
	public function __construct($fileID)
	{
		validation::ensureInt($fileID);		
		parent::__construct("pmm_entityFiles", $fileID, $fileID ? true : false);		
	}
	public function __get($key)
	{
		global $config;		
		
		switch($key)
		{
			case "file":
			case "localFile":					
				if (!$this->id) return false;
				
				$url = $this->fileFolder.$this->fileName;
				$loc = $config["baseAppDir"].$url;
				
				if (!file_exists($loc)) return false;
	
				return ($key == "file" ? "" : $config["baseAppDir"]).$url;
								
			case "fileFolder":
				switch($this->typeID)
				{
					case 0:	
						return "assets/municipal/";						
					case 1:
						$parentObj = $this->getParentEntity();
						if (!$parentObj->id) return false;
						
						return "assets/municipal/$parentObj->id/";
						
					case 2:
						$parentObj = $this->getParentEntity();
						if (!$parentObj->id) return false;
						
						$entityObj = $parentObj->getParentEntity();
						
						return "assets/municipal/$entityObj->id/minutes/";
						
					default:
						throw new Exception("Unsupported file location");						
				}				
		}
		
		return parent::__get($key);	
	}		
	public function getParentEntity()
	{
		//Multiple inheritance would be nice here...
		if (!$this->typeID || !$this->dataID) return false;
		
		switch($this->typeID)
		{
			case 0:
				return false;
			case 1:
				return new pmm_entity($this->dataID);
			case 2:
				return new pmm_entityAgenda($this->dataID);
		}	
	}	
	public function delete()
	{	
		//if (!unlink($this->localFile)) throw new Exception("Sorry, but we were unable to delete that file from the file system. Please contact support at your convenience to have this issue resolved. Thank you!");

		return parent::delete();
	}
	public function hasAccess($sec, $assigned_permissions, $global_permissions)
	{		
		if (!$this->dataID) return false;
		if ($sec->hasAccess(permissions::$manage_municipal_entities, $global_permissions)) return true;
		
		switch($this->typeID)
		{
			case 0:
				//General Municipal Files (not yet implemented)
				throw new Exception("Error, unsupported file type");
				break;
				
			case 1:
				//Entity Files					
				//return $sec->hasAccess(permissions::$municipal_entity, $assigned_permissions, $this->dataID);					
				//break;				
			case 2:
				//Entity Agendas
				$parentObj = $this->getParentEntity();
				if (!$parentObj->id) return false;
				
				return $sec->hasAccess(permissions::$municipal_entity, $assigned_permissions, $parentObj->entityID);					
				break;
				
			default:
				throw new Exception("Error, unsupported file type");
		}
		
	}
}
class pmm_entityLink extends phpDataset implements entityChild
{
	public function __construct($linkID)
	{
		validation::ensureInt($linkID);		
		parent::__construct("pmm_entityLinks", $linkID, $linkID ? true : false);		
	}
	public function __get($key)
	{	
		global $config;
		
		if ($key == "fullUrl")
		{
			$baseUrl = $this->url;			
			if (!preg_match("/^http/i", $baseUrl))
			{							
				if (!preg_match("'^/'i", $baseUrl)) return "http://".$baseUrl;
				return $config[scripturl].$baseUrl;
			}
			
			return $baseUrl; 
		}
		
		return parent::__get($key);
	}
	public function getParentEntity()
	{
		//Multiple inheritance would be nice here...
		if (!$this->typeID || !$this->dataID) return false;
		
		switch($this->typeID)
		{
			case 0:
				return false;
			case 1:
				return new pmm_entity($this->dataID);
		}	
	}	
	public function hasAccess($sec, $assigned_permissions, $global_permissions)
	{		
		if (!$this->dataID) return false;
		if ($sec->hasAccess(permissions::$manage_municipal_entities, $global_permissions)) return true;
		
		switch($this->typeID)
		{
			case 0:
				//General Municipal Files (not yet implemented)
				throw new Exception("Error, unsupported file type");
				break;
				
			case 1:
				//Entity Files					
				return $sec->hasAccess(permissions::$municipal_entity, $assigned_permissions, $this->dataID);					
				break;
				
			default:
				throw new Exception("Error, unsupported file type");
		}
		
	}
	public static function getQryObject($whereFragment="", $orderFragment="position asc", $limitFragment="")
	{
		$d = new DAL(true);
		
		if ($whereFragment) $wh = "where $whereFragment";
		if ($orderFragment) $order = "order by $orderFragment";
		if ($limitFragment) $limit = "limit $limitFragment";
		
		$qry = "select id
				from pmm_entityLinks
				$wh
				$order
				$limit";
				
		return $d->qry($qry, false);	
	}	
}
class pmm_entityAgenda extends phpDataset implements entityChild 
{
	public function __construct($agendaID)
	{
		validation::ensureInt($agendaID);		
		parent::__construct("pmm_entityAgendas", $agendaID, $agendaID ? true : false);		
	}
	public function getParentEntity()
	{
		//Multiple inheritance would be nice here...
		if (!$this->id) return false;
		return new pmm_entity($this->entityID);	
	}
	public function getFiles()
	{
		if (!$this->id) return false;
		return pmm_core::getLinkedFiles(2, $this->id);
	}
	public function delete()
	{
		$files = $this->getFiles();
		
		foreach((array)$files as $fileObj)
		{
			$fileObj->delete();	
		}
		
		return parent::delete();	
	}
	public static function getQryObject($whereFragment="", $orderFragment="meetingDateTime asc", $limitFragment="")
	{
		$d = new DAL(true);
		
		if ($whereFragment) $wh = "where $whereFragment";
		if ($orderFragment) $order = "order by $orderFragment";
		if ($limitFragment) $limit = "limit $limitFragment";
		
		$qry = "select id
				from pmm_entityAgendas
				$wh
				$order
				$limit";
				
		return $d->qry($qry, false);	
	}
}
?>