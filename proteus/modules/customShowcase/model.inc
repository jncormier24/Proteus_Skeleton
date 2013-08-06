<?php
class customShowcase_core
{
	public $asset_folders = array("showcases");	
	public $tables = array("showcase"=>"CREATE TABLE `showcase` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `name` varchar(50) NOT NULL DEFAULT '',
										 `description` text NOT NULL,
										 `lookupKey` varchar(20) NOT NULL DEFAULT '',										 
										 `inactive` int(11) NOT NULL DEFAULT '0',
										 `position` int(11) NOT NULL DEFAULT '0',										 
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"showcaseItems"=>"CREATE TABLE `showcaseItems` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `showcaseID` int(11) NOT NULL DEFAULT '0',
											 `description` text NOT NULL,
											 `url` varchar(255) NOT NULL DEFAULT '',
											 `title` varchar(75) NOT NULL DEFAULT '',
											 `image` varchar(255) NOT NULL DEFAULT '',
											 `position` int(11) NOT NULL DEFAULT '0',
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8");
}
class customShowcase extends phpDataset
{
	public $p_settings;	
	public $p_showcaseItems = array();
	
	private $defaults = array("itemThumbDim"=>"150x150", "itemThumbAspect"=>0, "itemDim"=>"800x600", "itemAspect"=>0, 
							  "thumbDim"=>"220x130", "thumbAspect"=>0);
		
	public function __construct($showcaseID=0)
	{	
		parent::__construct("showcase", $showcaseID, $showcaseID ? true : false);		
	}	
	public function __get($key)
	{		
		global $config;
		
		if (!parent::__get("id")) return false;
		
		switch ($key)
		{
			case "assetFolder":
				return "assets/showcases/{$this->id}";
				
			case "settings":
				if (!$this->p_settings) $this->p_settings = new customConfig(proteus_core::$module_showcases["siteType"], $this->id, $this->defaults);
				return $this->p_settings;
				
			case "showcaseItems":
				if (!$this->p_showcaseItems) $this->p_showcaseItems = $this->getShowcaseItems();
				return $this->p_showcaseItems;
				
		}
		return parent::__get($key);	
	}
	public function getShowcaseItems()
	{
		if (!$this->id) return false;
		
		$d = new DAL(true);
		$items = array();
		
		$qry = "select id
				from showcaseItems
				where showcaseID={$this->id}
				order by position asc";
		$iQry = $d->qry($qry);			
			
		while($itm = $iQry->fetch_assoc())
		{
			$items[] = new customShowcaseItem($itm[id]);	
		}
		
		return $items;
	}	
	public function getIconObject(&$fileLocation='', &$dimensions='')
	{
		global $config;
		
		//Shouldn't ever be getting an icon object for a non-existent showcase.
		if (!$this->id) return false;
		
		$baseLoc = $this->assetFolder;
			
		$url = $baseLoc."/icon.png";
		$fileLocation = $config["baseAppDir"].$url;
		
		$dimensions = explode("x", $this->settings->thumbDim);
		
		if (file_exists($fileLocation))
		{			
			$iconObject = new image("$url?rnd=".rand(0, 100), validation::prepForDisplay($this->name));
		}
		else
		{
			$iconObject = new phpDIV("<p>No Image Available</p>");
		}
		
		$iconObject->addClass("showcaseIcon");
		$iconObject->style = "width: {$dimensions[0]}px; height: {$dimensions[1]}px;";

		return $iconObject;
	}
	public function insert($data="")
	{
		global $config;
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from showcase");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		$id = parent::insert($data);
		
		$loc = $config["baseAppDir"].$this->assetFolder;
		if (!file_exists($loc)) mkdir($loc);
		
		$loc = $config["baseAppDir"].$this->assetFolder."/thumbs";
		if (!file_exists($loc)) mkdir($loc);
		
		return $id;
	}
	public function delete()
	{
		global $config;
		
		$baseLoc = $config["baseAppDir"].$this->assetFolder."/thumbs";
		
		foreach(glob($baseLoc."/*") as $file)
		{
			@unlink($file);
		}
		
		@rmdir($baseLoc);
		
		$baseLoc = $config["baseAppDir"].$this->assetFolder;
		
		foreach(glob($baseLoc."/*") as $file)
		{
			@unlink($file);
		}
		
		@rmdir($baseLoc);
		
		return parent::delete();
	}
	public function hasAccess(security $sec, $permission, $throwException = false)
	{
		$access = $sec->hasAccess(permissions::$manage_showcases, $permission) || 
			   	  $sec->hasAccess(permissions::$modify_showcase, $permission, $this->id);
		
		if (!$access && $throwException) $sec->security_error_a();
		
		return $access;
	}
}
class customShowcaseItem extends phpDataset
{		
	public function __construct($showcaseItemID)
	{
		parent::__construct("showcaseItems", $showcaseItemID, $showcaseItemID ? true : false);
	}	
	public function __get($key)
	{		
		global $config;
		
		switch($key)
		{
			case "assetFolder":
				return "assets/showcases/{$this->showcaseID}";
				
			case "showcase":
				return new customShowcase($this->showcaseID);
				
			case "imageThumbLocation":
				if (!$this->id) return false;		
				return $this->assetFolder."/thumbs/{$this->image}";
				
			case "imageLocation":
				if (!$this->id) return false;		
				return $this->assetFolder."/{$this->image}";
								
		}
		
		return parent::__get($key);
	}
	public function insert($data="")
	{
		global $config;
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from showcaseItems");
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
		global $config;
		
		$baseLoc = $config["baseAppDir"].$this->assetFolder;
		
		@unlink($baseLoc."/thumbs/".$this->image);
		@unlink($baseLoc."/".$this->image);		
			
		return parent::delete();
	}
	public function scale()
	{
		$showcase = $this->showcase;
		$dim = explode("x", $showcase->settings->itemDim);
		
		imageHelper::scaleImage($this->imageLocation, $dim[0], $dim[1], $showcase->settings->itemAspect);
	}
	public function scaleThumb()
	{
		$showcase = $this->showcase;
		$dim = explode("x", $showcase->settings->itemThumbDim);
		
		imageHelper::scaleImage($this->imageLocation, $dim[0], $dim[1], $showcase->settings->itemThumbAspect, $this->imageThumbLocation);
	}	
}
?>