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
	
	private $defaults = array("itemThumbDim"=>"150::150", "itemThumbAspect"=>0, "itemDim"=>"800::600", "itemAspect"=>0, 
							  "thumbDim"=>"150::150", "thumbAspect"=>0);
		
	public function __construct($showcaseID=0)
	{	
		parent::__construct("showcase", $showcaseID, $showcaseID ? true : false);
	}	
	public function __get($key)
	{
		if (!$this->id) return false;
		
		switch ($key)
		{
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
		
		$folder = "assets/showcases/{$this->id}/";		
		$absFolder = $config["baseAppDir"].$folder;		
			
		$file = "icon.png";

		$dimensions = explode("x", $this->settings->iconDim);
		
		if (file_exists($absFolder.$file))
		{			
			$iconObject = new image($folder.$file."?rnd=".rand(0, 100), validation::prepForDisplay($this->name));
		}
		else
		{
			$iconObject = new phpDIV("<p>No Image Available<p>");
		}
		
		$iconObject->addClass("showcaseIcon");
		$iconObject->style = "width: {$dimensions[0]}px; height: {$dimensions[1]}px;";

		return $iconObject;
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
		if (!$this->id) return false;
		
		global $config;
		
		switch($key)
		{
			case "showcase":
				return new customShowcase($this->showcaseID);
				
			case "imageThumbLocation":
				return $config["baseAppDir"]."assets/showcases/{$this->showcaseID}/thumbs/{$this->image}";
				
			case "imageLocation":
				return $config["baseAppDir"]."assets/showcases/{$this->showcaseID}/{$this->image}";
								
		}
		
		return parent::__get($key);
	}
	public function scale()
	{
		$showcase = $this->showcase;
		$dim = $showcase->settings->itemDim;
		
		imageHelper::scaleImage($this->imageLocation, $dim[0], $dim[1], $showcase->settings->itemAspect);
	}
	public function scaleThumb()
	{
		$showcase = $this->showcase;
		$dim = $showcase->settings->itemThumbDim;
		
		imageHelper::scaleImage($this->imageThumbLocation, $dim[0], $dim[1], $showcase->settings->itemThumbAspect);
	}
}
?>