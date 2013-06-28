<?php
class blank_security extends security 
{		
	public function __construct() {}
}
abstract class permission_base
{
	//Setup bitwise constants
	const read = 1;
	const write = 2;
	const insert = 4;
	const delete = 8;	
	
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
			if ($perm[key] == $key) return $perm;	
		}
		
		return false;
	}	
	public static function getPermissionEntry($entryID)
	{
		validation::ensureInt($entryID);
		if (!$entryID) throw new Exception("No permission entry specified.");

		$d = new DAL(true);
		
		$entry = $d->qryArray("select * from permissionsAssignment where id=$entryID");
		if (!$entry[id]) throw new Exception("Error, that permission entry could not be found.");
		
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
				$perms[$entry[id]] = new security($entry[userID]);
				$perms[$entry[id]]->permDataID = $entry[dataID];		
			}
		}			
		
		return $perms;	
	}
	public static function removePermissionAssignment($permissionEntryID)
	{
		$assn = self::getPermissionEntry($permissionEntryID);		
		
		$d = new DAL(true);		
		$d->qry("delete from permissionsAssignment where id=$assn[id]");
		
		return true;	
	}
}
class permissions extends permission_base
{	
	//Using constants instead of Database to make for an easy centralized location to modify permission entries without having to use a webservice (SOAP, et al).
	
	// -- Top-level group assignments: Set "isPermissionGroup" on any permission entry that should be assigned as a top-level group assignment (no R/W/I/D, just group assignment)
	public static $site_admin = array("key"=>"site_admin", "standalone"=>false, "isPermissionGroup"=>true);
	
	// -- Standalone permissions (not assigned to any particular data ID)
	public static $manage_category_items = array("key"=>"manage_category_items", "max_perms"=>15, "description"=>"Manage all Category Items","standalone"=>true,"siteType"=>proteus_core::module_categoryItems);
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
		return permission_base::getPermissionArray('permissions');	
	}
	public static function getPermissionByKey($key)
	{
		return permission_base::getPermissionByKey($key, 'permissions');	
	}	
	public static function getAuthorizedUser($login="", $pass="")
	{
		global $config;
	
		$d = new DAL(true);	
		
		// pageClass by default will initialize and attempt to get available login information.
		// If login information is never SET, this will always return false
		
		if ($_SESSION[username] && (!$login && !$pass))
		{
			$login = $_SESSION[username];
			$pass = $_SESSION[password];
		}
		else
		{
			$pass = crypt($pass, 'pr');
		}
		
		// No information found, nobody has logged in yet
		if (!$login || !$pass) return false;
	
		//Once validated, don't re-validate for another 20 minutes
		$sCache = new session_cache("login");
		$sCache->loadObject($secObj);
	
		if ($sCache->isExpired(20) || !$secObj)
		{
			//Check local users database for authentication if configured
			$qry = "select * from users where login='$login' and password='$pass'";
			$obj = $d->qryArray($qry);
	
			//Set a new security object here if the local DB authenticated - otherwise it'll be a blank security object
			if ($obj[id])
			{ 
				$secObj = new security($obj[id], true, $config[partialSecurityClass]);
	
				$_SESSION[username] = $login;
				$_SESSION[password] = $pass;
	
				$sCache->storeObject($secObj);
				
				return $secObj;
			}
		}	
		
		return $secObj ? $secObj : false;
	}
}
class security
{
	public $userID;
	private $sMatrix;
	private $dal;
	public $errMsg;
	private $account_details;
	
	//Permission key-based assignment
	public $key_access = array();
	private $perm_array = array();
	
	//Additional information to be sent in the join email for each group
	private $additionalJoinInfo;	

	private $partialSecClass;	
	
