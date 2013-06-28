<?php
class cache
{
	var $p_file;
	var $p_fpath;
	var $errMessage;
	var $errNo;
	var $neverExpires;
	
	function cache($file, $neverExpires=false)
	{
		global $config;		
		
		$this->p_file = $file.".cache";	
		$this->p_fpath = $config[baseAppDir].'cache/'.$this->p_file;
		
		$this->neverExpires = $neverExpires;
	}	
	function storeObject($obj)
	{
		$strVal = serialize($obj);
		return $this->store($strVal);
	}	
	function store($strVal)
	{
		$fPointer = @fopen($this->p_fpath, "w");		
		if (!$fPointer) return 0;

		fwrite($fPointer, $strVal, strlen($strVal));
		fclose($fPointer);
		
		return 1;
	}
	function isExpired($mins=10)
	{
		global $noCache;
		if ($noCache) return true;
		
		//Doesn't exist, the file has obviously expired!
		if (!file_exists($this->p_fpath)) return true;		
		
		//File exists, and doesn't expire - return false.
		if ($this->neverExpires) return false;
				
		$fileTime = filectime($this->p_fpath);
		
		$acceptableTime = strtotime("+$mins minutes",$fileTime);				
		$cTime = time();
	
		if ($acceptableTime >= $cTime)
		{
			//Expire time is greater than the current time - file has not yet expired
			return false;
		}
		else
		{
			//File has expired, time to refresh.			
			return true;
		}
	}
	function loadObject(&$object)
	{		
		if ($this->getCachedData($strData))
		{			
			$object = unserialize($strData);
			return 1;
		}
		else
		{
			return $this->errNo;	
		}
	}
	function getCachedData(&$retVal)
	{
		if (!file_exists($this->p_fpath))
		{			
			$this->errMessage = "Cached file does not exist!";
			$this->errNo = -1;
			
			return $this->errNo;
		}
		
		$retVal = file_get_contents($this->p_fpath);
		
		return 1;
	}
	function clear()
	{		
		return @unlink($this->p_fpath);
	}
	function runCleanup($path)
	{		
	}	
}
class session_cache extends cache
{	
	function session_cache($file)
	{
		if (!session_id()) return;
		
		$this->runCleanup();
		
		global $config;
		
		$dir = session_id();		
		$path = $config[baseAppDir].'cache/session_cache/'.$dir;
		
		if (!file_exists($path)) @mkdir($path);
		
		$this->p_file = $file.".cache";	
		$this->p_fpath = $path.'/'.$this->p_file;				
	}	
	function runCleanup()
	{
		global $config;
		
		//Cache the cleanup - once every hour ;)
		$cObj = new cache('cleanup');
		if (!$cObj->isExpired(60)) return;
		
		$path = $config[baseAppDir].'cache/session_cache/';
	
		$dirs = glob($path."/*");
		//Get around PHP5's warning
		$dirs = is_array($dirs) ? $dirs : array();
		
		foreach($dirs as $cDir)
		{
			$fileTime = @filectime($cDir);
			
			$acceptableTime = strtotime("+1 minutes",$fileTime);				
			$cTime = time();
		
			if ($acceptableTime < $cTime)
			{
				$files = glob($cDir.'/*.*');
				$files = is_array($files) ? $files : array();
				
				foreach($files as $cFile)
				{				
					@unlink($cFile);			
				}
				
				@rmdir($cDir);
			}
		}
		
		$cObj->storeObject(date("m-d-Y h:i:s"));		
	}	
}
class permission_cache extends cache
{
	function permission_cache($acctID)
	{		
		global $config;
		
		$path = $config[scriptLocation].'/includes/cache/permission_cache';		
		
		$this->p_file = 'permissions_'.$acctID.".cache";
		$this->p_fpath = $path.'/'.$this->p_file;				
	}	
}
?>