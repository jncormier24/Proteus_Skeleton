<?php
abstract class permission_base
{
	//Setup bitwise constants
	const read = 1;
	const write = 2;
	const insert = 4;
	const delete = 8;

	const permission_generic = 0;
	
	public static function getPermissionArray($class)
	{
		$permObj = new ReflectionClass($class);
		$permArr = $permObj->getStaticProperties();
		
		ksort($permArr);

		return $permArr;
	}
	public static function getPermissionByKey($key, $class)
	{
		$pArr = permissions::getPermissionArray($class);
		
		foreach($pArr as $perm)
		{
			if ($perm["key"] == $key) return $perm;	
		}
		
		return false;
	}	
	public static function getPermissionEntry($entryID)
	{
		validation::ensureInt($entryID);
		if (!$entryID) throw new Exception("No permission entry specified.");

		$d = new DAL(true);
		
		$entry = $d->qryArray("select * from permissionsAssignment where id=$entryID");
		if (!$entry["id"]) throw new Exception("Error, that permission entry could not be found.");
		
		return $entry;
	}
	public static function getPermissionAssignments($permission, $dataID=0, $permissionsMask=0, $userID=0)
	{
		$d = new DAL(true);
		$perms = array();
		
		validation::ensureInt($dataID, 0);
		if ($dataID) $wh[] = "dataID = $dataID";
		
		validation::ensureInt($userID);
		if ($userID) $wh[] = "users.id=$userID";
		
		validation::ensureInt($permissionMask, 0);		
		
		if (count($wh)) $whCl = "and ".implode(" and ", $wh);
		
		$qry = "select permissionsAssignment.id, userID, bitValue, dataID
				from permissionsAssignment inner join users on users.id = permissionsAssignment.userID
				where permissionKey='".$permission[key]."' and users.inactive = 0 $whCl
				order by users.lastName asc";
		$uQry = $d->qry($qry);
		
		while($entry = $uQry->fetch_assoc())
		{
			if (!$permissionMask || ($entry[bitValue] & $permissionsMask))
			{
				$perms[$entry["id"]] = new security($entry["userID"]);
				$perms[$entry["id"]]->permDataID = $entry["dataID"];		
			}
		}			
		
		return $perms;	
	}
	public static function removePermissionAssignment($permissionEntryID)
	{
		$assn = self::getPermissionEntry($permissionEntryID);		
		
		$d = new DAL(true);		
		$d->qry("delete from permissionsAssignment where id={$assn["id"]}");
		
		return true;	
	}
}
class permissions extends permission_base
{	
	//Using constants instead of Database to make for an easy centralized location to modify permission entries without having to use a webservice (SOAP, et al).
	
	// -- Top-level group assignments: Set "isPermissionGroup" on any permission entry that should be assigned as a top-level group assignment (no R/W/I/D, just group assignment)
	public static $site_admin = array("key"=>"site_admin", "standalone"=>true, "description"=>"Grants full R/W/I/D permissions to all site functionality. Assign with Caution!", "isPermissionGroup"=>true);
	
