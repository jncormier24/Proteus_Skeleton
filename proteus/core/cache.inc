<?php
class cache
{
	/*
	 * The initial caching mechanism was file-based, however due to limitations on number of directors per folder in Linux (32,768) it became obvious that a DB solution
	* needed to be integrated. This is the parent class that creates an abstraction layer to cache data into the database where it can be optimized via mySQL optimization
	* and internal mySQL caching. I believe this option will be faster in the long run, and requires less cache maintenance to clean out stale folders via CRON routine.
	*
	* This system creates a single ID for a cached entry associated with a particular dataID, typeID, and contentKey and then attempts to keep the same incremental ID so as
	* to not have to worry about auto_increment limitations down the line (an admittedly highly paranoid approach considering the outrageously large max)
	*/

	const cache_generic = 1;
	const cache_permissions = 2;
	
	protected $p_data;
	protected $p_typeID;
	protected $p_contentKey;
	protected $p_dataID;
	protected $p_ttl;

	public function __construct($typeID, $contentKey, $dataID=0)
	{
		validation::ensureInt($typeID);
		validation::ensureInt($dataID);

		//A dataID of 0 (zero) is OK; this is system cached data
		$this->p_typeID = $typeID;
		$this->p_contentKey = $contentKey;
		$this->p_dataID = $dataID;

		$this->refreshData();
	}
	private function refreshData()
	{
		global $config;

		if (!$this->p_contentKey) return;

		$d = new DAL(true);

		//This needs to be refreshed before a save just to make sure we don't have a dup key issue!
		$this->p_data = $d->qryArray("select dataID, typeID, contentKey, content, unix_timestamp(addedDateTime) as 'addedDateTime_ts', addedDateTime,
				unix_timestamp(updateStamp) as 'updating_ts'
				from cache
				where typeID = {$this->p_typeID} and contentKey = '{$this->p_contentKey}' and dataID = {$this->p_dataID}");					
	}
	public function isExpired($minutes=10)
	{
		global $config;
		
		// Adding a noCache=1 to any query string will disable the cache for that page, that pageload only
		global $noCache;
		
		// Set the number of seconds to continue to serve up cached data during the update of a stale entry (prevents race condition)
		$staleContentTimer = 45;

		try
		{
			$this->p_ttl = $minutes;		
			
			if (($noCache && $config["cache_allow_override"]) || $config["cache_disable"]) throw new Exception();
				
			if (!count($this->p_data) || $this->p_data["content"] == '') throw new Exception();
		
			// Allow content that doesn't expire
			if (!$minutes) return false;
		
			// Current time
			$curTime = time();
		
			// Return unexpired if the current record is set to be updating! (done when the first isExpired fails for this record type)
			if ($this->p_data["updating_ts"] && $curTime < ($this->p_data["updating_ts"] + $staleContentTimer))	return false;
		
			// The time this content expires - the time the content was cached, plus the number of minutes specified
			$eTime = $this->p_data["addedDateTime_ts"] + (60*$minutes);		
		
			// If the time the content was stored (plus minute offset) is less than the current time, it has expired.
			if ($eTime < $curTime) throw new Exception();
		}
		catch(Exception $ex)
		{		
			/* The cached data is expired; it is now assumed that the function calling this will update the cached entry, so now we
			 * set the content-updating timestamp on the cached entry to help alleviate any potential race condition. This notifies the
			 * caching mechanism that the content is presently being updated, so it will continue to serve up cached data for the set period
		     * of time ($staleContentTimer) until the cache no longer expires when being checked.
		    */
						
			$d = new DAL();
			$d->qry("update IGNORE cache set updateStamp = now() where typeID={$this->p_typeID} and contentKey='{$this->p_contentKey}' and dataID={$this->p_dataID}");
				
			return true;
		}
	
		return false;
	}
	public function storeObject($obj, $force=false)
	{
		$content = serialize($obj);
		$this->save($content, $force);

		return $content;
	}
	public function save($content, $force=false)
	{
		global $config;

		/*
		 * Still save the cache when ?noCache=1 is specified in the query string, however do NOT if the cache is internally disabled via 
		 * config (located in includes/template.inc)
		*/
		if ($config["cache_disable"]) return;
		
		// Just in case the data has been cached again since this object was created. This is an alternative to deleting, then re-inserting.
		// Is this faster? Probably marginal, but it's better housekeeping IMO
		$this->refreshData();

		$d = new DAL(true);

		validation::makeSafe($content);

		if (!$content)
		{
			// Housekeeping
			$d->qry("delete from cache where typeID={$this->p_typeID} and contentKey='{$this->p_contentKey}' and dataID={$this->p_dataID}");
			
			return;
		}

		$cv["content"] = "'$content'";
		$cv["addedDateTime"] = "now()";
		$cv["updateStamp"] = "null";

		try
		{		
			// Returned value is an array, the first index being the number of rows matched (test to see if this record exists)
			$aff = $d->qryUpdateByArray('cache', $cv, "typeID = {$this->p_typeID} and contentKey = '{$this->p_contentKey}' and dataID = {$this->p_dataID}", false, true);
			
			
			if (!$aff[0]) 
			{
				// No rows were updated in the update call above, so this is a new cache record
								
				$cv["dataID"] = "'{$this->p_dataID}'";
				$cv["typeID"] = $this->p_typeID;
				$cv["contentKey"] = "'{$this->p_contentKey}'";
	
				// Stop the occasional async error associated with multiple read/writes to this particular key
				$d->suppressErrors = true;
				
				$d->qryInsertByArray('cache', $cv, "", true);
			}
		}
		catch(Exception $ex)
		{
			error_log("DB Caching error occurred. Key: {$this->p_contentKey}, typeID: {$this->p_typeID}, dataID: {$this->p_dataID}");
		}
	}
	public function loadObject()
	{
		$obj = unserialize($this->p_data["content"]);
	}
	public function clear()
	{
		$this->save("");
	}
}
class session_cache
{
	/*
	 * This class is designed to use a storage mechanism for caching within the current session. It is not recommended to store 
	 * large amounts of data using this mechanism, however it is very efficient for storing user interaction data, etc quickly with little overhead.
	 */
	
	private $p_key;
	private $p_data = array();
	
	public function __construct($cacheKey)
	{
		if (!$cacheKey) return false;
		
		$this->p_key = $cacheKey;
		$this->p_data = $_SESSION["proteus_cache"];
	}
	public function isExpired($mins=10)
	{
		$cacheVal = $this->p_data[$this->p_key];
		
		if (!$cacheVal) return true;
		if (!$cacheVal["timestamp"]) return true;
		
		$nowTS = time();
		$expTS = $cacheVal["timestamp"] + ($mins*60);
		
		if ($nowTS > $expTS) return true;
		
		return false;
	}
	public function storeObject($content)
	{		
		$this->p_data[$this->p_key] = array("timestamp"=>time(), "content"=>serialize($content));
		
		$_SESSION["proteus_cache"] = $this->p_data;		
	}
	public function loadObject()
	{
		return unserialize($this->p_data[$this->p_key]["content"]);
	}
	public function clear()
	{		
		unset($this->p_data[$this->p_key]);	
		
		$_SESSION["proteus_cache"] = $this->p_data;
	}
}
?>