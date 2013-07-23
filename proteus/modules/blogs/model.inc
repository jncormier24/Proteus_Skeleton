<?php 
class customBlog_core
{
	public $asset_folders = array("blogs");	
	public $tables = array("blogEntry"=>"CREATE TABLE `blogEntry` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `blogID` int(11) NOT NULL DEFAULT '0',
										 `title` varchar(255) NOT NULL DEFAULT '',
										 `author` varchar(255) NOT NULL DEFAULT '',
										 `body` text NOT NULL,
										 `publishDate` date NULL',
										 `expireDate` date NULL',
										 `addedDateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
										 `updatedUserID` int(11) NOT NULL DEFAULT '0',
										 `updatedDateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
										 `inactive` tinyint(4) NOT NULL,
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"blogEntryLink"=>"CREATE TABLE `blogEntryLink` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `blogEntryID` int(11) NOT NULL DEFAULT '0',
											 `typeID` int(11) NOT NULL DEFAULT '0',
											 `dataID` int(11) NOT NULL DEFAULT '0',
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"blogFeeds"=>"	CREATE TABLE `blogFeeds` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `name` varchar(50) NOT NULL,
											 `inactive` tinyint(4) NOT NULL,
											 `position` tinyint(4) NOT NULL,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"blogs"=>"CREATE TABLE `blogs` (
									 `id` int(11) NOT NULL AUTO_INCREMENT,
									 `title` varchar(255) NOT NULL DEFAULT '',
									 `description` text NOT NULL,
									 `image` varchar(255) NOT NULL DEFAULT '',
									 `position` int(11) NOT NULL DEFAULT '0',
									 `addedDateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
									 `inactive` tinyint(4) NOT NULL,
									 PRIMARY KEY (`id`)
									) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"blogTagCloud"=>"CREATE TABLE `blogTagCloud` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `blogEntryID` int(11) NOT NULL,
											 `tag` varchar(25) NOT NULL,
											 `weight` int(11) NOT NULL,
											 `userAdded` tinyint(4) NOT NULL DEFAULT '0',
											 `excluded` tinyint(4) NOT NULL DEFAULT '0',
											 PRIMARY KEY (`id`),
											 KEY `blogEntryID` (`blogEntryID`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"blogTopics"=>"CREATE TABLE `blogTopics` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `title` varchar(255) NOT NULL,
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8");
	
	public static function getConfigObject()
	{
		return new customConfig(proteus_core::$module_blogs["siteType"], 0, array("blogIconDim"=>"250x150", "entryIconDim"=>"250x150"));
	}	
}
class customBlog extends phpDataset
{	
	public $blogEntries = array();
	public $settings;
	
	public function __construct($id, $populateEntries = false, $populateFilter = 0)
	{	
		parent::__construct("blogs", $id, $id ? true : false);		
		
		if ($id && $populateEntries)
		{
			$this->populateEntries($populateFilter);	
		}		
		
		$this->settings = customBlog_core::getConfigObject();
	}
	public function insert($data="")
	{
		global $config;
		
		$id = parent::insert($data);
		
		$loc = $config["baseAppDir"].$this->assetFolder;
		if (!file_exists($loc)) mkdir($loc);
		
		return $id;
	}
	public function delete()
	{
		$d = new DAL(true);
		
		//Cleanup
		$d->qry("delete from blogEntry where blogID={$this->id}");
		$d->qry("delete from blogEntryLink using blogEntryLink, blogEntry where blogEntryLink.blogEntryID=blogEntry.id and blogEntry.blogID={$this->id}");
		$d->qry("delete from comments using comments, blogEntry where blogEntry.id = comments.dataID and comments.typeID = 1 and blogEntry.blogID={$this->id}");
		
		
		parent::delete();	
	}
	public function __get($key)
	{
		switch($key)
		{
			case "assetFolder":
				return "assets/blogs/{$this->id}";
		}
		
		return parent::__get($key);
	}
	public function populateEntries($filter = 0, $blogEntryClass = "customBlogEntry")
	{
		if (!$this->id) return false;
		
		switch($filter)
		{
			case 0:
				//All entries
				$whCl = "1";
				break;
			case 1:
				//Active Only
				$whCl = "inactive <> 1 and (publishDate = 0000-00-00 or publishDate <= now()) and (expireDate = 0000-00-00 or expireDate >= now())";
				break;
			case 2:
				//Pending Only
				$whCl = "inactive <> 1 and publishDate <> 0000-00-00 and publishDate > now()";
				break;
			case 3:
				//Expired Only	
				$whCl = "inactive <> 1 and expireDate <> 0000-00-00 and expireDate < now()";
				break;
			case 4:
				$whCl = "inactive = 1";
				break;
		}
		
		$d = new DAL(true);
		
		$qry = "select id
				from blogEntry
				where blogID = $this->id and $whCl
				order by publishDate desc, addedDateTime desc";
		$eQry = $d->qry($qry);
		
		while($entry = $eQry->fetch_assoc())
		{
			$this->blogEntries[$entry["id"]] = new $blogEntryClass($entry["id"]);
		}
	}
	public function getIconObject(&$fileLocation='', &$dimensions='')
	{
		global $config;
		
		//Shouldn't ever be getting an icon object for a non-existent entity.
		if (!$this->id) return false;
		
		$baseLoc = $this->assetFolder;		
			
		$url = $baseLoc."/icon.png";
		$fileLocation = $config["baseAppDir"].$url;

		$dimensions = explode("x", $this->settings->blogIconDim);
		
		if (file_exists($fileLocation))
		{
			$iconObject = new image("$url?rnd=".rand(0, 100), validation::prepForDisplay($this->title));			
		}
		else
		{
			$iconObject = new phpDIV("<p>No Image Available</p>");
		}
		
		$iconObject->addClass("blogIcon");
		$iconObject->style = "width: {$dimensions[0]}px; height: {$dimensions[1]}px;";
		$iconObject->id = "blogIcon_{$this->id}";
		
		return $iconObject;
	}
}
class customBlogEntry extends phpDataset
{		
	public $p_linkedTopics = array();
	public $p_linkedFeeds = array();	
	
	const commentsNone = 0;
	const commentsProteus = 1;
	const commentsFacebook = 2;
	
	public $fbCommentsWidth = 490;
	
	public function __construct($id)
	{		
		parent::__construct("blogEntry", $id, $id ? true : false);		
	}
	function getParentBlog()
	{
		return new customBlog($this->blogID);
	}
	public function getLinkedTopics()
	{
		if (!count($this->p_linkedTopics))
		{
			$d = new DAL();

			$qry = "select blogTopics.*, blogEntryLink.id 'linkID'
					from blogTopics inner join blogEntryLink on blogEntryLink.dataID = blogTopics.id
					where blogEntryLink.blogEntryID = {$this->id} and typeID=2";
			$tQry = $d->qry($qry);
	
			while($topic = $tQry->fetch_assoc())
			{
				$this->p_linkedTopics[$topic["id"]] = $topic;	
			}
		}
			
		return $this->p_linkedTopics;	
	}
	public function getLinkedFeeds($inactive = 0)
	{
		if (!count($this->p_linkedFeeds))
		{
			$d = new DAL();

			$qry = "select blogFeeds.*, blogEntryLink.id 'linkID'
					from blogFeeds inner join blogEntryLink on blogEntryLink.dataID = blogFeeds.id
					where blogEntryLink.blogEntryID = {$this->id} and typeID=1 and inactive = $inactive";
			$tQry = $d->qry($qry);
	
			while($feed = $tQry->fetch_assoc())
			{
				$this->p_linkedFeeds[$feed["id"]] = $feed;	
			}
		}
			
		return $this->p_linkedFeeds;	
	}	
	public function getTagCloud()
	{
		$d = new DAL(true);
		$pc = new pageContent();
		
		$cap = 150;
		$start = 100;
		
		$qry = "select *
				from blogTagCloud
				where blogEntryID={$this->id} and excluded=0";
		$totalCt = $d->getTotalCount($qry);
				
		$tQry = $d->qry($qry);
		
		while($tag = $tQry->fetch_assoc())
		{
			$fontSize = $tag["weight"]*7;
			if ($fontSize > $cap) $fontSize = $cap;
			
			$fontSize += $start;
			
			$spn = new phpSpan($tag["tag"], "tag");
			$spn->style = "font-size: {$fontSize}%";		
			
			$pc->addControl($spn);
		}
		
		return $pc;
	}
	public function getIconObject(&$fileLocation='', &$dimensions='')
	{
		global $config;
		
		//Shouldn't ever be getting an icon object for a non-existent entity.
		if (!$this->id) return false;
		
		$baseLoc = $this->assetFolder;
			
		$url = $baseLoc."/icon.png";
		$fileLocation = $config["baseAppDir"].$url;

		$sObj = customBlog_core::getConfigObject();
		
		$dimensions = explode("x", $sObj->entryIconDim);
		
		if (file_exists($fileLocation))
		{			
			$iconObject = new image("$url?rnd=".rand(0, 100), validation::prepForDisplay($this->title));
		}
		else
		{
			$iconObject = new phpDIV("<p>No Image Available</p>");
		}
		
		$iconObject->addClass("blogIcon");
		$iconObject->style = "width: {$dimensions[0]}px; height: {$dimensions[1]}px;";
		$iconObject->id = "blogEntryIcon_{$this->id}";

		return $iconObject;
	}
	public function __get($key)
	{		
		switch($key)
		{
			case "assetFolder":
				return "assets/blogs/{$this->blogID}/{$this->id}";
		}
		
		return parent::__get($key);
	}
	public function delete()
	{
		global $config;
		$d = new DAL(true);
		
		$baseLoc = $config["baseAppDir"].$this->assetFolder;
		
		foreach(glob($baseLoc."/.*") as $file)
		{
			unlink($file);
		}
		
		rmdir($baseLoc);
		
		//Cleanup
		$d->qry("delete from blogEntryLink where blogEntryID={$this->id}");
		$d->qry("delete from blogTagCloud where blogEntryID={$this->id}");
		$d->qry("delete from comments where dataID = {$this->id} and comments.typeID = ".customComments::comment_blog_entry);		
		
		parent::delete();	
	}
	public function insert($data="")
	{
		global $config;
		
		$id = parent::insert($data);
		
		$loc = $config["baseAppDir"].$this->assetFolder;
		if (!file_exists($loc)) mkdir($loc);
		
		return $id;
	}
}
class customBlogFeed extends phpDataset
{		
	private $p_blogEntries = array();
	public $feedLimit;
	
	public function __construct($feedID, $feedLimit=100)
	{
		parent::__construct("blogFeeds", $feedID, $feedID ? true : false);		

		$this->blogEntries = array();
		$this->feedLimit = validation::ensureInt($feedLimit);
	}
	public function __get($key)
	{
		switch($key)
		{
			case "blogEntries":
				if (!$this->p_blogEntries) $this->p_blogEntries = $this->getBlogEntries();
				return $this->p_blogEntries;
		}
		
		return parent::__get($key);
	}
	public function getBlogEntries()
	{
		// Any parameters to this function need to be optional (see above __get switch statement)
		// Could set this function to private, but leaving as public lets us add more optional parameters down the line to get a different
		// list than what's available in the standard "blogEntries" get above
		$d = new DAL(true);	
		
		if ($this->feedLimit) 
		{			
			$lmCl = "limit ".$this->feedLimit;
		}
		
		$entries = array();
		
		$qry = "select blogEntryLink.*
				from blogEntryLink inner join blogFeeds on blogFeeds.id = blogEntryLink.dataID
								   inner join blogEntry on blogEntry.id = blogEntryLink.blogEntryID
				where blogEntryLink.typeID = 1 and blogEntryLink.dataID = {$this->id} 
					  and (publishDate is null or publishDate <= now()) and (expireDate is null or expireDate >= now())
				order by if(publishDate IS NULL, updatedDateTime, publishDate) desc
				$lmCl";
		$eQry = $d->qry($qry);

		while($entry = $eQry->fetch_assoc())
		{
			$eObj = new customBlogEntry($entry["blogEntryID"]);
			$entries[] = $eObj;	
		}		
		
		return $entries;
	}	
	public function insert($data="")
	{
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from blogFeeds");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		return parent::insert($data);
	}
}
class customBlogTopic extends phpDataset  
{	
	public $blogEntries = array();
	
	public function __construct($topicID, $populate = true)
	{
	
		if (!$topicID) throw new Exception("Error, no topic specified!");

		parent::__construct("blogTopics", $topicID, true );
	
		if ($populate)
		{
			$this->populateBlogEntries();
		}
	}
	public function populateBlogEntries($forceRefresh = false, $filter = '', $limit = '', $blogEntryClass = "customBlogEntry")
	{
		$d = new DAL(true);
		
		//Return the existing populated array (if any) as long as force refresh isn't specified
		if (!$forceRefresh && $this->blogEntries) return $this->blogEntries;
		
		$limitClause = '';
		validation::ensureInt($limit, 0);
		if ( $limit ) $limitClause = "LIMIT $limit";
		$this->blogEntries = array();
		
		$whClause = "(publishDate = '0000-00-00' or publishDate <= now()) and (expireDate = '0000-00-00' or expireDate >= now())";
		if($filter) $whClause = $filter;
		
		$qry = "select blogEntryLink.*
				from blogEntryLink inner join blogTopics on blogTopics.id = blogEntryLink.dataID
								   inner join blogEntry on blogEntry.id = blogEntryLink.blogEntryID
				where blogEntryLink.typeID = 2 and blogEntryLink.dataID = {$this->id} 
					  and $whClause
				order by case publishDate when '0000-00-00' then blogEntry.addedDateTime else publishDate end desc
				$limitClause";
				
		$eQry = $d->qry($qry);

		while($entry = $eQry->fetch_assoc())
		{
			$eObj = new $blogEntryClass($entry[blogEntryID]);
			$this->blogEntries[] = $eObj;	
		}		
		
		return $this->blogEntries;
	}
}
//this class defines default render() functions and RSS
class customBlogWebControl 
{
	//Simple control to wrap objects
	private $controls = array();
	
	//array of individual entries
	public $blogEntries = array();
	
	protected $blurbOnly;
	
	public $commentsType; //should we add comments to render(), if so what type fb or proteus
	
	//options
	
	public $blogAddress;//blog.php or something like blog or gamingArticles etc. if want to use file folder format
	public $blogAuthor;
	public $blogTitle;
	public $blogAuthorEmail;
	public $blogDescription;
	
	public function __construct(array $entries, $blurbOnly = false, $commentsType = 1, $blogAddress = "blog.php" )
	{
		
		$this->blogEntries = $entries;
		$this->blogAddress = $blogAddress;
		$this->commentsType = $commentsType;
		
		$this->setHtmlObject($blurbOnly); //default init
	}
	public function addControl($control)
	{
		array_push($this->controls, $control);	
	}
	function render()
	{		
		$content = "";
		foreach($this->controls as $control)
		{
			$content .= $control->render();	
		}
		return $content;
	}
	public function setHtmlObject($blurbOnly = false )
	{
		$this->blurbOnly = $blurbOnly;
		$this->controls = array(); //reset data
		if ( sizeof($this->blogEntries) )
		{
			foreach ($this->blogEntries as $i )
			{
				$i->blogAddress = $this->blogAddress;
				$i->setHtmlObject($this->blurbOnly, $this->commentsType);
				$this->addControl( $i );

			}
		}
		else 
		{
			$this->addControl(new phpParagraph("No entries to display!"));
		}
	}
	function getRSSObject($blogTitle, $blogDescription, $blogAuthor = '', $authorEmail = '')
	{
		global $config;
		$this->blogAuthor = $blogAuthor ? $blogAuthor : $config[businessName];
		$this->blogAuthorEmail = $authorEmail ? $authorEmail : $config[sitemail];
		$this->blogTitle = $blogTitle;
		$this->blogDescription = $blogDescription;
	
		$items = '';
		$first = NULL; 
		$flag = false;
		foreach ($this->blogEntries as $i) 
		{  		
			if(!$flag)
			{
				$first = $i;
				$flag = true;
			}
			$shortDesc = strip_tags($i->body, ''); 
			validation::makeURLFriendlyString($shortDesc);
			$shortDesc = str_replace("&amp;",'and' , $shortDesc );
			$shortDesc = str_replace("&nbsp;",' ', $shortDesc );
			$shortDesc = substr($shortDesc, 0, 220); $shortDesc.=" ...";
			$dt = $i->publishDate ? $i->publishDate : $i->addedDateTime;
			$xmlitem = "<item>
       					<title>". htmlspecialchars_decode($i->title)  ." </title>
       					<link>".$i->getEntryUrl(false)."</link>
       					<description>".$shortDesc."</description>
       					<pubDate>".validation::parseMySqlDate( $dt,"D, d M Y H:i:s T") ."</pubDate>
       					<guid>".$i->getEntryUrl(false)."</guid>
    					</item>	";
			
				$items .= $xmlitem;
		}
		$dt = $first->publishDate ? $first->publishDate : $first->addedDateTime;
		$xml = '<?xml version="1.0" encoding="windows-1252"?> 
		<?xml-stylesheet type="text/xsl" media="screen" href="/~d/styles/rss2full.xsl"?><?xml-stylesheet type="text/css" media="screen" href="http://feeds.gawker.com/~d/styles/itemcontent.css"?>
		<rss xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:feedburner="http://rssnamespace.org/feedburner/ext/1.0" xmlns:atom="http://www.w3.org/2005/Atom" xmlns:media="http://search.yahoo.com/mrss/" version="2.0">
   		<channel>
    	  <title>'.$this->blogTitle.'</title>
    	  <link>'.$config[scripturl].$this->blogAddress.'</link>
    	  <atom:link href="'.$config[scripturl].str_replace("/", '',$_SERVER['PHP_SELF'] ) .'" rel="self" type="application/rss+xml" />
    	  <description>'.strip_tags( $this->blogDescription ).'</description>
      	  <language>en-us</language>
      	  <pubDate>'.date("D, d M Y G:i:s T").'</pubDate>
      	  <lastBuildDate>'.validation::parseMySqlDate( $dt,"D, d M Y H:i:s T").'</lastBuildDate>
      	  <docs>http://blogs.law.harvard.edu/tech/rss</docs>
      	  <generator>Proteus CMS 2.0</generator>
      	  <managingEditor>'.$this->blogAuthorEmail." ($this->blogAuthor)".'</managingEditor>
          <webMaster>admin@mstech.com (Mainstay Technologies)</webMaster>
    	  '.$items.'
  		</channel>
		</rss>';
	
		return new phpLiteral($xml);
	}
	//-----------------------------------------------------------------------------//
	//strict helper functions for different loading operations in prep for display
	//----------------------------------------------------------------------------//
	public static function getBlogEntriesArray($limit = 5, $entryClass = "customBlogEntry", $blogID = '', $allowExpired = false )
	{
	
		$d = new DAL();
		$blogWh = '';
		if($blogID)
		{
			validation::ensureInt($blogID);
			$blogWh = "blogID = $blogID AND";
		}
		$whExpired = "and (expireDate = 0000-00-00 or expireDate >= now())";
		if($allowExpired) $whExpired = "";
		$qry = "select id
				from blogEntry
				where $blogWh inactive <> 1 and (publishDate = 0000-00-00 or publishDate <= now()) $whExpired 
				order by publishDate desc, addedDateTime desc limit $limit";
		$eQry = $d->qry($qry);
		$entries = array();
		while($entry = $eQry->fetch_assoc())
		{
			$entries[] = new $entryClass($entry[id]);
		}
		return $entries;
	}
	public static function getRecentPosts($limit = 5, $commentsType = 2, $addr = "blog.php", $entryClass = "customBlogEntry", $blogID = '', $allowExpired = false )
	{
		$blogRContainer = new phpDiv("","recentPostsContainer");	
		$entries = self::getBlogEntriesArray($limit, $entryClass, $blogID, $allowExpired);
		$dv = new customBlogWebControl($entries, true, $commentsType, $addr);
		$blogRContainer->addControl($dv);
	
		return $blogRContainer;
	}
	public static function getRecentFeedsPostsFromEntry(&$blogEntry, $commentsType = 2, $addr = "blog.php", $limit = 8)
	{
		$blogRContainer = new phpDiv("","recentPostsContainer");	
		$feeds = $blogEntry->getLinkedFeeds();
		$blogEntryClass = get_class($blogEntry);  //see if we are using a child class, and set array to this class for proper rendering
		foreach ( $feeds as $i )
		{
			$fd = new customBlogFeed( $i[id] , false);
			$fd->populateBlogEntries(true, $limit, $blogEntryClass );
			$dv = new customBlogWebControl($fd->blogEntries, true, $commentsType, $addr);
			$blogRHeader = new phpHeader(2,"Recent $fd->name Posts","page_sub_header");
			$blogRContainer->addControl($blogRHeader);
			$blogRContainer->addControl($dv);
		}
	
		return $blogRContainer;
	}
	public static function getRecentFeedPosts($feedID, $commentsType = 2, $addr = "blog.php", $limit = 6, $blogEntryClass = "customBlogEntry")
	{
		$blogRContainer = new phpDiv("","recentPostsContainer");	
		$fd = new customBlogFeed($feedID, false);
		$fd->populateBlogEntries(true, $limit, $blogEntryClass );
		$dv = new customBlogWebControl($fd->blogEntries, true, $commentsType, $addr);
		$blogRHeader = new phpHeader(2,"Recent $fd->name Posts","page_sub_header");
		$blogRContainer->addControl($blogRHeader);
		$blogRContainer->addControl($dv);
	
		return $blogRContainer;
	}
	public static function getAllFeedLinks($addr = "blog.php")
	{
		$d = new DAL();

		$qry = "select *
				from blogFeeds
				where inactive <> 1 
				order by position";
		$eQry = $d->qry($qry);
		$dvCon = new phpDIV('', "blogTopicsContainer");
		$dvCon->addControl(new phpHeader(2, "Blog Topics", "page_sub_header"));
		while($feed = $eQry->fetch_assoc())
		{
			$dv = new phpDIV('', 'blog_topic');
			$hl = new phpHyperlink($addr."?m=".validation::makeURLFriendlyString($feed[name]) . "&feed=".$feed[id], $feed[name], 'topic_link');
			$dv->addControl($hl);
			$dvCon->addControl($dv);
		}
		return $dvCon;
	}		
}
?>