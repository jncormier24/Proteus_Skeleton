<?php
class customComments_core
{		
	public $tables = array("comments"=>"CREATE TABLE `comments` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `dataID` int(11) NOT NULL,
										 `typeID` tinyint(4) NOT NULL,
										 `comment` text NOT NULL,
										 `rating` int(11) NOT NULL,
										 `approved` tinyint(4) NOT NULL,
										 `addedIPAddress` int(11) UNSIGNED NOT NULL,
										 `addedEmailAddress` varchar(75) NOT NULL,
										 `fullName` varchar(75) NOT NULL,
										 `socialNetworkData` varchar(50) NOT NULL,
										 `addedDateTime` datetime NOT NULL,
										 `addedUserID` int(11) NOT NULL,
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8");
}
class customComments
{
	public $dataID;
	public $typeID;
	
	private $p_baseQry;
	
	const comment_blog_entry = 1;
	const comment_category_item = 2;
	const comment_showcase = 3;
	const comment_user = 4; 
	
	public function __construct($dataID, $typeID)
	{
		validation::ensureInt($dataID);
		validation::ensureInt($typeID);		
		
		if (!$dataID || !$typeID) throw new Exception("Error, missing comment data!");
		
		$this->dataID = $dataID;
		$this->typeID = $typeID;
		
		$this->p_baseQry = "select *
							from comments
							where typeID=$typeID and dataID=$dataID";
	}
	public function getCommentQuery($orderBy='addedDateTime desc', $limit='')
	{
		$d = new DAL(true);
		
		$orderBy = "order by $orderBy";
		$limit = $limit ? "limit $limit" : "";
		
		return $d->qry($this->p_baseQry." $orderBy $limit", false);
	}
	public function getCommentCount()
	{
		$qryObj = $this->getCommentQuery();
		return $qryObj->num_rows;	
	}
	public function clearComments()
	{		
		//Get comment Query
		$cQry = $this->getCommentQuery();
		
		while($entry = $cQry->fetch_assoc())
		{
			//Loop through and clear all comments for this DataID/TypeID
			$totalAff += $this->deleteComment($entry["id"]);
		}
		
		return $totalAff;
	}
	public function addComment($comment)
	{
		global $g_sec;
		
		$d = new DAL(true);
		
		if (!strlen($comment))
		{
			throw new Exception("Error, no comment body submitted. Cannot add a blank comment.");
		}
		
		$ip = ip2long($_SERVER["REMOTE_ADDR"]);

		$cv["comment"] = "'".validation::prepMultiLineForStorage($comment)."'";				
		$cv["dataID"] = $this->dataID;
		$cv["typeID"] = $this->typeID;
		$cv["addedUserID"] = $g_sec->id;
		$cv["addedDateTime"] = "now()";				
		$cv["addedIPAddress"] = $ip;
		
		$cID = $d->qryInsertByArray("comments", $cv);
		
		if (!$cID) throw new Exception("Error adding comment! Please try again.");
		
		return $cID;
	}
	public static function deleteComment($commentID)
	{
		validation::ensureInt($commentID);
		if (!$commentID) throw new Exception("Error, no comment specified!");
		
		$d = new DAL(true);

		$affRows = $d->qry("delete from comments where id=$commentID");
		return $affRows;
	}	
	public static function getCommentByID($commentID)
	{
		$d = new DAL(true);
		
		validation::ensureInt($commentID);
		if (!$commentID) throw new Exception("Error, no comment ID provided.");

		$detail = $d->qryArray("select * from comments where id=$commentID", false);
		if (!$detail["id"]) throw new Exception("Error, that comment could not be found!");
		
		return $detail;
	}
	public function hasAccess($sec, $accessLevel=1)
	{
		/*
		 * This function should be overridden in a child class if there are custom permissions to check; this allows
		 * for the use of virtually any kind of typeID
		 * 
		 * Default is READ Access only
		*/				
		
		switch($this->typeID)
		{
			case customComments::comment_blog_entry:
				$perm = permissions::$manage_blogs;
				if ($accessLevel == permissions::insert) $accessLevel = permissions::read;
				if ($accessLevel == permissions::delete) $accessLevel = permissions::write;
				
				if ($sec->hasAccess($perm, $accessLevel, 0)) return true;
				
				$iObj = new customBlogEntry($this->dataID);
								
				return $sec->is(permissions::$modify_blog_category, $iObj->blogID); 
				
			case customComments::comment_user:
				$perm = permissions::$manage_users;		
				if ($accessLevel == permissions::insert) $accessLevel = permissions::read;
				if ($accessLevel == permissions::delete) $accessLevel = permissions::write;		
				break;
				
			default:
				throw new Exception("Error, invalid comment type specified.");		
		}
		
		return $sec->hasAccess($perm, $accessLevel, 0);
	}
} 
class commentContainer extends phpDIV
{
	public function __construct($dataID, $typeID, $renderLoader = false, $id = '')
	{
		parent::__construct();
		
		$this->class = "commentContainer";
		$this->addAttribute("data-dataid", $dataID);
		$this->addAttribute("data-typeid", $typeID);
		
		if ($id) $this->id = $id;
		
		if ($renderLoader)
		{
			if (!$this->id) $this->id = "comment_container_{$dataID}_{$typeID}";
			$this->addControl(new jqLoader("getComments($dataID, $typeID, '#$this->id')"));
		}
	}
}
?>