	// -- Standalone permissions (not assigned to any particular data ID)	
	//public static $manage_category_items = array("key"=>"manage_category_items", "max_perms"=>15, "description"=>"Manage all Category Items","standalone"=>true,"siteType"=>proteus_core::module_categoryItems);
	public static $manage_showcases = array("key"=>"manage_showcases", "max_perms"=>15, "description"=>"Manage all Showcases","standalone"=>true,"siteType"=>proteus_core::module_showcase);
	public static $manage_users = array("key"=>"manage_users", "max_perms"=>15, "description"=>"Manage System Users","standalone"=>true,"siteType"=>proteus_core::module_users);	
	public static $manage_category_types = array("key"=>"manage_category_types", "max_perms"=>15, "description"=>"Manage all Category Types","standalone"=>true,"siteType"=>proteus_core::module_categoryItems);
	public static $manage_custom_forms = array("key"=>"manage_custom_forms", "max_perms"=>15, "description"=>"Manage all Custom Forms","standalone"=>true,"siteType"=>proteus_core::module_customForms);
	public static $manage_blogs = array("key"=>"manage_blogs", "max_perms"=>15, "description"=>"Manage all Blog Categories","standalone"=>true,"siteType"=>proteus_core::module_blogs);
	public static $manage_blog_feeds = array("key"=>"manage_blog_feeds", "max_perms"=>15, "description"=>"Manage Blog Feeds","standalone"=>true,"siteType"=>proteus_core::module_blogs);
	public static $manage_meta_data = array("key"=>"manage_meta_data", "max_perms"=>15, "description"=>"Manage site Meta Data","standalone"=>true,"siteType"=>proteus_core::module_metaData);
	public static $inline_edit = array("key"=>"inline_edit", "max_perms"=>3, "description"=>"Inline Editor Access","standalone"=>true);
	public static $manage_calendars = array("key"=>"manage_calendars", "max_perms"=>15, "description"=>"Manage all Calendars","standalone"=>true,"siteType"=>proteus_core::module_calendars);
	public static $manage_custom_content = array("key"=>"manage_custom_content", "max_perms"=>15, "description"=>"Manage Custom Content Pages","standalone"=>true,"siteType"=>proteus_core::module_customContent);
	public static $manage_twitter = array("key"=>"manage_twitter", "max_perms"=>15, "description"=>"Manage Twitter Implementation (where applicable)","standalone"=>true,"siteType"=>proteus_core::module_twitter);
	public static $site_configuration = array("key"=>"site_configuration", "max_perms"=>3, "description"=>"Configure site options and modules","standalone"=>true);
	public static $manage_inventory = array("key"=>"manage_inventory", "max_perms"=>15, "description"=>"Manage inventory and manufacturers","standalone"=>true,"siteType"=>proteus_core::module_inventory);
	public static $manage_municipal_entities = array("key"=>"manage_municipal_entities", "max_perms"=>15, "description"=>"Manage All Municipal Data","standalone"=>true,"siteType"=>proteus_core::module_municipal);
	public static $manage_subscribers = array("key"=>"manage_subscribers", "max_perms"=>15, "description"=>"Manage All Subscriber Information","standalone"=>true,"siteType"=>proteus_core::module_subscribers);
	
	// -- Dynamic permissions (assigned to an item in the db)
	public static $modify_category_type = array("key"=>"modify_category_type", "max_perms"=>15, "description"=>"Modify specific Category Type and all items.","standalone"=>false, "siteType"=>proteus_core::module_categoryItems);
	public static $modify_blog_category = array("key"=>"modify_blog_category", "max_perms"=>15, "description"=>"Modify specific Blog Category Type and all Entries.","standalone"=>false, "siteType"=>proteus_core::module_blogs);
	public static $modify_showcase = array("key"=>"modify_showcase", "max_perms"=>15, "description"=>"Modify specific Showcase and all Photos.","standalone"=>false, "siteType"=>proteus_core::module_showcase);
	public static $modify_custom_form = array("key"=>"modify_custom_form", "max_perms"=>11, "description"=>"Modify specific Form and view reports.","standalone"=>false, "siteType"=>proteus_core::module_customForms);
	public static $modify_calendar = array("key"=>"modify_calendar", "max_perms"=>15, "description"=>"Modify specific Calendar and all Entries.","standalone"=>false, "siteType"=>proteus_core::module_calendars);
	public static $municipal_entity = array("key"=>"municipal_entity", "max_perms"=>15, "description"=>"Manage Entity-specific municipal data","standalone"=>false, "siteType"=>proteus_core::module_municipal);

