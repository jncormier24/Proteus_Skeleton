<?php 
class proteus_core
{
	const module_inventory = 1;
	const module_categoryItems = 2;
	const module_customContent = 3;
	const module_showcase = 4;
	const module_users = 5;
	const module_customForms = 6;
	const module_calendars = 7;
	const module_blogs = 8;
	const module_twitter = 9;
	const module_metaData = 10;
	const module_municipal = 11;
	const module_subscribers = 12;	
		
	public $files = array("sitemap.ajax");	
	public $tables = array("config"=>"CREATE TABLE `config` (
									 `id` int(11) NOT NULL AUTO_INCREMENT,
									 `keyName` varchar(255) NOT NULL DEFAULT '',
									 `keyValue` text NOT NULL,
									 `typeID` int(11) NOT NULL DEFAULT '0',
									 `dataID` int(11) NOT NULL DEFAULT '0',
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
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"sessions"=>"CREATE TABLE `sessions` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `ip` varchar(15) NOT NULL DEFAULT '',
										 `ses_id` varchar(32) NOT NULL DEFAULT '',
										 `ses_time` int(11) NOT NULL DEFAULT '0',
										 `ses_start` int(11) NOT NULL DEFAULT '0',
										 `ses_value` text NOT NULL,
										 PRIMARY KEY (`id`),
										 KEY `ses_id` (`ses_id`),
										 KEY `ses_time` (`ses_time`),
										 KEY `ses_start` (`ses_start`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8");
	
	public $queries = array("config"=>array("insert into config (keyName, keyValue) values('hitCount',0)",
											"insert into config (keyName, keyValue) values('version','4.0')",
											"insert into config (keyName, keyValue) values('siteType','2::4::5::6')"));
	
	public static function processURI($shiftCount = 1)
	{
		// processURI():
		// Takes the query string and extracts the vars by splitting on the '/'
		// Returns an array $url_array containing keys argN for each variable.
		 
		$var_array = explode("/",$_SERVER['SCRIPT_URL']);
		 
		for($i = 0; $i < $shiftCount; $i++)
		{
		array_shift($var_array);
		}
		 
		return $var_array;
	}
}
class proteus_user_core
{
	public $files = array("users.php", "users.ajax");	
	public $tables = array("users"=>"CREATE TABLE `users` (
									 `id` int(11) NOT NULL AUTO_INCREMENT,
									 `login` varchar(50) NOT NULL,
									 `password` varchar(50) NOT NULL,
									 `firstName` varchar(50) NOT NULL,
									 `lastName` varchar(50) NOT NULL,
									 `email` varchar(255) NOT NULL,
									 `lastLogin` datetime NOT NULL,
									 `middleInitial` varchar(5) NOT NULL,
									 `title` varchar(255) NOT NULL,
									 `createdDateTime` datetime NOT NULL,
									 PRIMARY KEY (`id`)
									) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"rightsMatrix"=>"CREATE TABLE `rightsMatrix` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `name` varchar(25) NOT NULL DEFAULT '',
											 `description` text NOT NULL,
											 `displayName` varchar(255) NOT NULL DEFAULT '',
											 `allowManual` smallint(1) NOT NULL DEFAULT '0',
											 `linkedDataString` varchar(255) NOT NULL,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"rightsAssignment"=>"CREATE TABLE `rightsAssignment` (
												 `id` int(11) NOT NULL AUTO_INCREMENT,
												 `userID` int(11) NOT NULL DEFAULT '0',
												 `matrixID` int(11) NOT NULL DEFAULT '0',
												 `matrixValue` varchar(255) NOT NULL DEFAULT '',
												 PRIMARY KEY (`id`)
												) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"permissionsAssignment"=>"CREATE TABLE `permissionsAssignment` (
													 `id` int(11) NOT NULL AUTO_INCREMENT,
													 `userID` int(11) NOT NULL,
													 `permissionKey` varchar(50) NOT NULL,
													 `typeID` tinyint(4) NOT NULL,
													 `dataID` int(11) NOT NULL,
													 `bitValue` tinyint(4) NOT NULL COMMENT '1,3,7,15 - bitwise',
													 `addedDateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
													 PRIMARY KEY (`id`),
													 KEY `permissionKey` (`permissionKey`),
													 KEY `typeID` (`typeID`),
													 KEY `dataID` (`dataID`)
													) ENGINE=MyISAM DEFAULT CHARSET=utf8");
	
	public $queries = array("rightsMatrix"=>array("INSERT INTO `rightsMatrix` (`id`, `name`, `description`, `displayName`, `allowManual`, `linkedDataString`) VALUES
												  (1, 'global_admin', 'Access to all site functionality and administration.', 'Site Admin', 1, ''),
												  (2, 'menu_access', 'Access to individual menu items and functionality', 'Menu Access', 0, '');"));
}
class customComments_core
{
	public $files = array("comments.ajax");	
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
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8");
}
class customComments
{
	public $dataID;
	public $typeID;
	private $p_baseQry;
	
	/*
	* Types:
	* 	1: Blog Entry
	* 	2: Category Item
	* 	3: Showcase
	*/
	
	public function __construct($dataID, $typeID, $statusID = 1)
	{
		validation::ensureInt($dataID);
		validation::ensureInt($typeID);
		validation::ensureInt($statusID);
		
		if (!$statusID) $statusID = 0;
		
		if (!$dataID || !$typeID) throw new Exception("Error, missing comment data!");
		
		$this->dataID = $dataID;
		$this->typeID = $typeID;
		
		$this->p_baseQry = "select *
							from comments
							where typeID=$typeID and dataID=$dataID and approved = $statusID";
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
		$d = new DAL(true);
		
		//Get comment Query
		$cQry = $this->getCommentQuery();
		
		while($entry = $cQry->fetch_assoc())
		{
			//Loop through and clear all comments for this DataID/TypeID
			$totalAff += $this->deleteComment($entry[id]);
		}
		
		return $totalAff;
	}
	public function addComment($comment, $email, $name, $socialData, $rating=0, $approveOverride = false)
	{
		$d = new DAL(true);
		
		if (!strlen($email) || !validation::isValidEmail($email)) throw new Exception("Error, invalid email supplied!");
		if (strlen($name) < 5) throw new Exception("Error, full name must be supplied!");
		
		if (!strlen($comment))
		{
			throw new Exception("Error, no comment body submitted. Cannot add a blank comment.");
		}	
		
		$email = strtolower($email);
		
		$ip = ip2long($_SERVER['REMOTE_ADDR']);
		
		$qry = "select count(*)
				from comments
				where addedIPAddress = $ip and date_add(now(), INTERVAL -5 MINUTE) > addedDateTime";
		
		if ($d->qryCount($qry) > 4) throw new Exception("We're sorry, but you have posted too many comments in a short period of time. Please wait a few minutes and try again.");
		
		if (!validation::isValidEmail($email)) throw new Exception("Sorry, but you've entered an invalid email address. Please try again.");

		$cv[comment] = "'".validation::prepMultiLineForStorage($comment)."'";
		$cv[addedEmailAddress] = "'$email'";
		$cv[fullName] = "'$name'";
		$cv[socialNetworkData] = "'$socialData'";
		$cv[addedIPAddress] = $ip;
		$cv[rating] = $rating;
		$cv[dataID] = $this->dataID;
		$cv[typeID] = $this->typeID;
		$cv[approved] = ($approveOverride ? 1 : 0);
		$cv[addedDateTime] = "now()";				
		
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
	public function getCommentControl($formID, $rows=4, $cols=40)
	{
		global $config;
		
		$frm = new phpForm('post',$formID);
		$frm->class = 'commentForm';
		
		if ($config[facebook_id])
		{
			$fbDiv = new phpDIV("Login with Facebook");
			$fbDiv->class = 'fb-login-button';
			$fbDiv->scope = "email";
			
			$caption = "<span>Enter information below, or </span>".$fbDiv->render();
		}
		else
		{
			$caption = "Please enter information below to post:";	
		}
		
		$fs = new phpFormFieldset($caption);

		$txtComment = new phpTextArea("{$formID}_commentData", '', $rows, $cols);
		$txtComment->addClass('commentData');

		$txtEmail = new customText("{$formID}_commentEmail", '', 25, 75, 'commentEmail');
		$txtEmail->setLabel("Email Address:", "commentEmailLabel");
		
		$txtName = new customText("{$formID}_commentName", '', 25, 75, 'commentName');
		$txtName->setLabel("Your Name:", "commentNameLabel");
		
		$btnPost = new jqUI_button("{$formID}_btnPost", "Post Comment", "", "ui-icon-comment", true);
		$btnPost->addClass('btnPostComment');		
		
		$hDataID = new hiddenInput("{$formID}_dataID", $this->dataID);
		$hDataID->addClass('dataID');		
		
		$fs->addControl($txtName);
		$fs->addControl($txtEmail);
		$fs->addControl($btnPost);
		$fs->addControl($hDataID);
		
		$frm->addControl($txtComment);
		$frm->addControl($fs);
		
		return $frm;
	}
	public static function getCommentByID($commentID)
	{
		$d = new DAL(true);
		
		validation::ensureInt($commentID);
		if (!$commentID) throw new Exception("Error, no comment ID provided.");

		$detail = $d->qryArray("select * from comments where id=$commentID", false);
		if (!$detail[id]) throw new Exception("Error, that comment could not be found!");
		
		return $detail;
	}
}
class customContent_core
{
	const content_generic = 0;
	const content_municipal_entity = 1;
	
	public $files = array("customContent.php", "customContent.ajax");	
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
class metaData_core
{
	public $files = array("metaData.php", "metaData.ajax");	
	public $tables = array("metaData"=>"CREATE TABLE `metaData` (
										 `id` int(11) NOT NULL auto_increment,
										 `baseUrl` varchar(255) NOT NULL,
										 `contentKey` varchar(50) NOT NULL,
										 `contentData` text NOT NULL,
										 `extraData` text NOT NULL,
										 PRIMARY KEY  (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8");
}
class inlineContent extends phpDataSet
{
	const contentType_singleLine = 1;
	const contentType_multiLine = 2;
	const contentType_wysiwyg = 3;	
	
	public function __construct($contentKey='', $id='')
	{		
		validation::ensureInt($id, 0);		
		if ($contentKey) $id = inlineContent::getContentIDByKey($contentKey);
		
		parent::__construct("inlineContent", $id, $id ? true : false);
		
		if ($this->typeID < 3) 
		{
			// Format the content for single-line text controls
			$this->formatted_content = validation::prepForDisplay($this->content);
		}		
		else
		{
			$this->formatted_content = $this->content;	
		}
	}
	public static function getContentIDByKey($contentKey)
	{
		$d = new DAL(true);
		return $d->qryArray("select id from inlineContent where contentKey='$contentKey'");
	}
}
class customConfig
{
	private $p_typeID = 0;
	private $p_dataID = 0;
	
	private $p_data = array();
	protected $p_defaults = array();
	
	const config_blogs = 8;
	const config_calendar = 7;
	const config_municipal = 11;
	
	public function __construct($configType=0, $dataID=0, $defaults = array())
	{		
		validation::ensureInt($configType);
		validation::ensureInt($dataID);
		
		$this->p_dataID = $dataID;
		$this->p_typeID = $configType;
		
		if (is_array($defaults)) $this->p_defaults = $defaults;
		
		$this->loadKeys();
	}
	public function loadKeys(&$configArray='')
	{
		$d = new DAL(true);		
		
		$qry = "select keyName, keyValue
				from config
				where typeID=$this->p_typeID and dataID=$this->p_dataID";
		$cQry = $d->qry($qry);
		
		while($cfg = $cQry->fetch_assoc())
		{			
			$this->p_data[$cfg[keyName]] = $cfg[keyValue];	
			$configArray[$cfg[keyName]] = $cfg[keyValue];
		}		
	}
	public function __get($key)
	{		
		//Key not found in config database - load the default (if any) and add the key to the DB
		if (!isset($this->p_data[$key])) $this->loadDefault($key);
		
		$data = $this->p_data[$key];
		
		return $data;	
	}
	private function loadDefault($key)
	{
		//Just in case... shouldn't ever happen.
		if (isset($this->p_data[$key])) return;
		
		if (!isset($this->p_defaults[$key]))
		{
			//Empty string default
			$this->p_defaults[$key] = "";	
		}
		
		$this->p_data[$key] = $this->p_defaults[$key];
		
		$d = new DAL(true);
		
		//Insert it into the DB
		$cv[keyName] = "'$key'";
		$cv[keyValue] = "'".$this->p_data[$key]."'";
		$cv[typeID] = $this->p_typeID;
		$cv[dataID] = $this->p_dataID;
		
		$cID = $d->qryInsertByArray('config', $cv);
		if (!$cID) throw new Exception("Error adding default config entry - please try again.");
		
		return true;			
	}
	public function __set($key, $value)
	{
		$d = new DAL(true);	
		
		$cv[keyName] = "'$key'";
		$cv[keyValue] = "'$value'";
		$cv[dataID] = $this->p_dataID;
		$cv[typeID] = $this->p_typeID;
		
		if ($this->$key)
		{			
			$d->qryUpdateByArray('config', $cv, "keyName='$key' and dataID=$this->p_dataID and typeID=$this->p_typeID");
		}
		else
		{
			$d->qryInsertByArray("config", $cv);
			$this->loadKeys();	
		}
	}
	public function config($key, $value='')
	{
		if (!$value)
		{
			//Call the internal getter
			return $this->__get($key);
		}
		else
		{
			//Call the internal setter
			$this->__set($key, $value);
		}
	}	
}
class validation
{
	public static function ensureInt(&$inVar, $defaultVal='')
	{
		if (!strlen($inVar))
		{
			$inVar = strlen($defaultVal) ? $defaultVal : 0;
			return;
		}

		if (!is_numeric($inVar))
		{
			if (substr($inVar, 0, 1) == "-")
			{
				$inVar = preg_replace("/[^0-9]/","",substr($inVar, 1)) * -1;
			}
			else
			{
				$inVar = preg_replace("/[^0-9]/","",$inVar);
			}
		}
		else
		{
			$inVar = preg_replace("/\.$/",'', $inVar);
		}

		//MySQL INT Limit
		if ($inVar > 2147483647) $inVar = 2147483647;

		if (strlen($defaultVal) && !$inVar) $inVar = $defaultVal;

		return $inVar;
	}
	public static function ensureDecimal(&$inVar)
	{
		if (!is_numeric($inVar)) $inVar = preg_replace("[^0-9\.]","",$inVar);
		 
		return $inVar;
	}
	public static function ensureFilename(&$filename)
	{
		$tmp = explode(".",$filename);

		//Pop off the last element and lowercase it - this is the extension!
		$ext = strtolower(array_pop($tmp));

		//Put the name back together minus the extension
		$fName = implode(".", $tmp);

		//Turn whitespaces into _ and everything else unacceptable into nothing.
		$fName = preg_replace(array("/\s/","/[^a-zA-Z0-9\.\-\_]/"), array("_",""), $fName);

		//Update the referenced variable with the safe name, plus the extension.
		$filename = $fName.".".$ext;

		//Return the extension (for potential logic use)
		return $ext;
	}
	public static function makeSafe(&$string)
	{
		validation::ensureString($string);
		return $string;
	}
	public static function ensureString(&$inVar)
	{
		global $config;

		if (is_array($inVar))
		{
			foreach($inVar as $key=>$value)
			{
				validation::ensureString($inVar[$key]);
			}
		}
		else
		{
			if (get_magic_quotes_gpc())
			{
				$tmp = stripslashes($inVar);
			}
			else
			{
				$tmp = $inVar;
			}				

			$inVar = mysqli_real_escape_string($config[dbLink], $tmp);
		}
	}
	public static function quotedArray($valueList, $exceptions='')
	{
		global $config;

		if (!$exceptions) $exceptions = array();
		if (!is_array($valueList)) return;

		//Exceptions is an array of keys NOT to encapsulate in quotes - aggregates, public functions, etc
		foreach($valueList as $key=>$value)
		{
			if (!in_array($key, $exceptions))
			{
				if (substr($value,0,1) == "'" && substr($value, strlen($value)-1,1) == "'")
				{
					//If it starts with a quote and ends with a quote - handle it.
					$tmp = substr($value, 1, strlen($value)-2);
				}
				else
				{
					$tmp = $value;
				}

				$tmp = stripslashes($tmp);
				$tmp = mysqli_real_escape_string($config[dbLink], $tmp);
				$tmp = str_replace('$', "&#36;", $tmp);

				//If it's non-numeric, or starts with Zero, encapsulate it in quotes
				if ($tmp[0] == 0 || !is_numeric($tmp)) $tmp = "'".$tmp."'";
				$valueList[$key] = $tmp;
			}
		}
		return $valueList;
	}
	public static function isIPValid($ip)
	{
		return preg_match("/^([0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3})(:?[0-9]{1,5})?$/", $ip);
	}
	public static function prepMultiLineForStorage($strVal, $sanitize=true)
	{
		$strVal = str_replace("\\r\\n","\r\n", $strVal);
		if ($sanitize) validation::sanitizeWYSIWYG($strVal);

		return $strVal;
	}
	public static function prepTextForInput($strVal)
	{
		if (is_array($strVal))
		{
			//Loop through the array and prep
			foreach($strVal as &$val)
			{
				$val = validation::prepTextForInput($val);
			}
		}
		else
		{
			//$strVal = htmlspecialchars($strVal, ENT_NOQUOTES);
			$strVal = stripslashes($strVal);
			$strVal = str_replace("&amp;#36", "&#36", $strVal);
			$strVal = str_replace("'","&#39;", $strVal);
			$strVal = str_replace('"',"&quot;", $strVal);
		}

		return $strVal;
	}
	public static function prepForDisplay($strVal, $bbCodes=true)
	{
		global $gBB;

		$retVal = htmlspecialchars($strVal,ENT_QUOTES);
		$retVal = stripslashes($retVal);

		$retVal = str_replace("&amp;#","&#", $retVal);
		$retVal = str_replace('$', "&#36;", $retVal);
		$retVal = nl2br($retVal);

		if ($bbCodes && $gBB)
		{
			$retVal = $gBB->compile($retVal);
		}
		elseif ($bbCodes && !$gBB)
		{
			//turn links into hyperlinks, and emails into hyperlinks!
			$srch = array("/\b((http(s?):\/\/)|(www\.))([\w\.]+)([A-Za-z\?\#\_\-\%\&\~\/\.\=0-9\;\,]+)/is", "/\b([A-Za-z\_\-\~\.0-9]+)\@([A-Za-z\_\-\~\.0-9]+)\.([a-zA-Z]{3})/is");
			
			$retVal = preg_replace_callback($srch, array('validation','parseCallBack'), $retVal );
		}

		return $retVal;
	}
	public static function parseCallback($matches)
	{
		switch(count($matches))
		{
			case 7:
				return "<a href=\"http$matches[3]://$matches[4]$matches[5]$matches[6]\" class=\"\" target=\"_blank\">$matches[2]$matches[4]$matches[5]$matches[6]</a>";
				break;
			case 4:
				$eml = validation::encodeEmail($matches[1]."@".$matches[2].'.'.$matches[3]);
				return "<a href='mailto:$eml'>$eml</a>";
		}
	}
	public static function parseMySqlDate($mySqlDatetime, $format, $emptyVal="")
	{
		if (!$mySqlDatetime || preg_match("/^(0000\-00\-00)(00:00:00)?/", $mySqlDatetime)) return $emptyVal ? $emptyVal : "";

		$val = explode(" ",$mySqlDatetime);
		$date = explode("-",$val[0]);
	  
		if (!$val[1])
		{
			$time = array(0,0,0);
		}
		else
		{
			$time = explode(":",$val[1]);
		}
	  
		return date($format, mktime($time[0],$time[1],$time[2],$date[1],$date[2],$date[0]));
	}
	public static function convertToMySqlDate($datetime, $separator='-', $emptyVal='0000-00-00 00:00:00')
	{
		if (!$datetime) return "";

		if (preg_match("/^(0000(-|\/|\.)00(-|\/|\.)00)(00\:00\:00)?/", $datetime)) return $emptyVal ? $emptyVal : "";

		//Support passing a Unix timestamp to get same format
		if (is_numeric($datetime))
		{
			//So it includes the hours below (sort of a hack)
			$val[1] = true;
			$ts = $datetime;
		}
		else
		{
			$val = explode(" ",$datetime);
			$date = explode($separator, $val[0]);

			if (!$val[1])
			{
				$time = array(0,0,0);
			}
			else
			{
				$time = explode(":",$val[1]);
			}

			$ts = @mktime($time[0], $time[1], $time[2], $date[0], $date[1], $date[2]);
		}
		//Exclude the time portion if it doesn't exist. MySql will know to use 00:00:00
		return date("Y-m-d".($val[1] ? " H:i:s" : ""), $ts);
	}
	public static function convertToUnixTimestamp($mySqlDatetime, $gmt=false)
	{
		if (preg_match("/^(0000\-00\-00)\s?(00:00:00)?/", $mySqlDatetime)) return "";

		$val = explode(" ",$mySqlDatetime);
		$date = explode("-",$val[0]);
	  
		if (!$val[1])
		{
			$time = array(0,0,0);
		}
		else
		{
			$time = explode(":",$val[1]);
			if (!$time[2]) $time[2] = 0;
		}
	  
		if ($gmt)
		{
			$stamp = @gmmktime($time[0],$time[1],$time[2],$date[1],$date[2],$date[0]);
		}
		else
		{
			$stamp = @mktime($time[0],$time[1],$time[2],$date[1],$date[2],$date[0]);
		}
		//if ($gmt) $stamp -= date("Z");
	  
		return $stamp;
	  
	}
	public static function parseDateDisplay($timestamp, $full=false)
	{
		if (!$timestamp) return false;
	
		$curTime = time();
	
		//$curTime = time();
		$diff = $curTime - $timestamp;
			
		if ($diff < 60)
		{			
			return "A few seconds ago";
		}
		else if ($diff > 60 && $diff < (60*75))
		{			
			$mins = round($diff/60);
				
			return $mins > 1 ? "$mins mins ago" : "About a minute ago";
		}
	
		$yest = strtotime("-1 days", $curTime);
		$yest = mktime(0,0,0,date("m", $yest), date("d", $yest), date("Y", $yest));
			
		$today = mktime(0,0,0,date("m"), date("d"), date("Y"));
			
		if ($timestamp > $yest && $timestamp < $today)
		{
			return "Yesterday at ".date("h:ia", $timestamp);
		}
		if ($diff > (60*60) && $diff < (60*60*24))
		{
			$hours = round($diff/60/60);
			return $hours > 1 ? "About $hours hours ago" : "About an hour ago";
		}
		if ($diff > (60*60*24) && $diff < (60*60*24*7))
		{			
			return date("D \a\\t h:ia", $timestamp);
		}
		if (date("Y", $curTime) != date("Y", $timestamp))
		{
			return date("M d, Y", $timestamp);
		}
		else
		{
			return date("D, M d \a\\t h:ia", $timestamp);
		}
	}
	public static function getValidEmailRegex($isolatedString=true)
	{
		return ($isolatedString ? "^" : "")."([_a-z0-9-]+(\.[_a-z0-9-]+)*@([0-9a-z][0-9a-z-]*[0-9a-z]\.)+[a-z]{2,4})[mtgvuz]?".($isolatedString ? "$" : "");	
	}
	public static function isValidEmail($email)
	{
		//Had to add the front-ticks here since Javascript doesn't know how to handle them. (the above expression is used to validate emails in JS where applicable)
		return preg_match("/".validation::getValidEmailRegex()."/", strtolower(trim($email)));	
	}
	public static function encodeEmail($email, $humanReadable=false)
	{
		if ($humanReadable)
		{
			return preg_replace(array("/\@/i","/\./i"),array(" at ", " dot "), $email);	
		}
		else
		{
			$eArr = preg_split('//', trim($email), -1, PREG_SPLIT_NO_EMPTY);
			
			for($i=0,$ct=count($eArr) ; $i < $ct ; $i++)
			{
				$tmp[$i] = "&#".ord($eArr[$i]).";";			
			}	
	
			return join('',$tmp);
		}
	}	
	public static function truncateText(&$strVal, $maxLength, $postElipse="...")
	{
		if (strlen($strVal) > $maxLength) $strVal = substr($strVal, 0, $maxLength).$postElipse;	
	}
	public static function ensureTelephone(&$phoneStr)
	{
		$phoneStr = preg_replace("/.*?([0-9]{3}).*?([0-9]{3}).*?([0-9]{4})/", "\\1.\\2.\\3", $phoneStr);	
	}
	public static function cleanBBCodes($strVal)
	{
		$strVal = preg_replace("/\[.*?\]/i","",$strVal);

		return $strVal;
	}
	public static function cleanHTML($strVal)
	{
		$strVal = preg_replace("/\<.*?\>/i","",$strVal);	
	}
	public static function makeURLFriendlyString($string)
	{
		$val = preg_replace(array("/[^a-zA-Z0-9\s\-]/", "/\s/"), array("","-"), trim($string));
		$val = preg_replace("/\-{2,}/","-", $val);
		
		validation::truncateText($val, 60, -1);
		
		return $val;	
	}
	public static function sanitizeWYSIWYG(&$content)
	{
		$content = preg_replace("/\<\!--\[if.*?\].*?\<\!\[endif\]--\>/is", "", $content);	
	}
}
?>