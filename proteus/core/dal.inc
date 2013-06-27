<?
class dbConnect
{
	var $dbLink;
	var $connectionAttempts=0;
	
	function dbConnect()
	{
		global $config;
		require($config[baseAppDir]."includes/secure.php");
		
		$mysqli = mysqli_init();
		
		if (!$mysqli) 
		{
		    die('mysqli_init failed');
		}		
		
		//if (!$mysqli->real_connect($db_host, $db_userName, $db_pass, $db_name, null, null, MYSQLI_CLIENT_COMPRESS))
		$ct = 0;
		while (!$mysqli->real_connect($db_host, $db_userName, $db_pass, $db_name))
		{
			$ct++;
			sleep(1);
			
			if ($ct > 5)
			{
				error_log("Error connecting: ".mysqli_connect_error());
				die("We are sorry for the inconvenience, but the database is currently unavailable. This could be for scheduled maintenance, or a technical problem that we are looking into. Please check back later.");
			}	
		}
		
		$mysqli->set_charset("utf8");
		
		//Return resource identifier
    	$this->dbLink = $mysqli; 

		$config[dbLink] = $mysqli;	
	
		//Support for newer sites that reference the site type in the DB, not in the settings.php file.
		$d = new DAL(false);
		$d->suppressErrors = true;
		
		if ($d->qryCount("show tables like 'config'"))
		{
			/*$cObj = new customConfig(0);
			$cObj->loadKeys($config);*/
			$qry = "select *
					from config";
			$cQry = $d->qry($qry);
			
			if (!$cQry) throw new Exception();
			
			while($c = $cQry->fetch_assoc())
			{
				//Only store the config entries marked as a type of 0 (base config)
				if (!$c[typeID]) $config[$c[keyName]] = $c[keyValue];	
			}
		}		
		$config[siteTypeConfig] = true;
	}	
}
class dbSync
{
	var $table;	
	var $configuredTables;
	var $tCache;
	
