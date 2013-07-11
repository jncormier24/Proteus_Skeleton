<?php
class customContent_core
{
	const content_generic = 0;
	const content_municipal_entity = 1;	
		
	public $asset_folders = array("contentPages");	
	
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
	public static function checkPermission(security $sec, $typeID, $dataID, $permissionMask='', $adminMask='')
	{		
		if (!$permissionMask && !$adminMask) return false;
		
		switch($typeID)
		{
			case proteus_core::$module_customContent["siteType"]:				
				return $sec->hasAccess(permissions::$manage_custom_content, $adminMask ? $adminMask : $permissionMask);				
								
			case proteus_core::$module_municipal["siteType"]:
				$obj = new pmm_entity($dataID);
				
				return $sec->hasAccess(permissions::$manage_municipal_entities, $adminMask ? $adminMask : $permissionMask) ||
					   ($permissionMask && $sec->hasAccess(permissions::$municipal_entity, $permissionMask, $obj->id));				
			default:
				return false;
		}			
	}	
}
class customContentData
{
	private $contentData = array();

	public function __construct($contentID='', $contentName='')
	{
		$d = new DAL();
			
		$baseQry = "select *
					from customContent
					where ";
		
		if ($contentID)
		{
			validation::ensureInt($contentID);
			$baseQry .= "id=$contentID";	
		}
		elseif ($contentName)
		{
			validation::ensureString($contentName);
			$baseQry .= "pageName='$contentName'";
		}
		else
		{
			throw new Exception("Error, nothing to query!");	
		}
		
		$this->contentData = $d->qryArray($baseQry);		
	}
	public function __get($keyName)
	{
		return $this->contentData[$keyName];	
	}	
	public static function getDataObject($typeID, $dataID, $contentID="")
	{
		$data = new stdClass();
		
		switch($typeID)
		{
			case -1:
				// Municipal FAQ Entries
				$data = new pmm_entityFaq($dataID);
				$imgDir = "municipal/$dataID/faq/$contentID";
				
				break;				
			case 1:	
				$data = new categoryItem($dataID);
				$imgDir = "images/categoryItems/$dataID";
				break;
				
			case 2:
				//Inline Edit images		
				$imgDir = "images/contentPages";
				break;
			
			case 3:				
				// Blog Entries				
				$entry = new customBlogEntry($dataID);
				$imgDir = "images/blogs/$dataID";
				break;
				
			case 4:
				// Custom Forms (description)
				$imgDir = "images/forms/$dataID";
				break;
				
			case proteus_core::$module_customContent["siteType"]:
				
				$data = new customContentData($dataID);				
				$imgDir = "contentPages/$dataID";
				$module = proteus_core::$module_customContent;				
				
				break;				
			case proteus_core::$module_municipal["siteType"]:

				$data = new pmm_entity($dataID);
				
				$imgDir = "municipal/$dataID/content/$contentID";
				$module = proteus_core::$module_municipal;
				
				break;			
			case proteus_core::$module_calendars["siteType"]:
				// Calendar Entries
				
				$data = new customCalendar($dataID, false);
				$imgDir = "calendars/$dataID/content/$contentID";
				$module = proteus_core::$module_calendars;
				
				break;				
			default:
				throw new Exception("Invalid upload type specified!");					
		}		
		
		$data->module = $module;
		$data->assetDir = $imgDir;
		
		return $data;
	}
} 
?>