	public static function getPermissionArray()
	{
		global $config;
		return permission_base::getPermissionArray($config["permission_class"]);	
	}
	public static function getPermissionByKey($key)
	{
		global $config;
		return permission_base::getPermissionByKey($key, $config["permission_class"]);	
	}	
	public static function getAuthorizedUser($login="", $pass="", $setLastLogin=true)
	{
		global $config;
	
		$d = new DAL(true);	
		
		$secObj = new security(0);
		
		// pageClass by default will initialize and attempt to get available login information.
		// If login information is never SET, this will always return false without needing a trip to the DB
		
		if ($_SESSION["username"] && (!$login && !$pass))
		{
			$login = $_SESSION["username"];
			$pass = $_SESSION["password"];
		}
		else
		{
			if (!$pass) return $secObj;
			$pass = crypt($pass, 'pr');
			
			$force = true;
		}
		
		// No information found, nobody has logged in yet
		if (!$login || !$pass) return $secObj;
		
		//Once validated, don't re-validate for another 5 minutes
		$sCache = new session_cache("login");
		$secObj = $sCache->loadObject($secObj);		
		
		if ($sCache->isExpired(5) || !$secObj->id || $force)
		{			
			//Check local users database for authentication if configured
			$qry = "select * from users where login='$login' and password='$pass'";
			$obj = $d->qryArray($qry);
	
			//Set a new security object here if the local DB authenticated - otherwise it'll be a blank security object
			if ($obj[id])
			{ 
				$secObj = new security($obj["id"]);
	
				$_SESSION["username"] = $login;
				$_SESSION["password"] = $pass;	
				
				$sCache->storeObject($secObj);
			}			
		}	
		
		if ($secObj->id && $setLastLogin)
		{
			$secObj->lastLogin = "now()";				
		}
		
		return $secObj;
	}
	public static function authenticateAjax($secure=true)
	{
		// This function is used in conjunction with the standard Proteus post-request bootstrapper ($.ajaxError)
		$sec = permissions::getAuthorizedUser();
		
		if (!$sec->id && $secure)
		{
			$js = new json_a();
			$js->error = "You must be logged in to access this content.";

			echo $js->render();
			exit(0);
		}
		
		return $sec;
	}
}
class security_a extends security
{
	public function __construct($userID)
	{		
		parent::__construct($userID);
		
		if (!$this->id) throw new Exception("Error, that user could not be found ($userID).");
	}
}
class security
{
	public $userID;
	private $account_data;
	
	//Permission key-based assignment
	public $key_access = array();
	private $perm_array = array();
	
	//private $global_admin = false;
	
	private $settings;
	private $notification_settings;
	
