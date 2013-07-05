<?php
class customContent_core
{
	const content_generic = 0;
	const content_municipal_entity = 1;	
		
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
			case customContent_core::content_generic:
				return $sec->hasAccess(permissions::$manage_custom_content, $adminMask ? $adminMask : $permissionMask);				
				break;	
				
			case customContent_core::content_municipal_entity:
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
} 
?>