	function dbSync($table='')	
	{			
		/*$this->tCache = new cache("configuredTables");
		
		if ($this->tCache->isExpired(120))
		{
			$this->configuredTables = array();	
		}		
		else
		{
			$this->tCache->loadObject($this->configuredTables);	
		}
		
		//Only attempt to sync on construction if a table is passed
		if ($table) $this->syncTable($table);*/
	}
	function syncTable($table='')
	{
		global $config;
		
		//For Proteus master servers
		if ($global[masterServer]) return;
		
		$d = new DAL();
		
		//Override constructor table if passed in
		if ($table) $this->table = $table;
		
		//Make sure the js folder is properly symlinked
		if (!file_exists($config[baseAppDir].'/admin/includes/js'))
		{			
			@symlink($config[scriptLocation].'admin/includes/js', $config[baseAppDir].'/admin/includes/js');
		}
		
		//Cache says this table is all set
		if ($this->configuredTables[$this->table]) return;
		
		//Twitter integration doesn't use the Proteus updates DB, just links the management console file.
		if ($table == "twitter")
		{
			if (!file_exists($config[baseAppDir].'/admin/twitter.php')) @symlink($config[scriptLocation].'admin/twitter.php', $config[baseAppDir].'/admin/twitter.php');
			return;	
		}
		
		$client = new phpSoap();    		
	    $setupData = $client->call('getTableSetup', array("tableName"=>$this->table));
		
	    if ($setupData === false) return;
	    
		$arr = $d->qryCount("show tables like '".$this->table."'");
		
    	//Add the table if it doesn't exist, use SOAP to get the right setup query
    	if (!$arr)
    	{
    		if (!strlen($setupData->item[0])) die("No table setup data returned for non-existent table ".$this->table.". Cannot continue!");
    	  	
    		//Separate setup query calls with ;; so that any additional queries can be run at the same time on the local db.
    		$tmp = explode(";;", $setupData->item[0]);
    		$cur = current($tmp);
    		
    		while(!($cur === false))
    		{
    			$d->qry($cur);
    			
    			$cur = next($tmp);
    		}
    	}
    	
    	$files = explode("::", $setupData->item[1]);
    	
    	foreach($files as $file)
    	{    	 
	    	//Now that the table has been created, see if there's file(s) that needs to be linked
			if (!file_exists($config[baseAppDir].'/admin/'.$file))
			{
				if (!@symlink($config[scriptLocation].'admin/'.$file, $config[baseAppDir].'/admin/'.$file)) error_log("Error linking file: $file");
			}		
    	}
    	
    	//Retrofitting Category Items...
		if ($table == "categoryItems" && !file_exists($config[baseAppDir].'/admin/categoryItems.ajax'))
		{
			@symlink($config[scriptLocation].'admin/categoryItems.ajax', $config[baseAppDir].'/admin/categoryItems.ajax');
		}
    	
		//Retrofit any core Proteus files that need to be added after initial installation
		if ($table == "config")
		{
			//$a = new pTemplate();
			
			//Added separate ajax file for showcases - 1/12			
			if (!file_exists($config[baseAppDir].'/admin/showcase.ajax')) @symlink($config[scriptLocation].'admin/showcase.ajax', $config[baseAppDir].'/admin/showcase.ajax');
			
			//Added seprate ajax file for categoryTypes - 1/12
			if (!file_exists($config[baseAppDir].'/admin/categoryTypes.ajax')) @symlink($config[scriptLocation].'admin/categoryTypes.ajax', $config[baseAppDir].'/admin/categoryTypes.ajax');
			
			//Added seprate ajax file for users - 2/12
			if (!file_exists($config[baseAppDir].'/admin/users.ajax')) @symlink($config[scriptLocation].'admin/users.ajax', $config[baseAppDir].'/admin/users.ajax');
		}
		
		$this->configuredTables[$this->table] = 1;
	}	
	function syncModule($core_class)
	{
		global $config;
		
		$refl = new ReflectionClass($core_class);
		$modName = $refl->getName();
			
		$cache = new cache($modName);
		$cache->loadObject($stat);
		
		if ($cache->isExpired(10) && $stat != -1)
		{	
			//Stop the race condition, mark as processing (probably entirely unnecessary)	
			$cache->storeObject(-1);
			
			try
			{
				$d = new DAL(true);
				$structObj = new dbStructUpdater();
				
				$baseLoc = $config[baseAppDir]."admin/";
				
				foreach((array)$core_class->files as $file)
				{
					//error_log($config[scriptLocation]."admin/$file");
					if (!file_exists($baseLoc.$file)) @symlink($config[scriptLocation]."admin/$file", $baseLoc.$file);
				}
					
				foreach((array)$core_class->tables as $table=>$structure)
				{				
					try
					{
						$d->suppressErrors = true;
						
						$res = $d->qryArray("show create table $table");
						$localStructure = $res[1];
					}
					catch(Exception $ex)
					{
						$localStructure = "";	
					}
					
					$changes = $structObj->getUpdates($localStructure, $structure);
		
					$d->suppressErrors = false;
					
					foreach((array)$changes as $alter)
					{
						$d->qry($alter);	
					}
					
					if (!$localStructure)
					{						
						$qArr = $core_class->queries[$table];

						foreach((array)$qArr as $qry)
						{
							$d->qry($qry);
						}
					}
				}
			}
			catch(Exception $ex)
			{
				$cache->clear();
				
				error_log("Error synchronizing module: $modName.\nSite: $config[scripturl]\nError:".$ex->getMessage(), 1, "scott@mstech.com");
				error("We're sorry, but an error has occurred updating your database. Our support staff have been notified and will look into this issue ASAP. Thank you for your patience.");
			}
			
			//Mark as processed	
			$cache->storeObject(1);
		}		
	}
	public function __destruct()
	{
		//$this->tCache->storeObject($this->configuredTables);
	}
	function checkForUpdates()
	{		
		$d = new DAL(true);
		
		$ver = $d->qryCount("select max(id) from dbSyncUpgrades");
		if (!$ver) $ver = 0;
		
		$client = new phpSoap();	
		$setupData = $client->call('getDBUpdate', array("minVer"=>$ver));		
		
		if (!$setupData) return;
		
		$setupData = explode("::", $setupData);		
		//if (!is_array($setupData)) return;
		
		foreach($setupData as $update)
		{
			$tmp = explode(";;", $update);
			
			$ct = 0;
			
			foreach($tmp as $qry)
			{			
				try
				{						
					//Allow multiple updates in one bundle
	    			$d->qry($qry);
				}
				catch(Exception $ex)
				{
					$failed[] = $ct;
				}
				
				$ct++;
			}
			
			$failed = implode("::", (array)$failed);
			
			//Synchronize the update data			
			$cv[alterQuery] = '"'.$update.'"';
			//$cv[failed] = "'$failed'";
			
			$d->qryInsertByArray('dbSyncUpgrades', $cv);
		}

		/*unset($cv);
		
		//Loop through any failed upgrades and attempt to run them again - fails silently on error.
		$uQry = $d->qry("select * from dbSyncUpgrades where success = 0");
		
		while($update = $uQry->fetch_assoc())
		{
			$tmp = explode(";;", $update[alterQuery]);
			
			foreach($tmp as $qry)
			{		
				try
				{
					$d->qry($update[alterQuery]);
					
					if (!$cv[failed]) $cv[failed] = 0;
				}			
				catch (Exception $ex) 
				{
					$cv[failed] = 1;
				}
			}
			
			$d->qryUpdateByArray("dbSyncUpgrades", $cv, "id=$update[id]");
		}*/
	}
}

