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

	public static $module_comments = array("siteType"=>0, "required"=>true, "location"=>"proteus/modules/comments");
	public static $module_customContent = array("siteType"=>3, "required"=>true, "location"=>"proteus/modules/customContent");
	
	public $files = array("sitemap.ajax");	
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
									) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	
	public $queries = array("config"=>array("insert into config (keyName, keyValue) values('hitCount',0)",
											"insert into config (keyName, keyValue) values('version','4.0')",
											"insert into config (keyName, keyValue) values('siteType','2::4::5::6')"));
	
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
	public static function includeModule(array $module)
	{
		global $config;
		include_once($config["baseAppDir"].$module["location"]."/model.inc");
	}
	public static function syncAdminModules()
	{
		$db = new dbSync();		
		
		//Sync all the core Proteus modules
		$db->syncModule(new proteus_core());
		$db->syncModule(new customComments_core());
		$db->syncModule(new customContent_core());
		$db->syncModule(new proteus_user_core());
		
		//$conf = new siteConfig();
		
		// Sync all the installed Proteus admin modules
		/* if ($conf->checkSiteType(proteus_core::module_categoryItems)) $db->syncModule(new customCategory_core());
		if ($conf->checkSiteType(proteus_core::module_showcase)) $db->syncModule(new customShowcase_core());
		if ($conf->checkSiteType(proteus_core::module_calendars)) $db->syncModule(new customCalendar_core());
		if ($conf->checkSiteType(proteus_core::module_inventory)) $db->syncModule(new customInventory_core());
		if ($conf->checkSiteType(proteus_core::module_customForms)) $db->syncModule(new customForm_core());
		if ($conf->checkSiteType(proteus_core::module_blogs)) $db->syncModule(new customBlog_core());
		if ($conf->checkSiteType(proteus_core::module_twitter)) $db->syncModule(new twitter_core());
		if ($conf->checkSiteType(proteus_core::module_metaData)) $db->syncModule(new metaData_core());
		if ($conf->checkSiteType(proteus_core::module_municipal)) $db->syncModule(new pmm_core());
		if ($conf->checkSiteType(proteus_core::module_subscribers)) $db->syncModule(new subscriber_core()); */
	}
}
class customConfig
{
	private $p_typeID = 0;
	private $p_dataID = 0;

	protected $p_data = array();
	protected $p_defaults = array();

	const config_site = 0;
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
	
	public function __construct()
	{
		parent::__construct(customConfig::config_site);		
	}
	public function __get($key)
	{
		return $this->p_data[$key]->$key;		
	}
	public function __set($key, $value)
	{
		return $this->p_data[$key]->$key = $value;
	}
	public function checkSiteType($type)
	{			
		$tmp = explode("::", $this->siteType);
	
		foreach($tmp as $val)
		{
			if ($val == $type) return true;
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
?>