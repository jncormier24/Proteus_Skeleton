<?php 
class customForms_core
{
	public $asset_folders = array("forms");			
	public $tables = array("customForms"=>"CREATE TABLE `customForms` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `title` varchar(255) NOT NULL DEFAULT '',
										 `description` text NOT NULL,
										 `responseText` text NOT NULL,										 
										 `lookupKey` varchar(50) NOT NULL DEFAULT '',
										 `inactive` tinyint(4) NOT NULL DEFAULT '0',
										 `customEmail` varchar(255) NOT NULL DEFAULT '',
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"customFormFields"=>"CREATE TABLE `customFormFields` (
										 `id` int(11) NOT NULL AUTO_INCREMENT,
										 `formID` int(11) NOT NULL DEFAULT '0',
										 `typeID` int(11) NOT NULL DEFAULT '0',
										 `fieldsetID` int(11) NOT NULL DEFAULT '0',
										 `verification` varchar(255) NOT NULL DEFAULT '',
										 `required` tinyint(4) NOT NULL DEFAULT '0',
										 `caption` varchar(255) NOT NULL DEFAULT '',
										 `summaryField` tinyint(4) NOT NULL DEFAULT '0',
										 `position` int(11) NOT NULL DEFAULT '0',
										 `extraData` text NOT NULL,
										 `exampleText` text NOT NULL,
										 PRIMARY KEY (`id`)
										) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"customFormFieldsets"=>"CREATE TABLE `customFormFieldsets` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `caption` varchar(501) NOT NULL,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"customFormResponses"=>"CREATE TABLE `customFormResponses` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `submissionID` int(11) NOT NULL,
											 `fieldID` int(11) NOT NULL,
											 `data` text NOT NULL,
											 `recordedDateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8",
							"customFormSubmissions"=>"CREATE TABLE `customFormSubmissions` (
											 `id` int(11) NOT NULL AUTO_INCREMENT,
											 `formID` int(11) NOT NULL,
											 `userID` int(11) NOT NULL,
											 `ipAddress` int(11) NOT NULL,
											 `dateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
											 PRIMARY KEY (`id`)
											) ENGINE=MyISAM DEFAULT CHARSET=utf8");
	
	public $queries = array("customForms"=>array("INSERT INTO `customForms` (`id`, `title`, `description`, `captcha`, `lookupKey`, `disabled`, `customEmail`) VALUES(1, 'Contact', '', 1, 'contact', 0, '')"),
							"customFormFields"=>array("INSERT INTO `customFormFields` (`id`, `formID`, `typeID`, `verification`, `required`, `caption`, `summaryField`, `position`, `extraData`, `exampleText`) VALUES(1, 1, 1, '0', 1, 'Full Name', 1, 1, '', ''),(2, 1, 1, '3', 1, 'Email address', 1, 2, '', ''),(3, 1, 1, '0', 1, 'Subject', 1, 3, '', ''),(4, 1, 2, '0', 1, 'Question', 0, 4, '', '')"));
	
	public static function getFormFieldTypes()
	{
		$fTypes["Text Box"] = 1;
		$fTypes["Text Area"] = 2;
		$fTypes["Drop Down List"] = 3;
		$fTypes["Check Box"] = 4;
		$fTypes["Display Label"] = 5;
		
		return $fTypes;
	}
	public static function getFormDataTypes()
	{
		$dTypes["Any Value"] = 0;
		$dTypes["Integer"] = 1;
		$dTypes["Phone Number"] = 2;	
		$dTypes["Email Address"] = 3;
		$dTypes["Custom"] = -1;
		
		return $dTypes;
	}
}
class customForm extends phpDataset 
{
	public function __construct($formID = 0)
	{
		validation::ensureInt($formID);
		parent::__construct("customForms", $formID, false);	
	}	
	public function __get($key)
	{
		switch($key)
		{
			case "assetFolder":
				return "assets/forms/{$this->id}";
				
			case "fields":
				return $this->getFields();
				
			case "submissions":
				return $this->getSubmissions();
		}
		
		return parent::__get($key);
	}
	public function getFields()
	{
		if (!$this->id) return;

		$d = new DAL(true);
		
		$fields = array();
		
		$qry = "select id
				from customFormFields
				where formID = $this->id
				order by position asc";
		$fQry = $d->qry($qry);
		
		while($field = $fQry->fetch_assoc())
		{
			$fields[] = new customFormField($field[id]);	
		}
		
		return $fields;
	}
	public function getSubmissions()
	{
		if (!$this->id) return;

		$d = new DAL(true);
		
		$submissions = array();
		
		$qry = "select *
				from customFormSubmissions
				where formID = $this->id
				order by dateTime desc";
		$sQry = $d->qry($qry);
		
		while($sub = $sQry->fetch_assoc())
		{
			$submissions[] = $sub;	
		}
		
		return $submissions;
	}
	public function insert($data="")
	{
		global $config;
		
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from customForms where inactive = 0");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		$id = parent::insert($data);
		
		$loc = $config["baseAppDir"].$this->assetFolder;
		if (!file_exists($loc)) mkdir($loc);		
		
		return $id;
	}
	public function delete()
	{
		global $config;
		
		$baseLoc = $config["baseAppDir"].$this->assetFolder;
		
		foreach(glob($baseLoc."/*") as $file)
		{
			@unlink($file);
		}
		
		@rmdir($baseLoc);		
		
		return parent::delete();
	}
	public function hasAccess(security $sec, $permission, $throwException = false)
	{
		$access = $sec->hasAccess(permissions::$manage_custom_forms, $permission > permissions::read ? permissions::write : $permission) || 
			   	  $sec->hasAccess(permissions::$modify_custom_form, $permission, $this->id);
		
		if (!$access && $throwException) $sec->security_error_a();
		
		return $access;
	}
	public function getFormObject($id = "customForm")
	{
		$frm = new phpForm("post", $id);
		$frm->class = "customForm";
		
		$fieldsets = array();
		
		foreach($this->fields as $field)
		{
			$ctl = $field->getWebControl();
			
			if ($field->fieldsetID)
			{
				if(!array_key_exists($field->fieldsetID, $fieldsets))
				{
					$fsData = $field->fieldset;
					$fieldsets[$field->fieldsetID] = $fsObj = new phpFormFieldset($fsData["caption"]);
				}
				else
				{
					$fsObj = $fieldsets[$field->fieldsetID];
				}
				
				$fsObj->addControl($ctl);
			}
			else
			{
				$frm->addControl($ctl);
			}
		}
		
		foreach($fieldsets as $fs)
		{
			$frm->addControl($fs);
		}
		
		$frm->addAttribute("data-formid", $this->id);
		
		$frm->addControl(new jqLoader("updateToken({$this->id})"));
		$frm->addControl(new hiddenInput("fields[token]"));
		$frm->addControl(new hiddenInput("formID", $this->id));
		
		return $frm;
	}	
}
class customFormField extends phpDataset
{	
	public function __construct($fieldID)
	{
		parent::__construct("customFormFields", $fieldID, $fieldID ? true: false);				
	}
	public function __get($key)
	{
		$d = new DAL(true);
		
		switch($key)
		{
			case "fieldset":			
				return $this->fieldsetID ? $d->qryArray("select * from customFormFieldsets where id = {$this->fieldsetID}") : false;
				 
			case "form":
				return new customForm($this->formID);				
		}		
		
		return parent::__get($key);		
	}
	public function validate($data)
	{
		//Required but not submitted - fails validation right away.
		if ($this->required && !strlen($data)) return false;
		
		//Nothing entered, and not required - pass validation.
		if (!$this->required && !strlen($data)) return true;
		
		//Use the custom validation routine
		switch($this->typeID)
		{
			case 1:
				switch($this->verification)
				{
					case -1:
						return preg_match($this->extraData, $data);
					case 0:
						$sLen = strlen($data);																			
						return (!$this->extraData || ($this->extraData && $sLen >= $this->extraData[0] && (!$this->extraData[1] || $sLen <= $this->extraData[1])));
					case 1:
						$sLen = strlen($data);
						return is_numeric($data) && (!$this->extraData || ($this->extraData && $sLen >= $this->extraData[0] && (!$this->extraData[1] || $sLen <= $this->extraData[1])));
					case 2:						
						return preg_match("/^(\(?[0-9]{3}\)?)?(\s|\.|\-)?[0-9]{3}(\-|\.|\s)?[0-9]{4}$/is", $data);
					case 3:
						return validation::isValidEmail($data);	
				}
				break;				
		}
		
		return true;
	}
	public function getWebControl()
	{
		switch($this->typeID)
		{			
			case 1:
				$obj = new customText("", "", $this->extraData[0], $this->extraData[1]);				
				if ($this->required) $obj->setRequired(null, "required");
				
				switch($this->verification)
				{
					case 1:
						$tmp = explode(",", $this->extraData);
						if (count($tmp) == 2)
						{
							for ($i = 0; $i < $tmp[0]; $i++)
							{
								$mask .= "9";
							}			
							
							$mask .= "?";
							
							for ($i = 0; $i < $tmp[1]; $i++)
							{
								$mask .= "9";
							}
							
							jqMaskedInputModule::setMask($obj, $mask);
						}
					case 2: 
						jqMaskedInputModule::setMask($obj, jqMaskedInputModule::mask_phone);
						break;
				}
				
				break;
			case 2:
				$obj = new phpTextArea("", "", 4, 40);				
				if ($this->required) $obj->setRequired(null, "required");
				
				break;
			case 3:
				$obj = new phpDDL("");				
				
				foreach($this->extraData as $itm)
				{
					$obj->addListItem(new phpListItem($itm, $itm));
				}				
				
				break;
			case 4:
				$obj = new phpCheckbox("");
				
				break;				
			case 5:
				return new phpSpan($this->caption, "label");
		}
		
		$ex = ($this->exampleText ? "<span class='example'>".validation::prepForDisplay($this->exampleText)."</span>" : "");
		
		if ($this->typeID == 4)
		{
			$obj->setLabel(validation::prepForDisplay($this->caption));
			$obj->setDescription($ex);
		}
		else
		{
			$obj->setLabel(validation::prepForDisplay($this->caption).$ex);
		}
		
		$obj->id = "fields[{$this->id}]";
		$obj->addAttribute('data-fieldID', $this->id);
		$obj->addAttribute('data-verification', $this->verification);
		
		return $obj;
	}
	public function insert($data="")
	{
		global $config;
		
		$d = new DAL(true);
		
		// Take care of setting the position when inserting here
		$max = $d->qryCount("select max(position) from customFormFields where formID = {$this->formID}");
		if (!$max) $max = 0;
		$max++;
			
		$this->position = $max;
		
		$id = parent::insert($data);		
		
		return $id;
	}
	public function delete()
	{
		$d = new DAL(true);
		$d->qry("delete from customFormResponses where fieldID={$this->id}");
		
		parent::delete();
		
		// Fieldset cleanup
		$d->qry("delete from customFormFieldsets where not exists(select id from customFormFields where fieldsetID = customFormFieldsets.id)");
		
		tableSorter_core::realignSorts(tableSorter_core::$sort_form_field, array("formID={$this->formID}"));
	}
}
class customFormSubmission extends phpDataset
{
	private $p_responses;
	
