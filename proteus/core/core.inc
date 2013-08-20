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
	
	public static $module_municipal = array("siteType"=>11, "required"=>false, "location"=>"proteus/modules/municipal", "caption"=>"Municipal Framework", "class"=>"pmm_core");
	public static $module_subscribers = array("siteType"=>12, "required"=>false, "location"=>"proteus/modules/subscribers", "caption"=>"Content Subscriptions", "class"=>"subscriber_core");
	public static $module_calendars = array("siteType"=>7, "required"=>false, "location"=>"proteus/modules/calendars", "caption"=>"Calendars", "class"=>"customCalendar_core");
	public static $module_blogs = array("siteType"=>8, "required"=>false, "location"=>"proteus/modules/blogs", "caption"=>"Blogs", "class"=>"customBlog_core");
	public static $module_customCategories = array("siteType"=>2, "required"=>false, "location"=>"proteus/modules/customCategories", "caption"=>"Custom Categories", "class"=>"customCategory_core");
	public static $module_showcases = array("siteType"=>4, "required"=>false, "location"=>"proteus/modules/customShowcase", "caption"=>"Custom Showcases", "class"=>"customShowcase_core");
	public static $module_customForms = array("siteType"=>6, "required"=>false, "location"=>"proteus/modules/customForms", "caption"=>"Custom Forms", "class"=>"customForms_core");
	
	public static $module_comments = array("siteType"=>0, "required"=>true, "location"=>"proteus/modules/comments", "caption"=>"Comments", "class"=>"customComments_core");
	public static $module_customContent = array("siteType"=>3, "required"=>true, "location"=>"proteus/modules/customContent", "caption"=>"Custom Content, Inline Editor", "class"=>"customContent_core");
	public static $module_users = array("siteType"=>0, "required"=>true, "location"=>"", "caption"=>"Users Database", "class"=>"proteus_user_core");
	public static $module_tableSorter = array("siteType"=>0, "required"=>true, "location"=>"proteus/modules/tableSorter", "caption"=>"Sortable Interface", "class"=>"tableSorter_core");
			
	public $tables = array("config"=>"CREATE TABLE `config` (
									 `id` int(11) NOT NULL AUTO_INCREMENT,
									 `keyName` varchar(255) NOT NULL DEFAULT '',
									 `keyValue` text NOT NULL,
									 `typeID` int(11) NOT NULL DEFAULT '0',
									 `dataID` int(11) NOT NULL DEFAULT '0',
									 PRIMARY KEY (`id`)
									) ENGINE=MyISAM DEFAULT CHARSET=utf8",							
							"cache"=>"CREATE TABLE `cache` (
									  `dataID` int(11) NOT NULL,
									  `typeID` tinyint(4) NOT NULL,
									  `contentKey` char(25) NOT NULL,
									  `content` mediumtext NOT NULL,
									  `addedDateTime` datetime NOT NULL,
									  `updateStamp` datetime default NULL,
									  PRIMARY KEY  (`dataID`,`contentKey`),
									  KEY `dataID` (`dataID`),
									  KEY `contentKey` (`contentKey`),
									  KEY `addedDateTime` (`addedDateTime`)
									) ENGINE=InnoDB DEFAULT CHARSET=utf8;",
							"sessions"=>"CREATE TABLE IF NOT EXISTS `sessions` (
									  `session_id` varchar(32) NOT NULL,
									  `userID` INT(11) NOT NULL,
									  `data` text NOT NULL,
									  `lastAccess` datetime NOT NULL,
									  `lastPage` int(11) NOT NULL,
									  `ipAddress` int(11) NOT NULL,
									  `views` int(11) NOT NULL,
									  PRIMARY KEY (`session_id`)
									) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	
	public $queries = array("config"=>array("insert into config (keyName, keyValue) values('hitCount',0)",
											"insert into config (keyName, keyValue) values('defaultEmailTemplate','default.tpl')",
											"insert into config (keyName, keyValue) values('version','4.0')",
											"insert into config (keyName, keyValue) values('siteType','3')"));
	
	public static function getModulesArray()
	{
		$permObj = new ReflectionClass("proteus_core");
		$permArr = $permObj->getStaticProperties();
		
		ksort($permArr);

		return $permArr;
	}
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
	public static function getMicrotime()
	{
		list($usec, $sec) = explode(" ",microtime());
		return ((float)$usec + (float)$sec);
	}
	public static function generateRandomString($length = 5) 
	{
		$chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";	

		$size = strlen($chars);
		
		for($i = 0; $i < $length; $i++) 
		{
			$str .= $chars[rand(0, $size - 1 )];
		}

		return $str;
	}
	public static function includeAllModules()
	{
		// Include all configured module model.inc files
		$conf = new siteConfig();
		$mods = proteus_core::getModulesArray();
		
		foreach($mods as $mod)
		{
			if ($conf->moduleEnabled($mod["siteType"])) proteus_core::includeModule($mod);
		}		
	}
	public static function includeModule(array $module)
	{
		// Include an individual module manually by including the model.inc
		global $config;
		include_once($config["baseAppDir"].$module["location"]."/model.inc");
		
		$strClass = $module["class"];
		
		return $strClass ? new $strClass() : false;
	}
	public static function includeSubPage($control_file)
	{
		// Use this to include a sub-page by using a named control (.inc) file. Subpages will have inherent access to the model.inc
		// file, if any, for the parent page.
		
		global $config;
		include($config["pageDir"]."/".$config["pageIndex"]."/".$control_file);
	}
	public static function syncAdminModules()
	{
		$db = new dbSync();		
		$mods = proteus_core::getModulesArray();
		
		foreach($mods as $mod)
		{
			if ($mod["required"])
			{
				$obj = new $mod["class"];
				$db->syncModule($obj);
			}
		}
	}
}
interface customAdminPage
{	
	// Check Menu Access: This method is requires as it determines the menu availability of the Custom Admin page in the backend navigation.
	// It only accepts a security class because it's going to check for READ access only, then the control file for the custom admin page will 
	// determine any additional security restrictions.	
	public static function checkMenuAccess(security $sec);	
	