class DAL
{	
	var $resultSets = array();
	var $suppressErrors = false;	
	var $throwException = false;
	
	function DAL($throwException=false)
	{		
		$this->throwException = $throwException;		
	}
	function __destruct()
	{	
		foreach($this->resultSets as $r)
		{	
			//Free any logged resultsets		
			@$r->free();	
		}					
	}
	function qry($qry, $logResult=true)
	{
		global $config;
		
		if ($config[dbLink])
		{
			global $sqldbg;
			
			if (isset($sqldbg)) $_SESSION[sqldebug] = $sqldbg;
			
			if ($sqldbg) $sv = getMicroTime();
			
			$retVal = mysqli_query($config[dbLink], $qry);
		}		
		
		if (mysqli_errno($config[dbLink]))
		{
			$mErr = mysqli_error($config[dbLink]);
			
			if (!$this->suppressErrors)
			{
				error_log(debug_string_backtrace());
				
				if (!$this->throwException) dbError($qry . "<br><hr width='70%'>".$mErr);
				
				error_log("Site: {$config[scripturl]}{$_SERVER['PHP_SELF']}\n--------\n".$mErr."\n--------\n".var_export(debug_backtrace(), true), 1, "scott@mstech.com");
				throw new Exception($config[debugMode] ? $mErr : "A Database error has occured, and a System Administrator has been notified. This information has been logged and someone will look into it as soon as possible, thank you for your patience!");
			}
		}
		
		//Add the result set to the internal array (that will be cleared on destruct)				
		if ($logResult && strtolower(substr($qry, 0, 6)) == "select")
		{		//echo "logged - ".get_class($retVal)." - $qry<hr />";
			array_push($this->resultSets, $retVal);
		}
		
		return $retVal;
	}	
	function qryArray($qry,$emptyError=false, $errMessage='')
	{
		$res = $this->qry($qry, false);
		
		if ($res)
		{		
			$retVal = $res->fetch_array();		
			$res->free();
		}
		elseif($emptyError==true)
		{
			if ($this->suppressErrors) return false;
			if ($this->throwException) throw new Exception($errMessage);			
			
			error($errMessage);
		}
		
		return $retVal;
	}
	function qryRow($qry)
	{		
		$qry = $this->qry($qry, false);
		$numRows = mysqli_num_rows($qry);
		
		mysqli_free_result($qry);
		
		return $numRows;
	}
	function qryCount($qry)
	{
		$val = $this->qryArray($qry);
		return $val[0];
	}
	function qryInsert($tableName, $columnList, $valueList, $errMess, $transType=0, $debug=false)
	{
		global $config;
		
		$vList = '';
		$cList = '';
		
		//validation::cleanValueList($valueList);			
		
		$cList = implode(',', $columnList);
		$vList = implode(',', $valueList);		
		
		$qry = "insert into $tableName ($cList) Values($vList)";
		
		if ($debug) echo($qry."<br>");
		
		$res = $this->qry($qry, false);		
		
		$id = mysqli_insert_id($config[dbLink]);
		
		if ($transType == 3 || !strlen($transType)) $this->commitTrans();		
		
		if (!$id)
		{
			//If the transType is Start/Process/Commit then roll it back!			
			//if ($transType) $this->rollbackTrans();
			error($errMess);
		}			
		
		return $id;	
	}
	function qryUpdate($tableName, $columnList, $valueList, $whereClause, $debug=false)
	{
		//validation::cleanValueList($valueList);			
		
		$stmt = "";
		
		for($i=0; $i < count($columnList); $i++)
		{
			$stmt .= "$columnList[$i]=$valueList[$i],";			
		}
		
		$stmt = substr($stmt, 0, strlen($stmt)-1);		
		
		$qry = "update $tableName set $stmt where $whereClause";
		
		if ($debug) error($qry);
		
		$res = $this->qry($qry, false);
	}
	function qryUpdateByArray($tableName, $cvList, $whereClause, $debug=false)
	{	
		$stmt = "";
		
		reset($cvList);
		$c = current($cvList);
		while(!($c===false))
		{
			$ky = key($cvList);
			
			$stmt .= "$ky=$c,";
			
			$c = next($cvList);	
		}
		
		$stmt = substr($stmt, 0, strlen($stmt)-1);		
		
		if (is_array($whereClause))
		{
			$whList = validation::quotedArray($whereClause);				
		
			$ct = 0;
			reset($whList);
			$c = current($whList);
			while(!($c===false))
			{
				$ky = key($whList);
				
				$whereList[$ct++] .= "$ky=$c";
				
				$c = next($whList);	
			}
			
			//Override the where clause with the array data if it exists
			$whereClause = implode(" AND ", $whereList);			
		}
		
		$qry = "update $tableName set $stmt where $whereClause";
		
		if ($debug) error($qry);
		
		$res = $this->qry($qry, false);
		
		return $res ? $res->affected_rows : null;
	}
	function qryInsertByArray($tableName, $cvlist, $errMess='')
	{		
		global $config;
		
		$ct = 0;
		
		reset($cvlist);
		$c = current($cvlist);
		while(!($c===false))
		{
			$ky = key($cvlist);

			$cList[$ct] = $ky;
			$vList[$ct] = $c;

			$ct++;
					
			$c = next($cvlist);	
		}
		
		
		$cList = implode(',', $cList);
		$vList = implode(',', $vList);		
		
		$qry = "insert into $tableName ($cList) Values($vList)";		
		
		$res = $this->qry($qry, false);				
		
		$id = mysqli_insert_id($config[dbLink]);
		
		if (!$id)
		{
			if (!$errMess) $errMess = mysqli_error($config[dbLink]);
			error($errMess);
		}			
		
		//Don't need this for inserts!! DUH!!!
		//@mysqli_free_result($res);
		
		return $id;	
	}
	function beginTrans()
	{
		@mysqli_query("START TRANSACTION");
	}
	function commitTrans()
	{
		@mysqli_query("COMMIT");
	}
	function rollbackTrans()
	{
		@mysqli_query("ROLLBACK");
	}
	function getTotalCount($qry)
	{			
		return $this->qryCount(preg_replace(array("/^select(.*?)from/is",
												  "/limit [0-9]+(\s?\,\s?[0-9]+)?/is",
												  "/order by (.*)/i"),
											array("select count(*) from",
												  "",
												  ""),
											$qry, 1));
	}
}
class phpDataset
{
	//A class used to somewhat simulate dataset usage - all errors throw exceptions.
	private $p_table;
	private $p_dataID;
	