	public function __construct($subID, $throwOnEmpty = true)
	{
		parent::__construct("customFormSubmissions", $subID, $throwOnEmpty);
	}
	public function __get($key)
	{
		switch($key)
		{
			case "form":
				return new customForm($this->formID);
				
			case "responses":
				if (!$this->p_responses) $this->fillResponses();
				return $this->p_responses;					
		}
		
		return parent::__get($key);
	}
	private function fillResponses()
	{
		if (!$this->id) return;
		
		$d = new DAL(true);
	
		$qry = "select customFormResponses.data, customFormFields.caption, customFormFields.typeID
				from customFormResponses inner join customFormFields on customFormFields.id = customFormResponses.fieldID
				where customFormResponses.submissionID = {$this->id}
				order by customFormFields.position asc";
		$fQry = $d->qry($qry);
	
		while($resp = $fQry->fetch_assoc())
		{
			$this->p_responses[] = $resp;
		}
	}
	public function addResponse($fieldID, $value)
	{
		if (!$this->id) return;
		
		$d = new DAL(true);
		
		$cv["submissionID"] = $this->id;
		$cv["fieldID"] = $fieldID;
		$cv["data"] = "'$value'";
		
		return $d->qryInsertByArray("customFormResponses", $cv);
	}
	public function delete()
	{
		if (!$this->id) return;
		
		$d = new DAL(true);
		
		// Delete the individual responses before deleting the submission itself
		$d->qry("delete from customFormResponses where submissionID={$this->id}");
		
		parent::delete();		
	}
}
class customFormsModule extends pageModule
{
	public function __construct()
	{						
		$this->addRequisiteModule(new jqMaskedInputModule());		
		$this->addScriptObject(new javascriptControl("", "proteus/modules/customForms/scripts/view.js"));		
	}	
}
?>