	// Since Interfaces cannot declare variables, have to use public getters for admin page options
	public static function getCaption();
	public static function getIconClass();

	// This function returns the key to a menu block within Proteus, or an array containing a new Menu dropdown caption and 
	// subsequent key (for re-use)
	public static function getMenuKey();
}
class customSession
{
	/*
	 * Custom Session Data - this class is used to mimic a simple session handler for custom data. Since the built in session handler
	 * is less than reliable, this creates an easy way to get online users, and store simple data in a fashion untethered from the 
	 * default session mechanism, which is extremely volatile and unpredictable.
	 * 
	 * All data is based on the CURRENT session only (by design), and instantiating the class creates a way to store/read custom data from 
	 * the internal array.
	 * 
	 * Use the static methods in this class to perform anonymous actions on the session table - set last access, etc.
	*/
	private $p_data = array();
	private $p_sessID;
	
	public function __construct()
	{		
		$this->p_sessID = session_id();
	}
	public function __get($key)
	{
		// Late bind the data for performance
		if (!count($this->p_data)) $this->fill();
		
		switch($key)
		{
			 // Used a switch case here in anticipation of adding more features to this
			case "data":				
				return unserialize($this->p_data["data"]);				
		}
		
		return $this->p_data[$key];
	}
	private function fill()
	{
		$d = new DAL(true);
		$this->p_data = $d->qryArray("select * from sessions where session_id='{$this->p_sessID}'");
	}
	public function __set($key, $value)
	{
		$d = new DAL(true);
		
		$this->p_data[$key] = $value;
		
		$cv["data"] = "'".serialize($this->p_data)."'";
		
		$d->qryUpdateByArray("sessions", $cv, array("session_id"=>$this->p_sessID));
	}	
	public static function setLastAccess(security $secObj = null)
	{
		global $config;
		
		$d = new DAL(true);
		$sessID = session_id();		
		
		$cv["lastAccess"] = "now()";
		$cv["lastPage"] = "'{$_SERVER["PHP_SELF"]}'";
		$cv["ipAddress"] = ip2long($_SERVER["REMOTE_ADDR"]);
		$cv["views"] = "views+1";
		
		// Could probably add some paranoid checking in here at the expense of DB overhead; ensure sessionID ip addresses match.
				
		$aff = $d->qryUpdateByArray("sessions", $cv, array("session_id"=>$sessID));
		
		if (!$aff[0])
		{
			$cv["session_id"] = "'{$sessID}'";

			if ($secObj->id) $cv["userID"] = $secObj->id;
			
			try 
			{
				$d->qryInsertByArray("sessions", $cv);
				customSession::clean();
			}
			catch (Exception $ex)
			{
				$cv["error_description"] = "Unable to store custom session information.";
				$cv["error_message"] = $ex->getMessage();
				debug($cv);
			}			
		}	
	}
	public static function clean()
	{
		
	}
}
class customConfig
{
	private $p_typeID = 0;
	private $p_dataID = 0;

	protected $p_data = array();
	protected $p_defaults = array();