	private $p_dirtyCols = array();
	protected $p_data;
	private $p_customSelect = '';
	private $p_customJoin = '';
	
	public $p_throw;
	public $emtpy_error_message = "Error, no valid entry specified. Please try again.";
	public $notfound_error_message = "Error, that entry could not be found!";
	
	protected function __construct($tableName, $dataID=0, $throwOnEmpty = true, $customSelect = '*', $customJoin = '')
	{	
		//Just in case...	
		validation::ensureInt($dataID, 0);
		
		$this->p_table = $tableName;
		$this->p_dataID = $dataID;
		$this->p_throw = $throwOnEmpty;
		
		$this->p_customJoin = $customJoin;
		$this->p_customSelect = $customSelect;
	}
	public function __get($key)
	{
		if (!isset($this->p_data))
		{					
			if (!$this->fill()) return;	
		}
		
		//Replace dollar signs with HTML equivalent (is this the best way to handle this?)
		if ($this->p_data[$key]) $this->p_data[$key] = str_replace('$','&#36;', $this->p_data[$key]);
		
		return $this->p_data[$key];	
	}
	public function fill()
	{
		if (!$this->p_dataID)
		{
			if ($this->p_throw) throw new Exception($this->emtpy_error_message);			
			return false;
		}
		
		//Late-binding the data structure the first time it's properties are accessed! (trick to get around static/dynamic methods and no overriding)
		$d = new DAL(true);
			
		$this->p_data = $d->qryArray("select {$this->p_customSelect} 
									 from {$this->p_table} {$this->p_customJoin} 
									 where id={$this->p_dataID}");		
		if (!$this->p_data[id] && $this->p_throw) throw new Exception($this->notfound_error_message);

		return true;
	}
	public function __set($key, $value)
	{
		if (!isset($this->p_data))
		{					
			$this->fill();	
		}
		
		//Don't mark as dirty if it's the same value
		if ($this->p_data[$key] == $value) return;
		
		//Fall through to set even if there is no data filled above		
		$this->p_dirtyCols[$key] = $value;
		$this->p_data[$key] = $value;
	}
	public function delete()
	{		
		if (!isset($this->p_data))
		{					
			if (!$this->fill()) return;	
		}
		
		$d = new DAL(true);
		return $d->qry("delete from {$this->p_table} where id={$this->id}")->affectedRows;
	}
	public function save($data="")	
	{
		if (is_array($data))
		{
			foreach($data as $key=>$value)
			{
				$this->$key = "'".validation::prepMultiLineForStorage($value)."'";	
			}
		}
		
		if (!count($this->p_dirtyCols)) return;		
		
		$d = new DAL(true);
		return $d->qryUpdateByArray($this->p_table, $this->p_dirtyCols, "id={$this->id}");	
	}
	protected function __clone()
	{
		//Unset the ID here so it can be re-inserted - will this ever be used?
		unset($this->p_data[id]);		
		$this->p_data[id] = $this->insert($this->p_data);
	}
	public function insert($data="")
	{		
		if ($this->p_dataID || $this->p_data[id]) throw new Exception("Error, this data entry already has an ID - cannot duplicate primary key.");
		
		if (is_array($data))
		{
			foreach($data as $key=>$value)
			{				
				$this->$key = "'".validation::prepMultiLineForStorage($value)."'";	
			}
		}
		
		$d = new DAL(true);		
		$insID = $d->qryInsertByArray($this->p_table, $this->p_dirtyCols);

		$this->p_dataID = $this->id = $insID;
		
		return $this->p_dataID;
	}
	public function cleanDirtyCols()
	{
		$this->p_dirtyCols = array();	
	}
	public function prepChangesForStorage($excludes=array())
	{
		$this->p_dirtyCols = validation::quotedArray($this->p_dirtyCols, $excludes);	
	}
	public function getDirtyColumns()
	{
		return $this->p_dirtyCols;	
	}
	public function getDataArray()
	{
		if (!isset($this->p_data))
		{					
			$this->fill();	
		}
		
		return $this->p_data;	
	}
}
?>
