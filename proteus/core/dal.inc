<?
class dbConnect
{	
	function __construct()
	{
		/*
		//Support for newer sites that reference the site type in the DB, not in the settings.php file.
		$d = new DAL(false);
		$d->suppressErrors = true;
		
		if ($d->qryCount("show tables like 'config'"))
		{
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
		$config[siteTypeConfig] = true; */
	}	
}
class DAL
{	
	private $resultSets = array();
	
	public $suppressErrors = false;	
	public $throwException = false;
	
	private $dbLink;
	
	public function __construct($throwException=false, $autoConnect=true)
	{		
		global $config;
		
		$this->throwException = $throwException;		
		if ($autoConnect) $this->connect();
	}
	private function connect()
	{
		global $config;
		
		// Share the same connection between instantiated DAL objects
		if ($config["dbLink"])
		{
			$this->dbLink = $config["dbLink"];
			return;
		}
		
		$mysqli = mysqli_init();
		
		if (!$mysqli)
		{
			$msg = "Unable to initialize Database interface.";
				
			if ($this->throwException) throw new Exception($msg);
			error($msg);
		}
		
		$ct = 0;
		while (!$mysqli->real_connect($config["db_host"], $config["db_user"], $config["db_pass"]))
		{
			$ct++;
			sleep(1);
		
			if ($ct > 5)
			{
				$msg = "We are sorry for the inconvenience, but the database is currently unavailable. This could be for scheduled maintenance, or a technical problem that we are looking into. Please check back later.";
		
				error_log("Error connecting: ".mysqli_connect_error());
		
				if ($this->throwException) throw new Exception($msg);
				die($msg);
			}
		}
		
		if (!$mysqli->select_db($config["db_name"]))
		{
			$msg = "Unable to select database: {$config["db_name"]} <br />Server Reply: ".mysqli_error($mysqli);
			
			if ($this->throwException) throw new Exception($msg);
			die($msg);
		}		
		
		// Set the Charset UTF-8
		$mysqli->set_charset("utf8");
		
		// Set the MySQL TimeZone so it matches the PHP server
		$mysqli->query("SET time_zone='{$config["server_timezone"]}'");
		
		$this->dbLink = $mysqli;
		$config["dbLink"] = $this->dbLink;
	}
	public function __destruct()
	{	
		foreach($this->resultSets as $r)
		{	
			//Free any logged resultsets		
			@$r->free();	
		}					
	}
	public function qry($qry, $logResult=true)
	{
		global $config;
		
		if (!$this->dbLink) $this->connect();
		
		$retVal = $this->dbLink->query($qry);		
		
		if ($this->dbLink->errno)
		{
			$mErr = $this->dbLink->error;
			
			if (!$this->suppressErrors)
			{
				if ($config["debug_mode"])
				{					
					if ($config["debug_email"])
					{
						error_log("Site: {$config[scripturl]}{$_SERVER['PHP_SELF']}\n--------\n".$mErr."\n--------\n".var_export(debug_backtrace(), true), 1, $config["debug_email"]);
					}
					
					if (!$this->throwException) error($mErr);
					throw new Exception($mErr);					
				}	
				else 
				{
					$msg = "We're sorry, but something seems to have gone wrong in the database. We've logged this error information, so please try again or submit a support ticket if the problem persists. Thank you.";
					
					if (!$this->throwException) error($msg);
					throw new Exception($msg);
				}				
			}
		}
		
		//Add the result set to the internal array (that will be cleared on destruct)				
		if ($logResult && strtolower(substr($qry, 0, 6)) == "select")
		{
			array_push($this->resultSets, $retVal);
		}		
		
		return $retVal;
	}	
	public function qryArray($qry)
	{
		if (!$this->dbLink) $this->connect();
		
		$res = $this->qry($qry, false);
		
		if ($res)
		{		
			$retVal = $res->fetch_array();		
			$res->free();
		}
		
		return $retVal;
	}
	public function qryRow($qry)
	{	
		$num_rows = 0;
			
		if (!$this->dbLink) $this->connect();
		
		$res = $this->qry($qry, false);
		
		if ($res)
		{
			$numRows = $res->num_rows;		
			$res->free();
		}
		
		return $numRows;
	}
	public function qryCount($qry)
	{
		$val = $this->qryArray($qry);
		return $val[0];
	}	
	public function qryUpdateByArray($tableName, $cvList, $whereClause)
	{	
		$stmt = "";
		
		foreach((array)$cvList as $ky=>$c)
		{	
			$tmp[] = "$ky=$c";
		}
		
		$stmt = implode(",", (array)$tmp);
				
		if (is_array($whereClause))
		{
			//$whList = validation::quotedArray($whereClause);				
		
			foreach($whereClause as $ky=>$wh)
			{
				// Support for 2 differerent syntax: array("column"=>"value") and array("column=value OR column2=value2")
				if (!is_numeric($ky))
				{
					$wTmp[] = "$ky='$wh'";
				}
				else
				{
					$wTmp[] = $wh;	
				}
			}
			
			//Override the where clause with the array data if it exists
			$whereClause = implode(" AND ", (array)$wTmp);					
		}
		
		$qry = "update ".($ignore ? "IGNORE " : "")."$tableName set $stmt where $whereClause";		
		
		if ($this->qry($qry, false)) return array($this->getMatchedRows(), $this->dbLink->affected_rows);

		return false;		
	}
	private function getMatchedRows()
	{		
		preg_match("/^Rows matched: ([0-9]?)/", $this->dbLink->info, $m);
		
		return $m[1];
	} 
	public function qryInsertByArray($tableName, $cvList, $errMess='', $highPriority=false)
	{
		foreach((array)$cvList as $ky=>$c)
		{			
			$cList[] = $ky;
			$vList[] = $c;
		}		
		
		$cList = implode(',', (array)$cList);
		$vList = implode(',', (array)$vList);		
		
		$qry = "insert ".($highPriority ? "HIGH_PRIORITY " : "")."into $tableName ($cList) Values($vList)";		
		
		$this->qry($qry, false);				
		
		$id = $this->dbLink->insert_id;		
				
		return $id;	
	}
	public function affectedRows()
	{		
		return $this->dbLink->affected_rows;	
	}
	public function beginTrans()
	{
		@mysqli_query("START TRANSACTION");
	}
	public function commitTrans()
	{
		@mysqli_query("COMMIT");
	}
	public function rollbackTrans()
	{
		@mysqli_query("ROLLBACK");
	}
	public function getTotalCount($qry)
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
		
		// Refill the object (to remove quotes, mysql functions, etc)
		$this->fill();
		
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
class dbSync
{
	private $table;
	private $configuredTables;
	
	function __construct() { }

	function syncModule($core_class)
	{
		global $config;

		$refl = new ReflectionClass($core_class);
		$modName = $refl->getName();
		
		try
		{
			$d = new DAL(true);
			$structObj = new dbStructUpdater();
				
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
			
			foreach((array)$core_class->asset_folders as $folder)
			{
				$loc = $config["baseAppDir"]."assets/$folder";

				if (!file_exists($loc)) mkdir($loc);
			}
		}
		catch(Exception $ex)
		{		
			error_log(print_r($ex, true));
			error("We're sorry, but an error has occurred updating your database. Our support staff have been notified and will look into this issue ASAP. Thank you for your patience.");
		}				
	}
	public function __destruct() { }	
}
?>
