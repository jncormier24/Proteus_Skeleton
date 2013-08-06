<?php
class customContent_core
{
	const content_generic = 0;
	const content_municipal_entity = 1;	
		
	public $asset_folders = array("customContent");	
	
	public $tables = array("customContent"=>"CREATE TABLE `customContent` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `dataID` int(11) NOT NULL DEFAULT '0',
											 `typeID` int(11) NOT NULL DEFAULT '0',
											 `pageName` varchar(255) NOT NULL DEFAULT '',
											 `pageData` text NOT NULL,
											 `caption` varchar(50) NOT NULL DEFAULT '',
											 `menuInclude` tinyint(4) NOT NULL DEFAULT '0',
											 `position` int(11) NOT NULL DEFAULT '0',
											 `updatedUserID` int(11) NOT NULL DEFAULT '0',
											 `updatedDateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',											 
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
						   "inlineContent"=>"CREATE TABLE `inlineContent` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `contentKey` varchar(255) NOT NULL,
											 `typeID` int(11) NOT NULL,
											 `content` text NOT NULL,
											 `lastAccessed` datetime NOT NULL,
											 `lastModified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8");	
	
	public static function getContentEntries($typeID, $dataID, $startPos=0, $length=0, &$totalCount='')
	{
		validation::ensureInt($typeID);
		validation::ensureInt($dataID);
		
		$d = new DAL(true);
		$arr = array();
		
		if ($startPos) $lmt = "limit $startPos, $length";
		
		$qry = "select id
				from customContent
				where typeID=".customContent_core::content_municipal_entity." and dataID=$dataID
				order by position asc
				$lmt";
				
		$totalCount = $d->getTotalCount($qry);
				
		$eQry = $d->qry($qry);
			
		while($content = $eQry->fetch_assoc())
		{
			$cObj = new customContentData($content[id]);
			$arr[$cObj->pageName] = $cObj;
		}		
			
		return $arr;	
	}	
	public static function checkPermission(security $sec, $typeID, $dataID, $permissionMask = 1, $globalMask = "")
	{		
		if (!$permissionMask && !$globalMask) return false;
		
		switch($typeID)
		{
			case proteus_core::$module_customContent["siteType"]:				
				return $sec->hasAccess(permissions::$manage_custom_content, $globalMask ? $globalMask : $permissionMask);				
								
			case proteus_core::$module_municipal["siteType"]:
				$obj = new pmm_entity($dataID);
				
				return $sec->hasAccess(permissions::$manage_municipal_entities, $globalMask ? $globalMask : $permissionMask) ||
					   ($permissionMask && $sec->hasAccess(permissions::$municipal_entity, $permissionMask, $obj->id));				
			default:
				return false;
		}			
	}	
}
class customContentData extends phpDataSet
{	
	public function __construct($contentID=0)
	{
		parent::__construct("customContent", $contentID, $contentID ? true : false);
	}
	public static function getDataObject($typeID, $dataID, $contentID="", security $sec = null)
	{
		$data = new stdClass();
		
		switch($typeID)
		{							
			case -5:
				// Blog Entry
				$data = new customBlogEntry($contentID);
				$data->hasAccess($sec, permissions::write, true);
				
				break;
			case -4:
				// Municipal Entity
				$data = new pmm_entity($dataID);
				$data->hasAccess($sec, permissions::write, true);
				
				break;
			case -3:
				// Municipal Agendas
				$data = new pmm_entityAgenda($contentID);
				$data->hasAccess($sec, permissions::write, true);
				
				break;
			case -2:
				// Municipal Post Entries
				$data = new pmm_entityPosting($contentID);
				$data->hasAccess($sec, permissions::write, true);
				
				break;
			case -1:
				// Municipal FAQ Entries
				$data = new pmm_entityFaq($dataID);
				$data->hasAccess($sec, permissions::write, true);
				
				break;				
			/* case proteus_core::$module_customCategories["siteType"]:
				$data = new categoryItem($dataID);
				$imgDir = "images/categoryItems/$dataID";
				break; */				
			
			case proteus_core::$module_blogs["siteType"]:				
				// Blog Category				
				$data = new customBlog($dataID);
				$data->hasAccess($sec, permissions::write, true);
				
				$module = proteus_core::$module_blogs;
				break;
				
			case proteus_core::$module_customForms["siteType"]:
				// Custom Forms (description)
				$data = new customForm($dataID);
				$data->hasAccess($sec, permissions::write, true);
				
				$module = proteus_core::$module_customForms;
				break;
				
			case proteus_core::$module_customContent["siteType"]:
				
				$data = new customContentData($contentID);	
				$data->hasAccess($sec, permissions::write);							

				$module = proteus_core::$module_customContent;				
				break;				
			case proteus_core::$module_municipal["siteType"]:

				$data = new pmm_entity($dataID);
				$data->hasAccess($sec, permissions::write, true);				

				$module = proteus_core::$module_municipal;
				break;			
			case proteus_core::$module_calendars["siteType"]:
				// Calendar Entries
				
				$data = new customCalendar($dataID);
				$data->hasAccess($sec, permissions::write, true);
				
				$module = proteus_core::$module_calendars;
				break;	
			case proteus_core::$module_showcases["siteType"]:
				// Custom Showcase
				$data = new customShowcase($dataID);
				$data->hasAccess($sec, permissions::write, true);

				$module = $module = proteus_core::$module_showcases;				
				break;
			default:
				throw new Exception("Invalid type specified!");					
		}		
		
		$data->module = $module;
		$data->assetDir = $data->assetFolder;		
		
		return $data;
	}
	public function __get($key)
	{		
		global $config;
		
		if (!parent::__get("id")) return false;
		
		switch ($key)
		{
			case "assetFolder":
				return "assets/customContent/{$this->id}";
		}
		
		return parent::__get($key);	
	}
	public function insert($data="")
	{
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from customContent where dataID={$this->dataID} and typeID={$this->typeID}");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		$id = parent::insert($data);
		
		$loc = $config["baseAppDir"].$this->assetFolder;
		if (!file_exists($loc)) mkdir($loc);		
		
		return $id;
	}
	public static function getContentContainer($typeID, $dataID=0)
	{
		validation::ensureInt($dataID);
		validation::ensureInt($typeID);
		
		// Setup the container object to receive the list of content pages (don't care of this method, but it works)
		$dv = new phpDIV();
		$dv->id = "container_{$typeID}_{$dataID}";
		
		$dv->addControl(new jqLoader("getContentEntries({$typeID},{$dataID});"));
		
		return $dv;
	}
	public function hasAccess(security $sec, $permission, $globalPermission = 0, $throwException = false)
	{
		$access = customContent_core::checkPermission($sec, $this->typeID, $this->dataID, $permission, $globalPermission);
		
		if (!$access && $throwException) $sec->security_error_a();
		
		return $access;
	}
} 
?>