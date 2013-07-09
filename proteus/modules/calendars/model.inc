<?php
class customCalendar_core
{
	public $files = array("calendar.php", "calendar.ajax");	
	public $tables = array("calendarEntry"=>"CREATE TABLE `calendarEntry` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `calendarID` int(11) NOT NULL,
											 `title` varchar(255) NOT NULL,
											 `description` text NOT NULL,
											 `startDate` date NOT NULL,
											 `startTime` varchar(25) NOT NULL,
											 `endTime` varchar(25) NOT NULL,
											 `recurring` varchar(4) NOT NULL,
											 `recurringWOM` varchar(2) NOT NULL,
											 `lookupKey` varchar(25) NOT NULL,
											 `addedDateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
											 `inactive` tinyint(4) NOT NULL,
											 PRIMARY KEY (`id`),
											 KEY `calendarID` (`calendarID`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"calendars"=>"CREATE TABLE `calendars` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `typeID` tinyint(4) NOT NULL,
										 `dataID` int(11) NOT NULL,
										 `title` varchar(25) NOT NULL,
										 `position` int(11) NOT NULL,
										 `lookupKey` varchar(25) NOT NULL,
										 `addedDateTime` datetime NOT NULL,
										 `inactive` tinyint(4) DEFAULT NULL,
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8");
}
class customCalendar extends customConfig 
{
	const calendar_standalone = 0;
	const calendar_municipal_entity = 1;
	
	private $p_caldata;
	private $p_calID;	
	
	public $entries;
	
	public function __construct($calID, $fillEntries = true)
	{
		validation::ensureInt($calID);
		if (!$calID) throw new Exception("No Calendar provided!");		
		
		//Fill the basic data for the parent calendar object.
		$d = new DAL(true);
		
		$qry = "select *
				from calendars
				where id=$calID";
				
		$this->p_caldata = $d->qryArray($qry);
		if (!$this->id) throw new Exception("Error, that Calendar cannot be found!");
		
		// Set the default icon size for each entry to 30x30 - this can be changed in Proteus backend, this is only used if
		// this value is not defined in the config manually. See the 'loadDefaults' method in the 'customConfig' class above.
		$this->p_defaults[iconSize] = "30x30";		
		
		//Get the configuration for this object - 7 is for type 'Calendar' in Proteus site type definitions
		parent::__construct(7, $calID);
		
		//Get all the entries for this Calendar (set fillEntries to false to filter this list by hand instead of getting all of them)
		if ($fillEntries) $this->fillEntries();
	}
	private function fillEntries($includeInactive = false)
	{	
		$this->entries = array();
			
		//Don't include inactive entries by default
		if (!$this->id) throw new Exception("Error, this calendar cannot be found!");
		
		$cCache = new cache("calendarEntries_".$this->id);

		//Need ALL the calendar entries (for recurrence...) to filter properly
		if ($includeInactive || $cCache->isExpired(10))
		{		
			$d = new DAL(true);
			
			$iWh = (!$includeInactive ? "and inactive=0" : ""); 
			
			$qry = "select id
					from calendarEntry
					where calendarID = $this->id $iWh
					order by startDate asc";
			$eQry = $d->qry($qry);
			
			while($entry = $eQry->fetch_assoc())
			{
				$eObj = new customCalendarEntry($entry[id]);
				$this->entries[] = $eObj;	
			}
			
			$cCache->storeObject($this->entries);
		}
		else
		{
			$cCache->loadObject($this->entries);	
		}
	}
	public function filterEntries($startDate, $behindStamp = 0, $aheadStamp = 0)
	{
		//Function returns a filtered entry array with recurrence
		
		//Start Date can be a timestamp OR a mySQL date/time		
		$startStamp = (is_long($startDate) ? $startDate : validation::convertToUnixTimestamp($startDate));
		
		//Set the ahead / behind stamps to the initial start date if not specified (So the loop still works)
		if (!$aheadStamp) $aheadStamp = $startStamp;
		if (!$behindStamp) $behindStamp = $startStamp;
		
		//Zero out any hour values so it's just a date stamp (i.e., midnight on the day)
		$startStamp = mktime(0,0,0,date("m", $startStamp),date("d", $startStamp),date("Y", $startStamp));
		$aheadStamp = mktime(0,0,0,date("m", $aheadStamp),date("d", $aheadStamp),date("Y", $aheadStamp));
		$behindtamp = mktime(0,0,0,date("m", $behindStamp),date("d", $behindStamp),date("Y", $behindStamp));
		
		//Quick filter the items to remove ones without recurrence, less than our start, or greater than our end (narrows down the list for performance!)
		foreach((array)$this->entries as $entry)
		{			
			if (!$entry->recurring && ($entry->startStamp < $behindStamp || $entry->startStamp > $aheadStamp))
			{
				//Don't include it...				
				continue;				
			}			
			$eArray[] = $entry;		
		}
		
		//Set the comparison to the furthest in the past to start (potentially the same timestamp...)
		$compStamp = $behindStamp;

		while($compStamp >= $behindStamp && $compStamp <= $aheadStamp)
		{
			/* @var $entry customCalendarEntry */					
			foreach((array)$eArray as $entry)
			{	
				if ($entry->startStamp == $compStamp || $entry->checkRecurrence($compStamp))
				{
					//Have to clone; Objects are passed implicitely by reference in a for loop whether the & is used or not!!
					$tmp = clone $entry;
					
					if ($entry->recurring)
					{
						//Set the date for recurring items (otherwise it shows the same date!)
						$tmp->startStamp = $compStamp;
						$tmp->startDate = validation::convertToMySqlDate($compStamp);						
					}
					
					//If it matches, add it to our filtered array
					$tmpArr[] = $tmp;
				}
			}
			
			//Add days until we've surpassed the aheadStamp - adding the time sets it to zero-hour
			$compStamp = strtotime("+1 day 00:00:00", $compStamp);			
		}
		
		//Return an array of filtered entries
		return $tmpArr;
	}	
	public function __get($key)
	{
		return $this->p_caldata[$key] ? $this->p_caldata[$key] : parent::__get($key);
	}	
	public static function getCalendars($typeID = 0, $dataID = 0)
	{		
		if (!pageClass::checkSiteType(proteus_core::module_calendars)) throw new Exception("Error, this site is not configured to use the Proteus Calendar Module. Please enable the module and try again.");

		validation::ensureInt($typeID);
		validation::ensureInt($dataID);
		
		$d = new DAL(true);
		$arr = array();
		
		$qry = "select id
				from calendars
				where typeID=$typeID and dataID=$dataID
				order by title";
		$iQry = $d->qry($qry);

		while($cal = $iQry->fetch_assoc())
		{
			$arr[$cal[id]] = new customCalendar($cal[id]);
		}
		
		return $arr;
	}
}
class customCalendarEntry
{
	protected $p_data;
	public $files;
	
			
	// 0 - weekly
	// 1 - monthly, same date
	// 2 - monthly, dow
	
	public $recurrenceType;
	
	public function __construct($entryID, $p_data = NULL)
	{
		global $config;
		if($p_data && is_array($p_data))
		{
			$this->p_data = $p_data;
		}
		else 
		{
			validation::ensureInt($entryID);
			if (!$entryID) throw new Exception("Error, no entry provided!");

			$d = new DAL(true);
		
			$qry = "select *
					from calendarEntry
					where id=$entryID";
			$this->p_data = $d->qryArray($qry);
		}
		//Do this here and be done with it! (has uses later on)
		$this->p_data[startStamp] = validation::convertToUnixTimestamp($this->p_data[startDate]);
		
		//Enumerate the files
		$baseLoc = "images/calendars/$this->calendarID/$entryID";
		$filesLoc = $config[baseAppDir].$baseLoc;
		
		if (!file_exists($filesLoc))
		{		
			//recursively create (thank you PHP5!)
			// HAVE TO USE THE @ SYMBOL HERE!! Failure to do so will cause mkdir to fail SILENTLY and never throw any errors. Definitely a PHP BUG!!
			if (!@mkdir($filesLoc, 0770, true)) throw new Exception("Error creating entry storage location (this is likely a permissions problem!)");			
		}		
		
		$dir = dir($filesLoc);
		
		while(($entry = $dir->read()) !== false)
		{
			if ($entry == "." || $entry == ".." || $entry == "icon.png" || preg_match("/^support_/", $entry)) continue;
				
			$file[name] = $entry;
			$file[size] = @filesize($filesLoc.'/'.$entry);
			$file[url] = $baseLoc."/$entry";
			
			$this->files[] = $file;
		}
		
		$dir->close();
		
		if (file_exists("$filesLoc/icon.png"))
		{
			$this->p_data[icon] = $config[scripturl]."$baseLoc/icon.png";	
		}
		
		$rec = explode("::", $this->recurring);
		$this->recurrenceType = intval( $rec[0] );
	}
	public function __get($key)
	{			
		return $this->p_data[$key];	
	}
	public static function getCalendarByEntryID($entryID, $fillEntries = false)
	{
		$eObj = new customCalendarEntry($entryID);
		return new customCalendar($eObj->calendarID, $fillEntries);	
	}
	public function getCalendarObject($fillEntries = false)
	{
		return new customCalendar($this->calendarID, $fillEntries);	
	}
	public static function  getWeekOfMonth($timestamp, $date = '')
    {
    	$rollover = "sunday";
    	$daylen = 86400;
    	if($date)
    	{
       	 	$cut = substr($date, 0, 8);
        	$timestamp = strtotime($date);
    	}
    	
        $first = strtotime($cut . "00");
        $elapsed = ($timestamp - $first) / $daylen;

        $i = 1;
        $weeks = 1;

        for($i; $i<=$elapsed; $i++)
        {
            $dayfind = $cut . (strlen($i) < 2 ? '0' . $i : $i);
            $daytimestamp = strtotime($dayfind);

            $day = strtolower(date("l", $daytimestamp));

            if($day == strtolower($rollover))  $weeks ++;
        }

        return $weeks;
    }
	public static function numToOrdinalWord($num)
	{
			$first_word = array('eth','First','Second','Third','Fouth','Fifth','Sixth','Seventh','Eighth','Ninth','Tenth','Elevents','Twelfth','Thirteenth','Fourteenth','Fifteenth','Sixteenth','Seventeenth','Eighteenth','Nineteenth','Twentieth');
			$second_word = array('','','Twenty','Thirty','Forty','Fifty');

			if($num <= 20)
				return $first_word[$num];

			$first_num = substr($num,-1,1);
			$second_num = substr($num,-2,1);

			return $string = str_replace('y-eth','ieth',$second_word[$second_num].'-'.$first_word[$first_num]);
	}
	public function getOrdinal()
	{
		$rec = explode("::", $this->recurring);
  		$entrDate = explode("-", $this->startDate);
		$entryjDayFirst = gregoriantojd($entrDate[1], 1, $entrDate[0]);
		$entryStartW = jddayofweek($entryjDayFirst);
		$entryWOM = customCalendarEntry::getWeekOfMonth('', $this->startDate);
		$dow = intval($rec[1]);//sunday is 0 by default
			
		$sub = $entryWOM > 1 ? ($entryStartW > $dow ? 1 : 0) : 0; //is this day of the week on the first week (ignore if the entry date is the first week)
		$ordinal = $entryWOM - $sub; //the # of the occurence of this day of the week in the month
		return $ordinal;
			
	}
  	protected function checkOrdinalDOW($compStamp)
  	{
  		$rec = explode("::", $this->recurring);
  		$dow = intval($rec[1]);//sunday is 0 by default
  		$ordinal = $this->recurringWOM;//$this->getOrdinal();

		//now find this # occurence in the month we are working with
		$month = date("m", $compStamp);
		$year = date("Y", $compStamp);
  	  	if($ordinal == "L") //last occurence of this day in the month
  		{
  			$is_last = (date("d", $compStamp) + 7) > cal_days_in_month(CAL_GREGORIAN, $month, $year);
  			if ( !$is_last ) return false;
  			$compJD = jddayofweek(gregoriantojd($month, date("d", $compStamp), $year));
  			return ($compJD == $dow);
  		}
  		$jDayFirst = gregoriantojd($month, 1, $year);
		$startW = jddayofweek($jDayFirst);
		$flip = 7 - ($dow);
		$dom = 1;
		if( $startW > $dow )
		{
			$dom = ($ordinal * 7) - $flip + 1 + (7 - $startW);
		}
		else 
		{
			$dom = ($ordinal * 7) - $flip - $startW + 1;
		} 
			 
		return intval($dom) == intval(date("j", $compStamp));
  	}
	public function checkRecurrence($compDate)
	{
		if (!$this->recurring) return false;
		
		$compStamp = is_long($compDate) ? $compDate : validation::convertToUnixTimestamp($compDate);
		$entryStamp = $this->startStamp;
		
		$match = false;
		//Evaluate the current date, then recurse if necessary until a match is found
		if ($this->recurrenceType == 0)
		{
			//If the DOW is the same
			$match = date("w", $compDate) == date("w", $entryStamp);
		}
		else if ($this->recurrenceType == 1)
		{
			//If the day of the month is the same
			$match = date("j", $compStamp) == date("j", $entryStamp);	
		}	
		else if ($this->recurrenceType == 2)
		{
			
			$match = $this->checkOrdinalDOW($compDate);
			
		}		
		
		return $match;
		
	}
	public function getOccurenceDescription($basic = false)
	{
		if ($this->recurring)
		{
			switch($this->recurrenceType)
			{
				case 0:
					$par = $basic ? "Recurring weekly on the same day." : "Every ".date("l", $this->startStamp);
					break;
				case 1:
					$par = $basic ? "Recurring monthly on the same numeric day" : date("dS", $this->startStamp)." of each month";
					break;
				case 2:
					if($basic)
					{
						$par = "Monthly, on the same week of the month & day of the week";
					}
					else 
					{
						if($this->recurringWOM == "L")
						{
							$rec = "last ". date("l", $this->startStamp) . " of the month.";	
						}
						else 
						{
							$rec = customCalendarEntry::numToOrdinalWord(intval($this->recurringWOM)). " ". date("l", $this->startStamp). " of the month.";
						}
						$par = "Recurring monthly on the ".$rec;
					}
					
					
					break;
			}
		}
		else
		{
			$par = validation::parseMySqlDate($this->startDate, "D, M d Y");
		}

		return $par;
	}
	public function getEntryLink()
	{	 
		$lnk = new phpHyperlink("#", validation::prepForDisplay($this->title, false), "viewLink");
		$lnk->onclick = "$('#entryWin{$this->startStamp}_$this->id').dialog('open'); return false;";
			
		$eWin = new jqUI_dialog("entryWin{$this->startStamp}_$this->id", false);
		$eWin->style = "display: none;";
		$eWin->addClass('entryWindow');
		$eWin->setOption("width", 550);
		$eWin->setOption("height", 400);
		$eWin->setOption("title", "'".validation::prepForDisplay($this->title, false)."'");
		$eWin->addButton("Close","function() { $(this).dialog('close'); }");
			
		//$par = new phpParagraph(validation::prepForDisplay($this->description));
		$par = new phpParagraph($this->description);
		$par->breakAfter = true;
			
		$eWin->addControl(new phpHeader(3, "Description:"));
		$eWin->addControl($par);			
			
		$eWin->addControl(new phpHeader(3, "Date:"));
		
		$par = new phpParagraph($this->getOccurenceDescription() );
		
		$eWin->addControl($par);	
			
		$eWin->addControl(new phpHeader(3, "Time:"));
		$endStr = $this->endTime ? "to $this->endTime" : "";
		$eWin->addControl(new phpParagraph("$this->startTime $endStr"));
			
		if (count($this->files))
		{
			$eWin->addControl(new phpSpan("Related Downloads:", "downloadableFiles"));
			
			foreach($this->files as $f)
			{
				$eWin->addControl(new phpHyperlink($f[url],$f[name],'downloadableFile'));	
			}	
		}
		$pCal = $this->getCalendarObject(false);
		if($pCal->typeID == 1) //municipal calendar
		{
			$ftrClass = "pmm_entity";
			$eWin->addControl(new phpHeader(3, "Posted by:"));
			$pEntity = new $ftrClass($pCal->dataID);
			$eWin->addControl(new phpParagraph($pEntity->title)); //show which municipal entity posted this
			
		}

		$lnk->postContent = $eWin->render();

		return $lnk;
	}
}
?>