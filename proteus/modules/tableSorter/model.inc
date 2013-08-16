<?php

/*
 * Table Sorter Module. This module is designed to provide a unified interface for sorting objects within tables. It supports paged tables, 
 * however to do that, consideration needs to be taken for tables that use a typeID/dataID structure. This module will assume a table column
 * of "typeID" to distinguish if the current sort request should be quantified by a specific type. If the returned object has a typeID value,
 * the sorter will intelligently align the other rows in the table based on that typeID only. 
 *   
 */

class tableSorter_core
{
	// These are the objects that this module supports. This is not intended to be extensible, but rather a single location to manage all
	// administrative sorting within the proteus admin module (though this can also be used in front-end pages that implement admin functionality)
	
	public static $sort_municipal_staff = array("sortType"=>10, "dataTable"=>"pmm_entityStaff");
	public static $sort_municipal_faq = array("sortType"=>11, "dataTable"=>"pmm_entityFaq");
	public static $sort_blog_category = array("sortType"=>5, "dataTable"=>"blogs");
	public static $sort_blog_feed = array("sortType"=>9, "dataTable"=>"blogFeeds");
	public static $sort_content_page = array("sortType"=>3, "dataTable"=>"customContent");
	public static $sort_showcase = array("sortType"=>4, "dataTable"=>"customShowcase");
	public static $sort_showcase_item = array("sortType"=>12, "dataTable"=>"showcaseItems");
	public static $sort_form_field = array("sortType"=>6, "dataTable"=>"customFormFields");
	public static $sort_category_feature = array("sortType"=>7, "dataTable"=>"categoryFeatures");
	
	public static function getSortTypes()
	{
		$permObj = new ReflectionClass("tableSorter_core");
		$permArr = $permObj->getStaticProperties();
		
		ksort($permArr);

		return $permArr;
	}
	public static function getSortByType($sortType)	
	{
		$pArr = tableSorter_core::getSortTypes();
		
		foreach($pArr as $sort)
		{
			if ($sort["sortType"] == $sortType) return $sort;	
		}
		
		return false;
	}	
	public static function getSortableObject($sortType, $dataID, &$quantifyArray="")
	{
		if (is_array($sortType)) $sortType = $sortType["sortType"];
		if (!is_array($quantifyArray)) $quantifyArray = array();
		
		switch($sortType)
		{
			case tableSorter_core::$sort_category_feature["sortType"]:
				$obj = new categoryFeature($dataID);
				$quantifyArray[] = "categoryID={$obj->categoryID}";
				
				return $obj;
				
			case tableSorter_core::$sort_form_field["sortType"]:
				$obj = new customFormField($dataID);				
				$quantifyArray[] = "formID={$obj->formID}";
				
				return $obj;
				
			case tableSorter_core::$sort_showcase_item["sortType"]:
				$obj = new customShowcaseItem($dataID);
				$quantifyArray[] = "showcaseID={$obj->showcaseID}";
			
				return $obj;
				
			case tableSorter_core::$sort_showcase["sortType"]:
				return new customShowcase($dataID);
				
			case tableSorter_core::$sort_content_page["sortType"]:
				return new customContentData($dataID);
				
			case tableSorter_core::$sort_blog_feed["sortType"]:
				return new customBlogFeed($dataID, false);
				
			case tableSorter_core::$sort_blog_category["sortType"]:
				return new customBlog($dataID);
				
			case tableSorter_core::$sort_municipal_staff["sortType"]:
				$obj = new pmm_entityStaff($dataID);
				$quantifyArray[] = "entityID={$obj->entityID}";
				
				return $obj;
				
			case tableSorter_core::$sort_municipal_faq["sortType"]:
				$obj = new pmm_entityFaq($dataID);
				$quantifyArray[] = "entityID={$obj->entityID}";
				
				return $obj; 
		}
		
		return false;
	}
	public static function hasAccess(security $sec, $sortType, $obj)
	{
		if (is_array($sortType)) $sortType = $sortType["sortType"];
		
		switch($sortType)
		{	
			case tableSorter_core::$sort_category_feature["sortType"]:
				return $sec->hasAccess(permissions::$manage_category_types, permissions::write) || $sec->hasAccess(permissions::$modify_category_type, permissions::write, $obj->categoryID);
				
			case tableSorter_core::$sort_form_field["sortType"]:
				return $sec->hasAccess(permissions::$manage_custom_forms, permissions::write) || $sec->hasAccess(permissions::$modify_custom_form, permissions::write, $obj->formID);
				
			case tableSorter_core::$sort_showcase_item["sortType"]:
				return $sec->hasAccess(permissions::$manage_showcases, permissions::write) || $sec->hasAccess(permissions::$modify_showcase, permissions::write, $obj->showcaseID);
								
			case tableSorter_core::$sort_showcase["sortType"]:				
				return $sec->hasAccess(permissions::$manage_showcases, permissions::write);
				
			case tableSorter_core::$sort_content_page["sortType"]:				
				return customContent_core::checkPermission($sec, $obj->typeID, $obj->dataID, permissions::write, permissions::write);
				
			case tableSorter_core::$sort_blog_feed["sortType"]:
				return $sec->hasAccess(permissions::$manage_blog_feeds, permissions::write);
				
			case tableSorter_core::$sort_blog_category["sortType"]:
				return $sec->hasAccess(permissions::$manage_blogs, permissions::write);
				
			case tableSorter_core::$sort_municipal_faq["sortType"]:
			case tableSorter_core::$sort_municipal_staff["sortType"]:				
				return $sec->hasAccess(permissions::$manage_municipal_entities, permissions::write) || $sec->hasAccess(permissions::$municipal_entity, permissions::write, $obj->entityID); 
		}
		
		return false;
	}
	public static function realignSorts($sortType, $quantifiers = array())
	{
		$d = new DAL(true);
				
		$wh = count($quantifiers) ? "and ".implode(" and ", $quantifiers) : "";

		$qry = "select id 
				from {$sortType["dataTable"]}
				where 1 $wh 
				order by position asc";
		$eQry = $d->qry($qry);
		
		$ct = 1;
		while($entry = $eQry->fetch_assoc())
		{
			$cv["position"] = $ct++;
			$d->qryUpdateByArray($sortType["dataTable"], $cv, "id={$entry["id"]}");
		}
	}
} 
class sortable
{
	private $p_type;
	private $curObj;
	private $replaceObj;	
	private $quantifier = array();
	