	const config_site = 0;
	const config_user = 5;
	const config_blogs = 8;	
	const config_calendar = 7;
	const config_municipal = 11;
	const config_user_notifications = 50;

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
			$this->p_data[$cfg["keyName"]] = $cfg["keyValue"];
			$configArray[$cfg["keyName"]] = $cfg["keyValue"];
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
		$cv["keyName"] = "'$key'";
		$cv["keyValue"] = "'".$this->p_data[$key]."'";
		$cv["typeID"] = $this->p_typeID;
		$cv["dataID"] = $this->p_dataID;

		$cID = $d->qryInsertByArray('config', $cv);
		if (!$cID) throw new Exception("Error adding default config entry - please try again.");

		return true;
	}
	public function __set($key, $value)
	{
		$d = new DAL(true);
		
		$cv["keyName"] = "'$key'";
		$cv["keyValue"] = "'$value'";
		$cv["dataID"] = $this->p_dataID;
		$cv["typeID"] = $this->p_typeID;

		if (isset($this->p_data[$key]))
		{
			$d->qryUpdateByArray('config', $cv, array("keyName"=>$key, "dataID"=>$this->p_dataID, "typeID"=>$this->p_typeID));
		}
		else
		{
			$d->qryInsertByArray("config", $cv);
			$this->loadKeys();
		}
	}
	public function config($key, $value='')
	{
		// DEPRECATED 7/2/13: This function is just an ease-of-use wrapper for get/set used in a lot of Admin modules
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
class siteConfig extends customConfig
{
	public $options = array();
	private $enabled_cache = array();
	
	public function __construct()
	{
		parent::__construct(customConfig::config_site);		
	}	
	public function moduleEnabled($type)
	{			
		if (is_array($type)) $type = $type["siteType"];
		
		// Store / fetch from a local cache. This makes the code easier to understand within loops, without degrading performance
		if ($this->enabled_cache[$type]) return true;
		
		$tmp = explode("::", $this->siteType);
			
		foreach($tmp as $val)
		{
			if ($val == $type) 
			{
				$this->enabled_cache[$type] = true;
				return true;
			}
		}
	
		return false;
	}
}
class proteus_user_core
{		
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
									 `dob` date NULL,
									 `createdDateTime` datetime NOT NULL,
									 `inactive` tinyint(4) NOT NULL,
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
	
	public $queries = array("permissionsAssignment"=>array("INSERT INTO permissionsAssignment (userID, permissionKey, bitValue) VALUES (1, 'site_admin', 1);"),
							"users" => array("insert into users (login, password, createdDateTime) values ('admin','prkxJ0EGtijhE',now())"));
}
class userConfig extends customConfig
{
	public function __construct(security $sec)
	{
		parent::__construct(customConfig::config_user, $sec->id);
	}
}
class notificationsConfig extends customConfig
{
	public function __construct(security $sec)
	{
		parent::__construct(customConfig::config_user_notifications, $sec->id);
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
class validation
{
	public static function ensureInt(&$inVar, $defaultVal='')
	{			
		if (!isset($inVar) || !strlen($inVar))
		{
			if (strlen($defaultVal)) $inVar = $defaultVal;			
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
			$inVar = mysqli_real_escape_string($config["dbLink"], $inVar);
		}
	}
	public static function quotedArray($valueList, $exceptions='')
	{
		global $config;

		if (!$exceptions) $exceptions = array();
		if (!is_array($valueList)) return;

		//Exceptions is an array of keys NOT to encapsulate in quotes - aggregates, public functions, etc
		foreach($valueList as $key=>&$value)
		{
			if (!in_array($key, $exceptions))
			{
				if (!preg_match("/^'.*'$/", $value))					
				{
					$value = validation::ensureString($value);
					$value = str_replace('$', "&#36;", $value);
					
					if (!is_numeric($value)) $value = "'".$value."'";
				}
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
		// SWB (7/13) - this function has been deprecated now that magic quotes is no longer used
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
 	public static function prepForDisplay($strVal, $formatLinks=true)
	{
		// SWB - 6/13: Deprecated use of BBCodes, in favor of strict WYSIWYG implementation
		//global $gBB;

		$retVal = htmlspecialchars($strVal,ENT_QUOTES);
		$retVal = stripslashes($retVal);

		$retVal = str_replace("&amp;#","&#", $retVal);
		$retVal = str_replace('$', "&#36;", $retVal);
		$retVal = nl2br($retVal);

		if ($formatLinks)
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
	public static function truncateText($strVal, $maxLength, $postElipse="...")
	{
		// SWB (7/13): No longer takes first argument as a pointer
		if (strlen($strVal) > $maxLength) $strVal = substr($strVal, 0, $maxLength).$postElipse;

		return $strVal;
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
class json
{
	private $vars = array();

	//Set to 1 to disable the "var="
	public $renderMode = 0;
	public $autoQuote = true;

	public function __construct($renderMode=0)
	{
		$this->renderMode = $renderMode;
	}
	public function clear()
	{
		$this->vars = array();
	}
	public function __set($key, $value)
	{
		$this->addObject($key, $value);
	}
	public function __get($key)
	{
		return $this->vars[$key];
	}
	public function reverse()
	{
		$this->vars = array_reverse($this->vars, true);
	}
	public function getUnformattedJSON()
	{
		return json::getUnformattedJSONFromArray($this->vars);
	}
	public static function getUnformattedJSONFromArray(array $arr)
	{
		//Use this function to get around the ridiculous auto-quote feature of json_encode
		foreach($arr as $key=>$val)
		{
			$content[] = "$key: $val";
		}
		return "{".implode(",", $content)."}";
	}
	public function addObject($key, $value, $stripFormat=true)
	{
		if ($stripFormat)
		{
			$value = str_replace("\n", "", $value);
			$value = str_replace("\r", "", $value);
			$value = str_replace("\t", "", $value);
				
			////// NOTE: This may be needed again, commented due to not being able to use || for inline javascript on JSON buttons (JQuery controls) ///////////
			// $value = str_replace("|", "&#124;", $value);
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//$value = str_replace("'","\\'", $value);
		}

		//$this->vars[$key] = utf8_encode($value);
		$this->vars[$key] = $value;
	}
	public static function renderArray(array $jsonArray)
	{
		foreach($jsonArray as $jsObj)
		{
			$content[] = $jsObj->render();
		}

		return "[".implode(",", (array)$content)."]";
	}
	public function render($evalVar="jsonData")
	{
		$enc = json_encode($this->vars);

		if (!$this->autoQuote) $enc = str_replace('"','', $enc);

		$jTxt = ($this->renderMode ? $enc : "var $evalVar = ".$enc);

		return $jTxt;
	}
}
class json_a extends json
{
	public function __construct($argArray=array())
	{
		parent::__construct($argArray);
		$this->renderMode = 1;
	}
}
class upload
{
	public $p_Pointer;
	public $p_Name;
	public $p_Size;
	public $p_Type;
	public $p_TmpName;
	public $p_image;
	public $allowedMimeTypes = array();

	public $err;
	public $uploadComplete = false;

	public function __construct($file='', $image=true)
	{
		$this->p_Pointer = $file;
		$this->p_Name = preg_replace("/[^a-zA-Z0-9\_\-\.]/", "_", $this->p_Pointer[name]); // $this->p_Pointer[name];
		$this->p_Size = $this->p_Pointer[size];
		$this->p_Type = $this->p_Pointer[type];
		$this->p_TmpName = $this->p_Pointer[tmp_name];
		$this->p_image = $image;
	}
	public function uploadFile($location, $allowedExtensions=array(), $resize=false, $width=640, $height=480, $forceResize=false)
	{		
		global $config;
		
		if (!$this->p_Pointer) throw new Exception("Error, no file specified for upload.");

		if ($this->p_Size > $config["upload_max_size"]) throw new Exception("That file is too large to upload! Max file size is ".($config["upload_max_size"] / 1048576)."Mb");
		
		if ($this->p_image)
		{
			$this->allowedMimeTypes = array("image/jpg", "image/gif", "image/png", "image/jpeg");			
		}

		try
		{
			switch($this->p_Pointer[error])
			{
				case '':
					//No error...
					break;
						
				case '1':
					$error = 'The uploaded file exceeds the upload_max_filesize directive in php.ini';
					break;
						
				case '2':
					$error = 'The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form';
					break;
						
				case '3':
					$error = 'The uploaded file was only partially uploaded';
					break;
						
				case '4':
					$error = 'No file selected for upload!';
					break;

				case '6':
					$error = 'Missing a temporary folder';
					break;
						
				case '7':
					$error = 'Failed to write file to disk';
					break;
						
				case '8':
					$error = 'File upload stopped by extension';
					break;
						
				case '999':
				default:
					$error = 'No error code avaiable';
			}
				
			if ($error) throw new Exception($error);
				
			/*== do extra security check to prevent malicious abuse==*/
			if (!is_uploaded_file($this->p_TmpName))
			{
				throw new Exception("Error uploading $location - (possible malicious file)");
			}
				
			$fExt = $this->getFileExtension($this->p_Name);				
			if (!$fExt) throw new Exception("Sorry, but uploaded files must have a proper extension; i.e., .jpg, .png, gif.");
				
			if (count($allowedExtensions) && !in_array($fExt, $allowedExtensions))
			{
				$extTypes = implode(", ", $allowedExtensions);
				throw new Exception("File provided does not have an allowed extension. ($extTypes)");
			}				
				
			$mime = $this->getMimeType();
							
			if (count($this->allowedMimeTypes) && !in_array($mime, $this->allowedMimeTypes))
			{
				throw new Exception("File provided does not have a correct MIME type for this kind of upload!");
			}

			if ($resize)
			{				
				$iSize = GetImageSize($this->p_TmpName);

				if (($iSize[0] > $width) || ($iSize[1] > $height) || $forceResize)
				{
					$dest = imagecreatetruecolor($width,$height);
						
					switch($fExt)
					{
						case "bmp":
							//$src = imagecreatefrombmp($this->p_TmpName);
							throw new Exception("No BitMap support (BMP). Please convert image to a valid format: jpg, gif, png");
							break;
						case "jpg":
						case "jpeg":
							$src = imagecreatefromjpeg($this->p_TmpName);
							break;
						case "gif":
							$src = imagecreatefromgif($this->p_TmpName);
							break;
						case "png":
							$src = imagecreatefrompng($this->p_TmpName);
							break;
						default:
							throw new Exception("Error, invalid image type uploaded. Please try again.");
					}
						
					if (!$src)
					{
						throw new Exception("Could not create base image file from uploaded file. Please try again and be sure the image is not corrupt!");
					}
						
					$retVal = imagecopyresampled($dest,$src,0,0,0,0,$width,$height,ImageSX($src),ImageSY($src));
						
					if (!$retVal) throw new Exception("Could not copy resampled image. Please try again and be sure the uploaded image is not corrupt!");
						
					switch($fExt)
					{
						case "jpg":
						case "jpeg":
							$retVal = imagejpeg($dest, $this->p_TmpName, $config["uploaded_image_quality"]);
							break;
								
						case "gif":
							$retVal = imagegif($dest, $this->p_TmpName);
							break;
								
						case "png":
							$retVal = imagepng($dest, $this->p_TmpName);
							break;
					}

					if (!$retVal) throw new Exception("Could not create resized image. Please try again and ensure that the uploaded image is not corrupt!");
				}
			}
				
			if (substr($location, -1) == "/")
			{
				$loc = $location."/".$this->p_Name;
			}
			else
			{
				$loc = $location;
			}			
							
			if (!move_uploaded_file($this->p_TmpName, $loc))
			{
				throw new Exception("Error uploading file ".$this->p_TmpName." to $loc. Please try again.");
			}
				
			$this->uploadComplete = true;
		}
		catch (Exception $exc)
		{
			if ($this->p_TmpName) @unlink($this->p_TmpName);
				
			throw $exc;
		}

		return true;
	}
	public function getFileExtension($str)
	{
		return fileHelper::getFileExtension($str);
	}
	public function getFileExtensionByMIME($file='')
	{
		if (!$file) $file = $this->p_TmpName;		
		return imageHelper::getExtensionByMIME($file);
	}
	public function getMimeType($file = '')
	{
		if (!$file) $file = $this->p_TmpName;		
		
		return fileHelper::getMimeType($file);
	}
	private function getSourceImageObject()
	{
		if ($this->uploadComplete) throw new Exception("Error, the upload for this file has already been completed.");

		return imageHelper::getImageObject($this->p_TmpName);		
	}
	public function convertImage($newType, $resizeDimensions="")
	{
		if ($this->uploadComplete) throw new Exception("Error, the upload for this file has already been completed.");
		
		// No size conversion, use the source dimensions (no scaling)
		if (!$resizeDimensions) $resizeDimensions = getimagesize($this->p_TmpName);

		$destImg = imagecreatetruecolor($resizeDimensions[0], $resizeDimensions[1]);
		$srcImg = $this->getSourceImageObject();

		if (!$srcImg) throw new Exception("Error creating source image.");

		imagecopyresampled($destImg,$srcImg,0,0,0,0,$resizeDimensions[0],$resizeDimensions[1],ImageSX($srcImg),ImageSY($srcImg));

		switch($newType)
		{
			case "bmp":
				imagewbmp($destImg, $this->p_TmpName);
				break;

			case "jpg":
			case "jpeg":
				imagejpeg($destImg, $this->p_TmpName, 85);
				break;

			case "gif":
				imagegif($destImg, $this->p_TmpName);
				break;

			case "png":
				imagepng($destImg, $this->p_TmpName);
				break;
		}

		//Destroy the resources
		imagedestroy($srcImg);
		imagedestroy($destImg);
	}
}
?>