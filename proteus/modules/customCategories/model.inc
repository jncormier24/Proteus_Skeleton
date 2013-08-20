<?php
class customCategory_core 
{	
	public $asset_folders = array("categories");	
	public $tables = array("categoryFeatures"=>"CREATE TABLE `categoryFeatures` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `categoryID` int(11) NOT NULL DEFAULT '0',
											 `fieldName` varchar(50) NOT NULL DEFAULT '',
											 `featureType` int(11) NOT NULL DEFAULT '0',
											 `featureConfigData` text NOT NULL,
											 `featureKey` varchar(50) NOT NULL DEFAULT '',
											 `position` int(11) NOT NULL DEFAULT '0',
											 `featureRequired` smallint(6) NOT NULL,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryFeatureData"=>"CREATE TABLE `categoryFeatureData` (
													 `id` int(11) NOT NULL AUTO_INCREMENT,
													 `categoryItemID` int(11) NOT NULL DEFAULT '0',
													 `categoryFeatureID` int(11) NOT NULL DEFAULT '0',
													 `value` text NOT NULL,
													 `additionalData` text NOT NULL,
													 PRIMARY KEY (`id`)
													) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryItems"=>"CREATE TABLE `categoryItems` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `categoryID` int(11) NOT NULL DEFAULT '0',
											 `title` varchar(255) NOT NULL DEFAULT '',
											 `description` text NOT NULL,
											 `lookupKey` varchar(50) NOT NULL,
											 `inactive` tinyint(4) NOT NULL DEFAULT '0',
											 `position` int(11) NOT NULL DEFAULT '0',
											 `addedDateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
											 `updatedDateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
											 `updatedUserID` int(11) NOT NULL DEFAULT '0',
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryKeyLookup"=>"CREATE TABLE `categoryKeyLookup` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `categoryItemID` int(11) NOT NULL,
												 `categoryID` int(11) NOT NULL,
												 `keyData` varchar(255) NOT NULL,
												 `position` int(11) NOT NULL DEFAULT '0',
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categoryExposure"=>"CREATE TABLE `categoryExposure` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `categoryID` int(11) NOT NULL,
												 `linkedCategoryID` int(11) NOT NULL,
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"categories"=>"CREATE TABLE `categories` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `name` varchar(50) NOT NULL DEFAULT '',
											 `description` text NOT NULL,
											 `sortMode` tinyint(4) NOT NULL DEFAULT '0',											 											 
											 `position` int(11) NOT NULL DEFAULT '0', 
											 `inactive` tinyint(4) NOT NULL DEFAULT '0',
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8");
}
class customCategory extends phpDataset
{	
	private $p_features;
	private $p_items;
	