	public function __construct($userID=0)
	{
		global $config;		
		
		validation::ensureInt($userID);
		$this->userID = $userID;
	}	
	public function __get($key)
	{		
		if (!count($this->account_data)) $this->account_data = security::getAccountFromID($this->userID, false);
		if (!$this->account_data["id"]) return;
		 
		switch($key)
		{
			case "global_admin":
				return $this->is(permissions::$site_admin);
				
			case "notification_settings":
				$this->notification_settings = new notificationsConfig($this);
				return $this->notification_settings;
				
			case "settings":
				$this->settings = new userConfig($this);
				return $this->settings;
				
			case "fullName":
				return $this->account_data[firstName] && $this->account_details[data] ? $this->account_data[lastName].", ".$this->account_data[firstName] : $this->account_data[login];

			default:
				return $this->account_data[$key];
		}		
	}
	public function __set($key, $value)
	{
		if (!$this->id) throw new Exception("No user specified.");
		
		$d = new DAL(true);
		
		$cv[$key] = $value;
		
		return $d->qryUpdateByArray("users", $cv, array("id"=>$this->id));
	}
	public function is($permission, $dataID='', $explicit=false)
	{
		return $this->hasAccess($permission, 1, $dataID, $explicit);	
	}
	public function hasAccess($permission, $accessLevels, $dataID = 0, $explicit=false)
	{			
		if (!$explicit && $this->global_admin) return true;
		if (!$this->userID) return false;
		
		//Stored array of access keys with Bitwise comparison. Stops multiple DB lookups for the same key.
		if (!$dataID && (($this->key_access[$permission["key"]] & $accessLevels) || ($this->key_access[$permission["key"]] && !$accessLevels))) return true;		
		
		if (!$permission[key]) return false;		

		$permAssn = $this->getPermissionAssignment($permission, $dataID);
		if (!$permAssn[id]) return false;
		
		//BITWISE COMPARISON!
		if (($permAssn[bitValue] & $accessLevels) || !$accessLevels)
		{
			$this->key_access[$permission["key"]] = $permAssn["bitValue"];
			return true;
		}	
		
		return false;
	}
	public function getPermissionAssignments($permission, $dataID=0, $permissionsMask=0)
	{
		return permission_base::getPermissionAssignments($permission, $dataID, $permissionsMask, $this->id);	
	}
	public function getPermissionAssignment($permission, $dataID=0)
	{		
		$d = new DAL(true);
		
		validation::ensureInt($dataID, 0);
		
		if ($dataID) $dWh = "and dataID='$dataID'";
		
		$permAssn = $d->qryArray("select * from permissionsAssignment where permissionKey='$permission[key]' and userID=$this->userID and typeID = 0 $dWh");
		if (!$permAssn[id]) return false;
		
		return $permAssn;
	}	
	public function addPermissionAssignment($permission, $dataID=0, $bitVal=1)
	{
		$d = new DAL(true);
		
		validation::ensureInt($dataID, 0);
		
		$key = $permission[key];
		
		$permAssn = $this->getPermissionAssignment($permission, $dataID);						
				
		if ($permAssn[id])
		{
			throw new Exception("Error, that permission assignment ($key) already exists!");			
		}
		else
		{
			$cv[userID] = $this->id;
			$cv[permissionKey] = "'$key'";
			$cv[dataID] = $dataID;
			$cv[bitValue] = $bitVal;
									
			return $d->qryInsertByArray("permissionsAssignment", $cv);
		}
	}	
	public static function getAccountFromID($userID, $verbose=true)
	{	
		validation::ensureInt($userID);
			
		if (!$userID)
		{
			if ($verbose) error("Error getting account details: No account ID was specified!");			
			return false;
		}
		
		if ($userID == -1)
		{
			$acct[login] = "System Administrator";				
			$acct[id] = -1;
		}
		else
		{
			$d = new DAL();
			$acct = $d->qryArray("SELECT * FROM users WHERE id=$userID");	
			
			if (!$acct[id])
			{					
				if ($verbose) error("Error, that user does not seem to exist! ID: $userID");					
				return false;
			}
		}
		
		return $acct;		
	}
	public function security_error_a($msg='')
	{
		// Just a wrapper for ajax requests to make the generic error throw an exception instead
		$this->security_error($msg, true);
	}
	public function security_error($msg = '', $throwException = false)
	{
		if (!$msg) $msg = "Error, you do not have access to this area! This error has been logged for review.";
		
		$acct = security::getAccountFromID($this->userID, false);
		
		if (!$acct[id])
		{
			//Anonymous Security Breach...
			$acct[name] = "Anonymous";			
		}		
		else
		{
			$eInfo = "Account ID: $acct[id]";
		}		
		
		$iAddr = $_SERVER["REMOTE_ADDR"];		
		$details = $acct[name]." user attempted access to restricted area.\nIP Address: $iAddr \nCalling file: {$_SERVER["PHP_SELF"]} \n".$eInfo;		
		
		error_log($details);		
		
		if (!$throwException) error($msg);
		throw new Exception($msg);
	}
}
class securityEntry
{
	private $values;
	
	function securityEntry($value)
	{		
		$this->addEntry($value);		
	}	
	function addEntry($value)
	{
		$ct = count($this->values);		
		$this->values[$ct] = $value;	
	}
	function keyExists($value)
	{			
		//return array_search($value, $this->values);
		return in_array($value, $this->values);
	}
}
?>