	public function __construct($sec, $sortType, $sortID, $replacementID)
	{				
		validation::ensureInt($sortType);
		if (!$sortType) throw new Exception("Error, no sort type specified.");
		
		validation::ensureInt($replacementID);
		if (!$replacementID) throw new Exception("Error, no position specified. Please try again.");
		
		validation::ensureInt($sortID);
		if (!$sortID) throw new Exception("Error, no element specified. Please try again.");
		
		$this->p_type = tableSorter_core::getSortByType($sortType);		
		
		$this->curObj = tableSorter_core::getSortableObject($this->p_type, $sortID, $quantifier);
		$this->replaceObj = tableSorter_core::getSortableObject($this->p_type, $replacementID);
		
		if (!tableSorter_core::hasAccess($sec, $this->p_type, $this->curObj) || !tableSorter_core::hasAccess($sec, $this->p_type, $this->replaceObj)) throw new Exception("You do not have access to sort these items.");
	
		// Check if the objects have a typeID quantifier, make accomodations if there is (so that aligned rows will limit to the correct quantifier)
		if ($curObj->typeID || $replaceObj->typeID)
		{
			// Make sure the 2 objects are of the same type
			if ($replaceObj->typeID != $curObj->typeID) throw new Exception("Mismatched sort request. Please try again.");
			
			// Restrict the updates to this quantifier
			$this->quantifier[] = "typeID={$curObj->typeID}";
		}
		
		// Loop through any quantifiers set in the getSortableObject call (passed by reference)
		foreach($quantifier as $q)
		{
			$this->quantifier[] = $q;
		}
		
		// Take inactive items into consideration as a quantifier (if the table structure includes that column)
		// SWB 7/13: I don't think this is entirely necessary - leaving out for now
		//if (isset($curObj->inactive)) $this->quantifier["inactive"] = $curObj->inactive;
	}	
	public function sort()
	{
		$d = new DAL(true);
				
		$currentPos = $this->curObj->position;
		$newPos = $this->replaceObj->position;
		
		if ($newPos > $currentPos)
		{
			$cv["position"] = "position - 1";
			$wh[] = "(position > $currentPos and position <= $newPos)";
		}
		else
		{
			$cv["position"] = "position + 1";
			$wh[] = "(position >= $newPos and position < $currentPos)";			
		}
		
		$wh[] = "id <> {$this->curObj->id}";
		
		// Merge the where argument above with the internal quantifier
		$wh = array_merge($wh, $this->quantifier);
		//throw new Exception(nl2br(print_r($wh, true)));
		// Realign the affected elements in the data table
		$d->qryUpdateByArray($this->p_type["dataTable"], $cv, $wh);
		
		// Set the current objec to the new position
		$this->curObj->position = $newPos;
		$this->curObj->save();
		
		// Realign all the items in the table just to be safe
		tableSorter_core::realignSorts($this->p_type, $this->quantifier);
	} 
}
?>