<?php
class customCategory_core
{
	public $files = array("categoryItems.php", "categoryTypes.php", "categoryItems.ajax", "categoryTypes.ajax");	
	public $tables = array("categoryFeature"=>"CREATE TABLE `categoryFeature` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `categoryTypeID` int(11) NOT NULL DEFAULT '0',
											 `fieldName` varchar(50) NOT NULL DEFAULT '',
											 `featureType` int(11) NOT NULL DEFAULT '0',
											 `featureConfigData` text NOT NULL,
											 `featureKey` varchar(50) NOT NULL DEFAULT '',
											 `position` int(11) NOT NULL DEFAULT '0',
											 `featureRequired` smallint(6) NOT NULL,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryFeatureLookup"=>"CREATE TABLE `categoryFeatureLookup` (
													 `id` int(11) NOT NULL AUTO_INCREMENT,
													 `categoryItemID` int(11) NOT NULL DEFAULT '0',
													 `categoryFeatureID` int(11) NOT NULL DEFAULT '0',
													 `value` text NOT NULL,
													 `additionalData` text NOT NULL,
													 PRIMARY KEY (`id`)
													) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryItems"=>"CREATE TABLE `categoryItems` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `categoryTypeID` int(11) NOT NULL DEFAULT '0',
											 `title` varchar(255) NOT NULL DEFAULT '',
											 `description` text NOT NULL,
											 `lookupKey` varchar(50) NOT NULL,
											 `inactive` int(11) NOT NULL DEFAULT '0',
											 `position` int(11) NOT NULL DEFAULT '0',
											 `addedDateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryKeyLookup"=>"CREATE TABLE `categoryKeyLookup` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `categoryItemID` int(11) NOT NULL,
												 `keyID` int(11) NOT NULL,
												 `keyData` varchar(255) NOT NULL,
												 `position` int(11) NOT NULL DEFAULT '0',
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryKeyOptions"=>"CREATE TABLE `categoryKeyOptions` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `categoryTypeID` int(11) NOT NULL,
												 `value` varchar(255) NOT NULL,
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryTypes"=>"CREATE TABLE `categoryTypes` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `name` varchar(50) NOT NULL DEFAULT '',
											 `description` text NOT NULL,
											 `image` varchar(255) NOT NULL DEFAULT '',
											 `inactive` smallint(6) NOT NULL DEFAULT '0',
											 `parserPage` varchar(50) NOT NULL DEFAULT '',
											 `lookupKey` varchar(50) NOT NULL,
											 `position` int(11) NOT NULL DEFAULT '0',
											 `descIsHtml` smallint(6) NOT NULL,
											 `exposeAsKeys` int(11) NOT NULL DEFAULT '0',
											 `sortMode` int(11) NOT NULL DEFAULT '0',
											 `hideSubKeys` tinyint(4) NOT NULL DEFAULT '0',
											 `disableDelete` tinyint(4) NOT NULL DEFAULT '0',
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8");
}
class categoryFeatureFile
{
	var $fileName;
	var $caption;	
	var $index;
	var $fileSize;
	var $url;

	public function __construct($fileName, $caption, $index)
	{
		global $config;
		
		$this->fileName = $fileName;
		$this->caption = $caption;	
		$this->index = $index;
		
		$this->fileSize = @filesize($config[baseAppDir].'images/categoryItems/'.$fileName);
		$this->url = 'images/categoryItems/'.$fileName;
	}
}
class categoryFeature
{
	private $categoryItemID;
	private $featureID;
	private $categoryTypeID;
	
	var $feature = array();
	var $data = array();
	var $files = array();
	private $sortDirection = "asc";
	private $modifiedKeys = array();
	
	public function categoryFeature($categoryTypeID, $categoryItemID, $featureID)
	{
		$d = new DAL();
		
		validation::ensureInt($categoryTypeID);
		validation::ensureInt($categoryItemID);
		validation::ensureInt($featureID);
		
		if (!$featureID) throw new Exception("Error, no feature provided!");
		if (!$categoryItemID) throw new Exception("Error, no category item specified!");
		if (!$categoryTypeID) throw new Exception("Error, no category type specified!");
		
		$this->feature = $d->qryArray("select * from categoryFeature where categoryTypeID = $categoryTypeID and id = $featureID");
		$this->data = $d->qryArray("select * from categoryFeatureLookup where categoryItemID = $categoryItemID and categoryFeatureID = $featureID");
				
		if (!$this->feature[id]) throw new Exception("Error, that category feature cannot be found!");
		
		$this->categoryItemID = $categoryItemID;
		$this->featureID = $featureID;
		$this->categoryTypeID = $categoryTypeID;
		
		$this->data[raw_value] = $this->data[value];
		
		//Only prep a regular text field for display
		$this->data[rawValue] = $this->data[value];
		if ($this->feature[featureType] == 1) $this->data[value] = validation::prepForDisplay($this->data[value]);
		
		//Combine the two for complete data
		$this->data[featureLookupID] = $this->data[id];
		$this->data = array_merge($this->data, $this->feature);
		
		if ($this->feature[featureType] == customCategoryTypes::downloadable_file && $this->feature[featureConfigData] == 1 && $this->data[value])
		{			
			//split out multiple files
			$files = explode("::", $this->data[value]);
			$captions = explode("::", $this->data[additionalData]);
			
			for ($i=0; $i < count($files); $i++)
			{	
				$this->files[$i] = new categoryFeatureFile($files[$i], $captions[$i], $i);
			}
		}
		elseif($this->feature[featureType] == customCategoryTypes::blog_category || $this->feature[featureType] == customCategoryTypes::blog_topic)
		{
			//Separate the topics, or categories, by the internal delimeter. Sets the value to an indexed array of the corresponding data.
			if ($this->data[value])
			{
				$this->data[value] = explode("::", $this->data[value]);
			}
			else
			{
				$this->data[value] = array();
			}
		}
	}	
	public function __get($key)
	{
		if (isset($this->data[$key]))
		{
			return $this->data[$key];			
		}
		else
		{
			return false;	
		}
	}
	public function __set($key, $value)
	{
		$this->touch($key);		
		$this->data[$key] = $value;
	}
	public function touch($key='value')	
	{
		// This function will mark the specified key as 'dirty' so that the save routine picks it up. 
		// This is necessary for features that have an array as their value (using the :: delimeter)
		
		if (!in_array($key, $this->modifiedKeys)) $this->modifiedKeys[] = $key;
	}
	public function sortFeatureFiles($sortMode=1, $direction="asc")
	{
		//sortMode:
		//1 = caption
		//2 = file name
		//3 = file size
		
		//direction:
		//asc
		//other = desc
		
		switch($sortMode)
		{
			case 1:
				$func = "sortByCaption";
				break;				
			case 2:
				$func = "sortByName";
				break;
			case 3:
				$func = "sortBySize";
				break;
		}
		
		$this->sortDirection = $direction;
		usort($this->files, array($this, $func));
		
	}
	private function sortByCaption($a, $b)
	{
		if ($this->sortDirection == "asc") return strcmp(strtolower($a->caption), strtolower($b->caption));	
		return strcmp(strtolower($b->caption), strtolower($a->caption));
	}
	private function sortByName($a, $b)
	{
		if ($this->sortDirection == "asc") return strcmp(strtolower($a->fileName), strtolower($b->fileName));	
		return strcmp(strtolower($b->fileName), strtolower($a->fileName));
	}
	private function sortBySize($a, $b)
	{
		if ($this->sortDirection == "asc") return $a->fileSize - $b->fileSize;
		return !($a->fileSize - $b->fileSize);
	}
	public function saveData()
	{
		$d = new DAL(true);

		foreach($this->modifiedKeys as $key)
		{			
			if (is_array($this->$key))
			{
				//Handle types that have multi-assignment using :: delimeter - re-form the proper string for storage.
				$tmp = implode("::", $this->$key);
				$cv[$key] = $tmp;
			}
			else 
			{
				$cv[$key] = $this->$key;
			}				
		}		
		
		if (!count($cv)) throw new Exception("Error, nothing to update!");
		
		$qCV = validation::quotedArray($cv);
		
		if (!$this->featureLookupID)
		{	
			if (!$qCV[categoryItemID]) $qCV[categoryItemID] = $this->categoryItemID;
			if (!$qCV[categoryFeatureID]) $qCV[categoryFeatureID] = $this->featureID;
			
			$lookupID = $d->qryInsertByArray("categoryFeatureLookup", $qCV);
			if (!$lookupID) throw new Exception("Error adding category feature lookup data. Please try again.");
		}
		else
		{						
			$d->qryUpdateByArray("categoryFeatureLookup", $qCV, "id=$this->featureLookupID");	
		}
	}	
	public function deleteLookup()
	{
		if (!$this->featureLookupID) return false;
		
		$d = new DAL(true);
		return $d->qry("delete from categoryFeatureLookup where id = {$this->featureLookupID}");
	}
}
class categoryItem
{
	var $item = array();
	var $features = array();
	var $subKeys = array();
	
	public function categoryItem($categoryItemID, $wysiwygText=false, $populateFeatures = true)
	{
		$d = new DAL();
		
		validation::ensureInt($categoryItemID);
		if (!$categoryItemID) throw new Exception("Error, no category item information provided!");
		
		//Load the item data
		$this->item = $d->qryArray("select * from categoryItems where id=$categoryItemID");
		
		if (!$this->item[id]) throw new Exception("Error, that category item cannot be found!");
		
		if ($wysiwygText) $this->item[description] = stripslashes($this->item[description]);
		
		if ($populateFeatures)
		{
			$qry = "select id, featureKey
					from categoryFeature
					where categoryTypeID = {$this->item[categoryTypeID]}
					order by position asc";
			$fQry = $d->qry($qry);
			
			$ct = 0;
			while($fea = mysqli_fetch_assoc($fQry))
			{
				if (!$fea[featureKey]) $fea[featureKey] = $ct++;
				$this->features[$fea[featureKey]] = new categoryFeature($this->item[categoryTypeID], $categoryItemID, $fea[id]);
			}
		}		
	}	
	public function populateSubKeys($keyIDFilter='', $orderByPosition = false)
	{	
		$d = new DAL();
		
		if ($keyIDFilter) $whClause = "and keyID = '$keyIDFilter'";
		$order = "id asc";
		if($orderByPosition) $order = "position asc, id asc";
		//Load secondary Key data
		$kQry = $d->qry("select categoryKeyLookup.id, keyID, keyData, categoryItemID
					 	 from categoryKeyLookup
					 	 where categoryItemID = {$this->item[id]} $whClause
					 	 order by $order");	
		$sCt = 0;
		while($kData = mysqli_fetch_assoc($kQry))
		{	
			$this->subKeys[$sCt++] = $kData;
		}
	}
	public function __get($key)
	{
		if (isset($this->item[$key]))
		{
			return $this->item[$key];			
		}	
		else
		{
			return false;	
		}
	}
	public static function getIDByLookupKey($key)
	{
		validation::ensureString($key);
		if (!$key) return false;
		
		$d = new DAL();
		return $d->qryCount("select id from categoryItems where lookupKey = '$key'");	
	}
}
class customCategoryTypes
{
	const text_field = 1;
	const hyperlink = 2;
	const image = 3;
	const downloadable_file = 4;
	const wysiwyg_text = 5;
	const showcase = 6;
	const date_field = 7;
	const html_literal = 8;
	const custom_form = 9;
	const blog_topic = 10;
	const blog_category = 11;
	const inventory_type = 12; 
	const inventory_manufacturer = 13;  	
}
class customCategory
{
	var $catID;
	var $categoryKeyFilter;
	var $category = array();
	var $categoryItems = array();	
	var $itemKeyFilter;
	var $getInactive;	
	var $itemIDFilter;
	var $itemSecondaryKeyFilter;
	
	public function customCategory($catID = "", $categoryKeyFilter= "", $itemKeyFilter="", $no_populate=false, $get_inactive = false, $itemIDFilter="", $itemSecondaryKeyFilter="")
	{
		validation::ensureInt($catID);
		
		$this->catID = $catID;
		$this->categoryKeyFilter = $categoryKeyFilter;
		$this->itemKeyFilter = $itemKeyFilter;
		$this->getInactive = $get_inactive ? 1 : 0;
		$this->itemIDFilter = $itemIDFilter;
		$this->itemSecondaryKeyFilter = $itemSecondaryKeyFilter;
		
		if ($this->catID || $this->categoryKeyFilter) $this->populateCategory($no_populate);		
	}	
	public function populateCategory($no_populate=false)
	{
		$d = new DAL();
		
		$whClause = ($this->catID ? "id = {$this->catID}" : "lookupKey = '{$this->categoryKeyFilter}'");		
		
		$qry = "select *
				from categoryTypes
				where $whClause";	
		
		$this->category = $d->qryArray($qry);
		
		if (!$no_populate && $this->category) $this->populateCategoryItems();
	}
	public function populateCategoryItems($orderBy="", $startPos=0, $limit=0)
	{
		// Sort Mode:
		// 0: Sort by position, ascending
		// 1: Sort by addedDateTime descending
		$d = new DAL();
		
		if ($limit) $limitClause = "limit ".($startPos ? $startPos.", " : "").$limit;
		
		if ($this->itemKeyFilter) $whClause = " and categoryItems.lookupKey = '{$this->itemKeyFilter}'";
		if ($this->itemIDFilter) $whClause .= " and categoryItems.id=".$this->itemIDFilter;
		
		if ($this->itemSecondaryKeyFilter)
		{
			$join = "inner join categoryKeyLookup on categoryKeyLookup.categoryItemID = categoryItems.id ";
			$whClause .= " and categoryKeyLookup.keyID = ".$this->itemSecondaryKeyFilter;
		}
		
		if (!$orderBy) 
		{
			if (!$this->category[sortMode]) $this->category[sortMode] = 0;
			$sortMode = $this->category[sortMode];
			
			switch($sortMode)
			{
				case 0:
					$orderBy = "categoryItems.position asc";
					break;
					
				case 1:					
					$orderBy = "addedDateTime desc";
					break;
				
				case 2:
					$orderBy = "title asc";
					break;
			}
		}
		
		$qry = "select categoryItems.id
				from categoryItems $join
				where categoryItems.categoryTypeID = {$this->category[id]} and categoryItems.inactive = {$this->getInactive} $whClause
				order by $orderBy
				$limitClause";
		
		$cQry = $d->qry($qry);
		
		$ct=0;
		while($cItem = mysqli_fetch_assoc($cQry))
		{
			$this->categoryItems[$ct++] = new categoryItem($cItem[id], $this->descIsHtml);
		}		
	}
	public function __get($value)
	{
		if (isset($this->category[$value]))
		{
			return $this->category[$value];				
		}
		else
		{
			return false;	
		}
	}
}
?>