	public function __construct($id)
	{		
		parent::__construct("categories", $id, $id ? true : false);		
	}	
	public function __get($key)
	{		
		global $config;		
		
		switch ($key)
		{
			case "items":
				if (!$this->p_items) $this->p_items = $this->getItems();
				return $this->p_items;
				
			case "features":
				if (!$this->p_features) $this->p_features = $this->getFeatures();
				return $this->p_features;
				
			case "assetFolder":
				return "assets/categories/{$this->id}";
		}
		
		return parent::__get($key);	
	}
	public function getFeatures()
	{
		if (!$this->id) return false;
	
		$d = new DAL(true);
		$features = Array();
		
		$qry = "select id
				from categoryFeatures
				where categoryID = {$this->id}
				order by position";
		$fQry = $d->qry($qry);
	
		while($f = $fQry->fetch_assoc())
		{
			$features[$f["id"]] = new categoryFeature($f["id"]);
		}
		
		return $features;
	}
	public function getItems()
	{
		if (!$this->id) return false;
	
		$d = new DAL(true);
		$items = Array();
		
		$sortOrd = $this->sortMode ? "addedDateTime desc" : "position asc"; 
		
		$qry = "select id
				from categoryItems
				where categoryID = {$this->id}
				order by $sortOrd";
		$fQry = $d->qry($qry);
	
		while($i = $fQry->fetch_assoc())
		{
			$items[$i["id"]] = new categoryItem($i["id"]);
		}
		
		return $items;	
	}	
	public function hasAccess(security $sec, $permission, $throwException = false)
	{
		$access = $sec->hasAccess(permissions::$manage_category_types, $permission > permissions::read ? permissions::write : $permission) || 
			   	  $sec->hasAccess(permissions::$modify_category_type, $permission, $this->id);
		
		if (!$access && $throwException) $sec->security_error_a();
		
		return $access;
	}
	public function insert($data="")
	{
		global $config;
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from categories");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		$id = parent::insert($data);
		
		$loc = $config["baseAppDir"].$this->assetFolder;
		if (!file_exists($loc)) mkdir($loc);		
		
		return $id;
	}
	public function delete()
	{
		$d = new DAL(true);
		
		foreach($this->items as $itemObj)
		{
			$itemObj->delete();
		}
		
		$d->qry("delete from categoryExposure where categoryID = {$this->id} or linkedCategoryID = {$this->id}");
		$d->qry("delete from categoryKeyLookup where categoryID = {$this->id}");
		
		$baseLoc = $config["baseAppDir"].$this->assetFolder;
		
		foreach(glob($baseLoc."/*") as $file)
		{
			@unlink($file);
		}
		
		@rmdir($baseLoc);
		
		return parent::delete();
	}
	public function isExposed($categoryID)
	{
		validation::ensureInt($categoryID);
		if (!$categoryID) return false;
		
		$d = new DAL(true);
		return $d->qryCount("select id from categoryExposure where categoryID = {$this->id} and linkedCategoryID = $categoryID");
	}
	public function toggleExposure($categoryID)
	{
		$d = new DAL(true);
		
		$lCatObj = new customCategory($categoryID);
		if (!$lCatObj->id) throw new Exception("Error, the Custom Category you are trying to link to could not be found.");
		
		$expID = $this->isExposed($categoryID);
		
		if ($expID)
		{
			return $d->qry("delete from categoryExposure where id=$expID");				
		}
		else
		{
			$cv["categoryID"] = $this->id;
			$cv["linkedCategoryID"] = $categoryID;
			
			return $d->qryInsertByArray("categoryExposure", $cv);
		}
	}
}
class categoryFeature extends phpDataSet
{
	public static $types_array = array("Text Field"=>1, "Hyperlink"=>2, "Image"=>3,"Downloadable File(s)"=>4, "WYSIWYG Text"=>5, "Showcase Link"=>6,
									   "Date Field"=>7, "HTML Literal"=>8, "Custom Form Link"=>9, "Blog Topic Link"=>10, "Blog Category Link"=>11);
		
	var $files = array();
	private $sortDirection = "asc";
	private $modifiedKeys = array();
	
	public function __construct($id)	
	{
		parent::__construct("categoryFeatures", $id, $id ? true : false);		
	}
	public function __get($key)
	{
		switch($key)
		{
			case "category":
				return new customCategory($this->categoryID);
				
			case "type":
				return array_search($this->featureType, categoryFeature::$types_array);	
		}	
		
		return parent::__get($key);
	}
	public function insert($data="")
	{
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from categoryFeatures where categoryID={$this->categoryID}");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		$id = parent::insert($data);		
		
		return $id;
	}
	public function delete()
	{
		$d = new DAL(true);
		
		$qry = "delete from categoryFeatureData where categoryFeatureID = {$this->id}";
		$d->qry($qry);	
		
		$catID = $this->categoryID;
		
		parent::delete();
		
		//Realign the positions
		$qry = "select id
				from categoryFeatures
				where categoryID = $catID
				order by position asc";
		$pQry = $d->qry($qry);
		
		$ct=1;
		while($p = $pQry->fetch_assoc())
		{
			$cvList["position"] = $ct++;	
			$d->qryUpdateByArray("categoryFeatures", $cvList, "id={$p["id"]}");
		}
	}
}
class categoryFeatureData extends phpDataset
{
	public $p_files;
	public $p_links;
	