	function __construct($userID='', $populateSec=true, $partialSecurityClass='')
	{
		global $config;

		$this->dal = new DAL(true);		
		
		if (!$partialSecurityClass && $config[partialSecurityClass]) $partialSecurityClass = $config[partialSecurityClass];
		
		//An instance of an extending security class (for custom security configs)
		if ($partialSecurityClass) $this->partialSecClass = $partialSecurityClass;
		
		validation::ensureInt($userID);
		
		if (!$userID) 
		{
			// No user ID specified
			return false;
		}
		else
		{
			$this->userID = $userID;
			$this->account_details = $this->getAccountFromID($this->userID, true);
			
			if ($populateSec && $userID) 
			{				
				$this->populateSecurity();				
			}
			
			return true;	
		}		
	}	
	public function __call($method, $args)
	{
		if (is_array($args))
		{
			array_push($args, $this);	
		}
		else
		{
			$args = array($this);
		}
		
		$method = "{$this->partialSecClass}::{$method}";
		
		if (is_callable($method)) return call_user_func_array($method, $args);
	}
	public function __get($key)
	{		
		switch($key)
		{
			case "fullName":
				return $this->account_details[firstName] && $this->account_details[lastName] ? $this->account_details[lastName].", ".$this->account_details[firstName] : $this->account_details[login];

			default:
				return $this->account_details[$key];
		}		
	}
	public function is($permission, $dataID='', $explicit=false)
	{
		return $this->hasAccess($permission, 1, $dataID, $explicit);	
	}
	public function hasAccess($permission, $accessLevels, $dataID = '', $explicit=false)
	{			
		if (!$explicit && ($this->is(permissions::$site_admin) || $this->proteusUser)) return true;
		if (!$this->userID) return false;
		
		//Stored array of access keys with Bitwise comparison. Stops multiple DB lookups for the same key.
		if (!$dataID && (($this->key_access[$permission[key]] & $accessLevels) || ($this->key_access[$permission[key]] && !$accessLevels))) return true;		
		
		if (!$permission[key]) return false;		

		$permAssn = $this->getPermissionAssignment($permission, $dataID);
		if (!$permAssn[id]) return false;
		
		//BITWISE COMPARISON!
		if (($permAssn[bitValue] & $accessLevels) || !$accessLevels)
		{
			$this->key_access[$permission[key]] = $permAssn[bitValue];
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
	public function populateSecurity()
	{	
		global $login;
		
		if (!$this->userID) return false;
		
		$qry = "select rightsMatrix.name, rightsAssignment.matrixValue
				from rightsAssignment inner join rightsMatrix on rightsMatrix.id = rightsAssignment.matrixID
				where rightsAssignment.userID=".$this->userID;	
		$rQry = $this->dal->qry($qry);
		
		while($entry = $rQry->fetch_assoc())
		{			
			if (empty($this->sMatrix[$entry[name]]))
			{
				$this->sMatrix[$entry[name]] = new securityEntry($entry[matrixValue]);	
			}
			else
			{
				$this->sMatrix[$entry[name]]->addEntry($entry[matrixValue]);
			}
		}			
		
		$strkey = "global_admin";	
		$this->global_admin = ((!empty($this->sMatrix[$strkey]) && !($this->sMatrix[$strkey]->keyExists($value)===false)) ? 1 : 0);						
	}
	public function check_strkey($strkey='', $value='', $verbose=false, $explicit=false)
	{	
		if (!$this->userID) return false;
		if ($this->global_admin && !$explicit) return true;
		
		if ($this->sMatrix[$strkey] && (!$value || $this->sMatrix[$strkey]->keyExists($value)))
		{
			$retVal = true;	
		}
		else
		{	
			$retVal = false;
		}
		
		if (!$retVal && $verbose) error("Error, this account (ID:".$this->userID.") does not have security access to this function!");
		
		return $retVal;		
	}
	public function check_key($key, $value, $verbose=false)
	{	
		if (!$this->userID) return false;
		if ($this->global_admin) return true;
		
		$qry = "select name from rightsMatrix where id=$key";
		$key = $this->dal->qryCount($qry);		
		
		return $this->check_strkey($key, $value, $verbose);
	}
	public function check_menuAccess($baseName, $verbose=true)
	{
		//Global Admins and Proteus (remote) users have full access
		if ($this->global_admin || $this->proteusUser) return true;
		
		return $this->check_strkey('menu_access', $baseName, $verbose);	
	}
	public function key_exists($strkey)
	{
		//This is used for assignments that don't require a specific value to check, or just looking for 1 instance that's not specific
		return (empty($this->sMatrix[$strKey]) ? false : true);	
	}
	public function getMatrix($groupID='')
	{
		if ($groupID) $wClause = " where groupID = $groupID";
		
		$qry = "select id, name, description, groupID, displayName
				from rightsMatrix".$wClause;	
		
		return $this->dal->qry($qry);
	}
	public function getMatrixByName($strKey)
	{
		$qry = "select id, description, displayName
				from rightsMatrix
				where name='$strKey'";	
		
		return $this->dal->qryArray($qry, true,"Error, that matrix doesn't seem to exist. Please ensure proper database installation!");
	}
	public function getMatrixByID($id)
	{
		validation::ensureInt($id);
		
		$qry = "select id, description, displayName, groupID, name
				from rightsMatrix
				where id=$id";	
		
		return $this->dal->qryArray($qry, true,"Error, that matrix doesn't seem to exist. Please ensure proper database installation!");
	}
	public function getAccountsByKeyValue($strKey, $value='', $countOnly=false)
	{
		if ($value) $vWhere = "and rightsAssignment.matrixValue = '$value'";
		
		$qry = "select users.id, users.firstName, users.lastName, matrixValue, rightsAssignment.id 'rightsID', users.email
				from users inner join rightsAssignment on rightsAssignment.userID = users.id
							 inner join rightsMatrix on rightsMatrix.id = rightsAssignment.matrixID
				where rightsMatrix.name = '$strKey' $vWhere";
		
		if ($countOnly)
		{
			return $this->dal->qryRow($qry);
		}
		else
		{
			return $this->dal->qry($qry, false);
		}
	}	
	public function addEntry($strKey, $strVal, $userID, $emailNotify=false)
	{
		$matrix = $this->getMatrixByName($strKey);
		
		$qry = "select id from rightsAssignment where matrixID=$matrix[id] and userID=$userID and matrixValue='$strVal'";
		
		if (!$this->dal->qryCount($qry))
		{
			$cList[0] = "matrixID";
			$cList[1] = "matrixValue";
			$cList[2] = "userID";
			
			$vList[0] = $matrix[id];
			$vList[1] = "'$strVal'";	
			$vList[2] = $userID;
			
			$id = $this->dal->qryInsert("rightsAssignment", $cList, $vList);
			
			if (!$id)
			{
				$this->errMsg = "Error adding admin. Please try again or contact support if problem persists!";
			}
			else
			{
				//if ($emailNotify) $this->sendGroupJoinEmail($userID, $strKey);	
			}
			
			return $id;
		}
		else
		{
			$this->errMsg = "This user is already setup with these rights!";
			return 0;
		}
	}
	public function removeEntry($entryID)
	{	
		global $config;
		
		$qry = "delete from rightsAssignment where id=$entryID";
		
		$this->dal->qry($qry);
		
		$retVal = mysqli_affected_rows($config[dbLink]);
		
		if (!$retVal) $this->errMsg = "Error removing admin entry. Possible cause: This user was not setup with the access you are trying to remove.";
		
		return $retVal;
	}	
	public function removeByAccountKey($userID, $strKey, $strVal)
	{	
		global $config;
		
		//use this to remove an entry if you don't have a specific entry ID.
		$matrix = $this->getMatrixByName($strKey);
		
		$qry = "delete from rightsAssignment where matrixID=$matrix[id] and matrixValue='$strVal' and userID=$userID";
		
		$this->dal->qry($qry);
		
		$retVal = mysqli_affected_rows($config[dbLink]);
		
		if (!$retVal) $this->errMsg = "Error removing admin entry. Possible cause: This user was not setup with the access you are trying to remove.";
		
		return $retVal;
	}
	public function getAccountFromID($userID='', $verbose=true)
	{		
		if (!$userID)
		{
			$this->errMsg = "Error getting account details: No account ID was specified!";
			if ($verbose) error($this->errMsg);
			
			return false;
		}
		else
		{			
			if ($userID == -1)
			{
				$acct[login] = "System Administrator";				
				$acct[id] = -1;
			}
			else
			{
				$acct = $this->dal->qryArray("SELECT * FROM users WHERE id=$userID");	
				
				if (!$acct[id])
				{
					$this->errMsg = "Error, this user does not seem to exist! ID:$userID";
					if ($verbose) error($this->errMsg);
					
					return false;
				}
			}
			
			return $acct;			
		}	
		
	}
	public function populateAccountDetails()
	{
		if ($this->userID) $this->account_details = $this->getAccountFromID($this->userID,false);
	}	
	public function security_error_a($msg='')
	{
		$this->security_error($msg, true);
	}
	public function security_error($msg = '', $throwException = false)
	{
		if (!$msg) $msg = "Error, you do not have access to this area! This error has been logged for review.";
		
		$acct = $this->getAccountFromID($this->userID, false);
		
		if (!$acct[id])
		{
			//Anonymous Security Breach...
			$acct[name] = "Anonymous";			
		}		
		else
		{
			$eInfo = "Account ID: $acct[id]";
		}		
		
		$iAddr = getenv("REMOTE_ADDR");			
		$details = $acct[name]." user attempted access to restricted area.\nIP Address: $iAddr \nCalling file: {$_SERVER[PHP_SELF]} \n".$eInfo;		
		
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