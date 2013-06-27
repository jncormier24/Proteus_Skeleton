<?php
class contentParser
{
	private $pParam = array();
	public $templateContent;
	
	public function __construct($templateContent)
	{
		$this->templateContent = $templateContent;	
	}
	public function pushParam($key, $value)
	{
		$this->pParam[$key] = $value;
	}
	public function __get($key)
	{
		return $this->pParam[$key];	
	}
	public function render($verbose=true)
	{		
		$str = $this->templateContent;
		
		if (empty($str))
		{
			if ($verbose)
			{
				die("Template Error!");
			}
			else
			{
				//No template file found; Loop through the values and add them as literal controls (in order added)
				$pc = new pageContent();
				
				foreach($this->pParam as $key=>$value)
				{
					$pc->addControl(new phpLiteral($value));
				}
				
				return $pc->render();	
			}
		}				
		
		foreach($this->pParam as $key=>$value)
		{			
			//Replace all the TemplateTags first...			
			$str = preg_replace("/{(%%)?".$key."(%%)?}/",$value,$str);			
		}	
		
		//Replace optional parameters with blank if they have not already been replaced
		$str = preg_replace("/{(%%){1}.*?(%%){1}}/","",$str);
		
		return $str;
	}	
}
class fileParser extends contentParser
{
	public $pFile;
	public $absolute = false;
	
	function __construct($file, $absolute=false)
	{
		global $config;
		
		$this->pFile = $file;
		$this->absolute = $absolute;
		$this->pushParam("scripturl", $config[scripturl]);		
		
		/* foreach($conf as $key=>$val)
		{				
			//Don't store any config params that are arrays! (very bad!!)
			if (!is_array($val) && $key != "dbLink")
			{
				$this->pushParam($key, $val);				
			}
		} */
		//parent::__construct(@file_get_contents($filename));
	}
	function render()
	{
		global $config;
		
		if ($this->absolute)
		{
			$filename = $this->pFile;	
		}
		else
		{
			//$filename = $config[baseAppDir].($config[adminTheme] ? "admin/" : "").$config[theme]."/".$this->pFile;
			$filename = $config["baseAppDir"]."themes/".$config["theme"]."/".$this->pFile;						
		}
				
		$this->templateContent = @file_get_contents($filename);
		
		return parent::render();
	}
}
?>