	public function __construct($featureID)
	{
		parent::__construct("categoryFeatureData", $featureID, $featureID ? true : false);		
	}
	public function __get($key)
	{
		switch($key)
		{
			case "feature":
				return new categoryFeature($this->categoryFeatureID);
				
			case "item":
				return new categoryItem($this->categoryItemID);
				
			case "files":
				$feature = $this->feature;
				
				// Return nothing if the type isn't Downloadable File. This is a drawback to using an array for types, as there's no
				// constant to reference. This is a rare thing to have to do with Features, so for the sake of simplicity a static value
				// has been used here (4) which represents Downloadable Files(s) at the current date.
				if (!$feature->featureType != 4 || $feature->featureConfigData != 1) return false;
				
				if (!$this->p_files)
				{
					//split out multiple files
					$files = explode("::", $this->value);
					$captions = explode("::", $this->additionalData);
			
					foreach($files as $key=>$value)
					{
						$this->p_files[] = new categoryFeatureFile($value, $captions[$key], $key);
					}
				}	

				return $this->p_files;
				
			case "links":
				$feature = $this->feature;
				
				if (!$feature->featureType != 10 && $feature->featureConfigData != 11) return false;
				
				if (!$this->p_links)
				{
					// Separate the topics, or categories, by the internal delimeter. Sets the value to an indexed array of the corresponding data.
					$this->p_links = explode("::", $this->value);
				}
				
				return $this->p_links;			
		}
		
		return parent::__get($key);
	}	
}
class categoryItem extends phpDataset
{
	public $p_featureData;
	
	var $subKeys = array();
	
	public function __construct($categoryItemID)
	{
		parent::__construct("categoryItems", $categoryItemID, $categoryItemID ? true : false);				
	}	
	public function __get($key)
	{
		switch($key)
		{
			case "assetFolder":
				return "assets/categories/{$this->categoryID}/{$this->id}";				
						
			case "category":
				return new customCategory($this->categoryID);
				
			case "featureData":
				if (!$this->p_featureData) $this->p_featureData = $this->getFeatureData();
				return $this->p_featureData;
		}
		
		return parent::__get($key);
	}
	public function insert($data="")
	{
		global $config;		
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from categoryItems where categoryID={$this->categoryID}");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		$id = parent::insert($data);		
		
		$loc = $config["baseAppDir"].$this->assetFolder;
		if (!file_exists($loc)) mkdir($loc);
		
		return $id;
	}
	public function delete()
	{
		$d = new DAL(true);
		
		$qry = "delete from categoryFeatureData where categoryItemID = {$this->id}";
		$d->qry($qry);	
		
		$qry = "delete from categoryKeyLookup where categoryItemID = {$this->id}";
		$d->qry($qry);
		
		$qry = "delete from categoryFeatures where categoryID = {$this->categoryID}";
		$d->qry($qry);		
		
		$baseLoc = $config["baseAppDir"].$this->assetFolder;
		
		foreach(glob($baseLoc."/*") as $file)
		{
			@unlink($file);
		}
		
		@rmdir($baseLoc);
		
		// Need to grab this before deleting for below realignment
		$catID = $this->categoryID;		
		
		parent::delete();
		
		//Realign the positions
		$qry = "select id
				from categoryItems
				where categoryID = $catID
				order by position asc";
		$pQry = $d->qry($qry);
		
		$ct=1;
		while($p = $pQry->fetch_assoc())
		{
			$cvList["position"] = $ct++;	
			$d->qryUpdateByArray("categoryItems", $cvList, "id={$p["id"]}");
		}
	}
	public function getFeatureData()
	{
		if (!$this->id) return false;
		
		$qry = "select id, featureKey
					from categoryFeature
					where categoryID = {$this->item[categoryTypeID]}
					order by position asc";
			$fQry = $d->qry($qry);
			
			$ct = 0;
			while($fea = mysqli_fetch_assoc($fQry))
			{
				if (!$fea[featureKey]) $fea[featureKey] = $ct++;
				$this->features[$fea[featureKey]] = new categoryFeature($this->item[categoryTypeID], $categoryItemID, $fea[id]);
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
	public static function getIDByLookupKey($key)
	{
		validation::ensureString($key);
		if (!$key) return false;
		
		$d = new DAL();
		return $d->qryCount("select id from categoryItems where lookupKey = '$key'");	
	}
	public function hasAccess(security $sec, $permission, $throwException = false)
	{
		$access = $sec->hasAccess(permissions::$manage_category_types, $permission > permissions::read ? permissions::write : $permission) || 
			   	  $sec->hasAccess(permissions::$modify_category_type, $permission, $this->categoryID);
		
		if (!$access && $throwException) $sec->security_error_a();
		
		return $access;
	}
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
?>