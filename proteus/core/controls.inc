<?PHP
class pageContent
{
	//Simple control to wrap objects
	public $controls = array();

	public function addControl($control)
	{
		array_push($this->controls, $control);
	}
	public function insertControl($control)
	{
		array_unshift($this->controls, $control);
	}
	function insertControlAt($control, $index)
	{
		$firstHalf = array_slice($this->controls, 0, $index);
		$secondHalf = array_slice($this->controls, $index);
	
		array_unshift($secondHalf, $control);
		$this->controls = array_merge($firstHalf, $secondHalf);
	}
	function render()
	{
		global $config;
		$content = "";
		
		foreach($this->controls as $control)
		{
			if ($config["debugMode"] && !is_callable(array($control, "render"))) throw new Exception("Invalid Control render attempt:".print_r($control, true));
			$content .= $control->render();
		}
		
		return $content;
	}
}
class webControl extends pageContent
{
	private $attributes = array();
	private $parameters = array();
	
	public $controlTag;
	public $controlData;
	public $controlCloseTag;	
	
	public $renderedControls;
	
	public $classes = array();
	
	//Render Order: 1=Controls first 2=Content first
	public $renderOrder=2;	
	
	public $breakAfter=false;
	
	public $preContent = "";
	public $postContent = "";
	
	function addAttribute($key, $value)
	{
		$this->$key = $value;	
	}
	function addClass($class)
	{
		$this->classes[$class] = $class;
	}
	public function __set($key, $value)
	{			
		if ($key == "id" && get_class($this) != "phpHeader") $this->name = $value;
		
		if ($key == "class")
		{
			if ($value)
			{
				$this->classes = array($value=>$value);				
			}
			else
			{
				$this->classes = array();
			}
			
			return;
		}
				
		$this->attributes[$key] = $value;
	}
	public function __get($key)
	{
		return $this->attributes[$key];	
	}
	public function addParameter($value)
	{
		$this->parameters[$value] = $value;
	}
	function render()
	{	
		$controls = parent::render();
		
		$strControl = $this->getOpenTag().($this->renderOrder == 1 ? $controls.$this->controlData : $this->controlData.$controls);
		
		if ($this->controlCloseTag == true) $strControl .= $this->getCloseTag();		
		
		return $this->preContent.$strControl.$this->postContent.($this->breakAfter ? "<br style='clear:both;' />" : "");	
	}
	function getOpenTag()
	{		
		return "<".strtolower($this->controlTag)." ".$this->renderAttributes()." ".(!$this->controlCloseTag ? " /" : "").">";
	}
	function getCloseTag()
	{
 		$strControl .= "</".strtolower($this->controlTag).">";
		return $strControl;
	}
	private function renderAttributes()
	{
		$attArr = array();		
		
		if (count($this->classes)) $attArr[] = "class=\"".implode(" ", $this->classes)."\"";
		
		//If there are no attributes, don't bother
		foreach($this->attributes as $key=>$attr)
		{
			$evt = strtolower($key);
			$attArr[] = "$evt=\"$attr\"";
		}
		
		foreach($this->parameters as $value)
		{
			$attArr[] = "$value=\"$value\"";	
		}	
		
		return implode(" ", $attArr);
	}
	function __clone() 
	{
        foreach ($this as $key => $val) 
        {
            if (is_object($val) || (is_array($val))) 
            {
                $this->{$key} = unserialize(serialize($val));
            }
        }
    }
    function loadContent($contentKey, $defaultType = 1)
    {    	
    	$iObj = new inlineContent($contentKey);
    	
    	if (!$iObj->id && !$this->controlData)
    	{
			$this->controlData = "[b]No content configured for `$contentKey`[/b]";						 
    	}
    	
    	if($iObj->id)
    	{	
    		$this->controlData = $iObj->formatted_content;    		   		
    	}
    	else
    	{
    		$d = new DAL(true);
    		
    		$defaultContent = $this->controlData;
    		validation::makeSafe($defaultContent);
    		
    		//Content is not configured - setup the initial record
    		$cv[contentKey] = "'$contentKey'";
    		$cv[typeID] = $defaultType;
    		$cv[content] = "'$defaultContent'";
    		
    		$d->qryInsertByArray("inlineContent", $cv);
    		
    		$this->controlData = validation::prepForDisplay($this->controlData);
    	}
    	
    	$this->addClass('inlineContent');
    	$this->addAttribute("data-contentkey", $contentKey);
    }    
}
class stylesheetControl extends webControl
{
	public function __construct($file)
	{
		$this->controlTag = "link";
		$this->controlCloseTag = false;
		$this->rel = "stylesheet";
		$this->href = $file;
		
		$this->postContent = "\n";
	}
}
class javascriptControl extends webControl
{
	public function __construct($script="", $src="")
	{
		$this->controlTag = "script";
		$this->addAttribute("type","text/javascript");
		$this->controlCloseTag = true;
		$this->controlData = $script;

		if ($src)
		{
			global $config;
			if ($config[jsRev] && !preg_match("/^http/i", $src)) $src .= "?rev=$config[jsRev]";
				
			$this->src = $src;
		}
		
		$this->postContent = "\n";
	}
}
/*     Proteus        */
class proteus_footer extends pageContent
{
	function proteus_footer()
	{
		global $config;
		
		$renderTime = round(pageClass::getMicroTime() - START_TIME, 4);
		
		$fBlock = new panelBlock();
		$fBlock->addControl(new phpLiteral($config[businessName]." - Powered by: Proteus Small Business CMS".($renderTime ? "<br />Page rendered in $renderTime(s)" : "")));
		
		$this->addControl($fBlock);
	}		
}
class proteus_header extends pageContent
{
	function proteus_header()
	{
		global $config, $g_sec;
		
		$img = new image($config[theme]."/images/logo_medium.gif");
		$img->class = "logo";
		
		$top = new phpHeader(1, $config[businessName]);
		$top->style = "text-align: left;";
		
		$mid = new phpSpan("Powered By: Proteus Small Business CMS",'hdrMid');		
		$mid->style = "text-align: left;";
		
		$bot = new phpFiller();
		
		if ($g_sec->id || $g_sec->name)
		{			
			$bot = new phpSpan("Logged in as: ".($g_sec->login ? $g_sec->login : "<em class='global'>{$g_sec->name}</em>"), 'hdrBottom');			
		}
		
		$hBlock = new panelBlock();		
		$hBlock->id = 'mainHeaderBlock';		
			
		$hBlock->addControl($img);		
		$hBlock->addControl($top);
		$hBlock->addControl($mid);
		$hBlock->addControl($bot);		
		
		$this->addControl($hBlock);
	}	
}
/************************/

class image extends webControl
{	
	function image($url, $alt="", $class="")
	{		
		$this->src = $url;
		
		$this->controlTag = "img";		
		
		//Always define an alt tag, even its empty!
		$this->setAltTag($alt ? $alt : " ");
		
		if ($class) $this->class = $class;
	}
	function setAltTag($text)
	{
		$this->addAttribute("alt", $text);
		if (trim($text)) $this->addAttribute("title", $text);
	}	
}
class resizedImage extends image 
{	
	function resizedImage($file, $width='', $height='')
	{
		global $config;		
		
		if (!$width && !$height)
		{
			//Don't bother if there are no resizeable dimensions to specify!
			parent::image($file);
			return;	
		}
		
		$tmp = split("/", $file);
		
		//Only support 3 dirs...
		$fileName = $width."_".$tmp[2];
		$subDir = $tmp[1]."/";
		$baseDir = $tmp[0]."/";
		
		$oFile = $config[baseAppDir].$file;
		
		if (!file_exists($oFile)) 
		{
			//Don't process if the local file cannot be found!
			parent::image($file);
			return;	
		}
		
		if (!eregi("^.*\.(jpeg|jpg|gif)$", $fileName, $regs))
		{
			// This should never happen!!
			error("Invalid image file: $file. Please contact Proteus Support");
		}		
		
		$tDir = $config[baseAppDir].'images/'.$subDir.'thumbnails';
		
		if (!file_exists($tDir))
		{
			//Create the thumbnail folder if it doesn't exist
			if (!mkdir($tDir)) die('could not create resized storage directory '.$tDir);			
		}	
		
		$tFile = $tDir.'/'.$fileName;
		
		if (!file_exists($tFile))
		{
			switch(strtolower($regs[1]))
			{
				case "jpg":
				case "jpeg":
					$img = imagecreatefromjpeg($oFile);
					break;
				case "gif":	
					$img = imagecreatefromgif($oFile);
					break;
			}
			
			if (!$img) die("could not create image resource (thumbnail) for $file");
			
			list($width_orig, $height_orig) = getimagesize($oFile);
			
			$ratio_orig = $width_orig/$height_orig;						
			
			//Only update the width / height if there isn't one (keep aspect ratio). One or the other MUST be defined.
			
			if ($width && $width_orig < $width) $width = $width_orig;
			if ($height && $height_orig < $height) $height = $height_orig;
			
			if (!$width)
			{
				$width = $height * $ratio_orig;	
			}
			
			if (!$height)
			{	
				$height = $width / $ratio_orig;
			}
			
			$newImg = imagecreatetruecolor($width, $height);
			
			imagecopyresampled($newImg, $img, 0,0,0,0,$width, $height, $width_orig, $height_orig);
			
			switch(strtolower($regs[1]))
			{
				case "jpg":
				case "jpeg":
					$res = imagejpeg($newImg, $tFile, 100);
					break;
				case "gif":	
					$res = imagegif($newImg, $tFile);
					break;
			}
			
			imagedestroy($newImg);
			imagedestroy($img);
			
			if (!$res) die("Could not create thumbnail: ".$file." at $tFile");
		}
		
		parent::image($baseDir.$subDir.'thumbnails/'.$fileName);
	}	
}
class formInput extends webControl
{			
	public $label;	
	public $required;
	public $validation = "^.{1,}$";
	
	//Set to 1 to render the label AFTER the control (checkboxes)
	public $labelRenderOrder = 0;
	
 	public $desc;	
	
	public function __construct($name, $type, $value='')
	{		
		$this->name = $name;
		$this->id = $name;
		
		$type = strtolower($type);
		
		$this->type = $type;
		$this->value = $value;
		
		$this->controlTag = "input";				
	}		
	public function setLabel($label, $class='', $breakAfter=false)
	{
		$this->label = new phpLabel($label, $this->id, $class);			
		if ($breakAfter) $this->label->breakAfter = true;
	}
	public function setDescription($desc, $class='')
	{
		$this->desc = new phpSpan($desc, $class);		
	}
	public function setRequired($desc='', $class='')
	{
		$this->required = true;	
		
		$tmp = "* ".$desc;		
		if (isset($this->desc) && !$desc) $tmp = "* ".$this->desc->controlData;
		
		$this->setDescription($tmp, $class);
	}
	public function render()
	{
		if (isset($this->label) && !$this->labelRenderOrder) $this->preContent = $this->label->render();
		if (isset($this->label) && $this->labelRenderOrder) $this->postContent = $this->label->render();
		
		if (isset($this->desc) && !$this->controlCloseTag) $this->addControl($this->desc);
		return parent::render();
	}
}
class phpRadioButton extends formInput 
{
	public function __construct($name, $value, $selected = false, $class="")
	{
		if ($class) $this->class = $class;		
		parent::__construct($name, 'radio', $value);
		
		// Set to 1 to render the label AFTER the control!
		$this->labelRenderOrder = 1;
		
		if ($selected) $this->addParameter('checked');
	}	
}

class phpCheckbox extends formInput 
{
	public function __construct($name, $value=1, $selected = false, $class='')
	{
		if ($class) $this->class = $class;		
		parent::__construct($name, 'checkbox', $value);
		
		// Set to 1 to render the label AFTER the control!
		//$this->labelRenderOrder = 1;
		
		if ($selected) $this->addParameter('checked');
	}	
}
class hiddenInput extends formInput
{			
	function hiddenInput($name, $value='')
	{		
		parent::__construct($name, 'hidden', $value);		
	}		
}
class phpForm extends webControl
{	
	var $validate_fields;
	
	function phpForm($method="post", $id="", $action="")
	{	
		$this->method = $method;
		$this->useFieldset = $useFieldset;
		
		if ($id) $this->id = $id;
		//if (!$action) $action = $_SERVER['PHP_SELF'];
		
		if ($action) $this->action = $action;
		
		$this->controlTag = "form";
		$this->controlCloseTag = true;
	}
	function getRequired_string($controls)
	{
		foreach((array)$controls as $control)
		{	
			if ($control->required)
			{
				$req[] = $control->id;
			}						
			
			$cReq = $this->getRequired_string($control->controls);
			if ($cReq) $req[] = $cReq;
		}	
		
		return implode(",", (array)$req);
	}
	function getRequiredWithValidation_string($controls)
	{
		foreach((array)$controls as $control)
		{	
			if ($control->required)
			{
				$req[] = $control->id."::".$control->validation;
			}						
			
			$cReq = $this->getRequiredWithValidation_string($control->controls);
			if ($cReq) $req[] = $cReq;
		}	
		
		return implode(";;", (array)$req);
	}
	function render()
	{
		//if ($this->useFieldset) parent::addControl($this->fieldSet);		
		if ($this->validate_fields)
		{
			$reqString = $this->getRequired_string($this->controls);			
			$this->addAttribute('onsubmit', "if (window.validate_fields) {return validate_fields(this,'".$reqString."');}else{alert('No validation function registered'); return false;}");
		}
		
		return parent::render();	
	}
}
class phpFormFieldset extends webControl 
{
	public $legend;
	
	function phpFormFieldset($title='', $class='')
	{			
		$this->controlTag = "fieldset";
		$this->controlCloseTag = true;
		if ($class) $this->class = $class;
		
		if ($title) $this->legend = new phpLegend($title);
		$this->addControl($this->legend);
	}
}
class phpLegend extends webControl
{
	public function __construct($content)
	{
		$this->controlCloseTag = true;
		$this->controlTag = "legend";
		$this->controlData = $content;
	}
}
class phpUploader extends phpForm 
{	
	function phpUploader($ctl_name, $max_size=50000000, $id='', $action='')
	{
		$this->phpForm("POST", $id, $action);
		
		$this->addAttribute("enctype", "multipart/form-data");
		
		$this->addControl(new hiddenInput("MAX_FILE_SIZE", $max_size));
		$this->addControl(new formInput($ctl_name, 'file'));		
	}	
}
class tblCell extends webControl
{		
	public function __construct($content = "", $class = "", $align = "", $colspan = "", $width = "")
	{		
		if (is_object($content) && is_callable(array($content, "render")))
		{
			$this->addControl($content);
		}
		else if (is_array($content))
		{
			//Array must be an array of controls - no checks here, let it bomb ungracefully if incorrect data type.
			foreach($content as $itm)
			{		
				//No idea why this needed to be wrapped...		
				$this->addControl(new phpLiteral($itm->render()));	
			}
		}
		else
		{
			$this->controlData = $content;
		}
		
		if ($class) $this->class = $class;
		if ($width) $this->width = $width;
		
		if ($align) $this->align = $align;
		if ($colspan) $this->colspan = $colspan;
		
		$this->controlTag = "td";
		$this->controlCloseTag = true;				
	}
}
class tblHeader extends tblCell
{		
	public function __construct($content='', $class='', $align='', $colspan='', $width = '')
	{		
		parent::__construct($content, $class, $align, $colspan, $width);

		//Override just this tag	
		$this->controlTag = "th";		
	}
}
class tblRow extends webControl
{
	private $p_cellCount;
	private $p_headerCount;
	
	public function __construct($class="")
	{	
		if ($class) $this->class = $class;		
		
		$this->controlTag = "tr";
		$this->controlCloseTag = true;	
		$this->renderOrder = 2;
	}
	public function __get($key)
	{
		switch($key)
		{
			case "cellCount":
				return $this->p_cellCount;
				
			case "headerCount":
				return $this->p_headerCount;
		}
	}	
	public function &addCell($cell='')
	{
		if (empty($cell))
		{
			$cell = new tblCell();	
		}
		
		$this->addControl($cell);
		$this->p_cellCount++;
		
		return $cell;
	}
	public function &addHeader($header='')
	{
		if (empty($header))
		{
			$header = new tblHeader();	
		}
		
		$this->addControl($header);
		$this->p_headerCount++;
		
		return $header;
	}	
}
class tblBuilder extends webControl
{
	var $rows;	
	
	public function __construct($align="center", $border=0, $cellspacing=0, $cellpadding=5, $width="100%")
	{
		$this->align = $align;
		$this->border = $border;
		
		$this->cellspacing = $cellspacing;
		$this->cellpadding = $cellpadding;
		
		$this->width = $width;
		
		$this->controlTag = "table";
		$this->controlCloseTag = true;		
		
		$this->addControl(new phpLiteral("<thead>"));
	}	
	public function &addRow($tblRow='')
	{		
		if (empty($tblRow))
		{				
			$tblRow = new tblRow();
		}
		
		$this->addControl($tblRow);	
		return $tblRow;	
	}			
	public function render()
	{			
		foreach($this->controls as $key=>$tRow)
		{
			if (!$key) continue;
			
			if (!$tRow->headerCount)
			{
				$this->insertControlAt(new phpLiteral("</thead><tbody>"), $key);
				break;
			}			
		}
		
		$this->addControl(new phpLiteral("</tbody>"));
		return parent::render();	
	}
}
////////////////////////////////////////
class phpList extends webControl
{	
	var $listItems;
	
	function phpList($type)
	{
		$this->controlTag = $type;
		$this->controlCloseTag = true;
	}	
	function &addListItem($htmlListItem='')
	{
		$idx = count($this->listItems);		
		
		if (!empty($htmlListItem))
		{				
			$this->listItems[$idx] = $htmlListItem;		
		}
		else
		{
			$this->listItems[$idx] = new htmlListItem();
			return $this->listItems[$idx];
		}		
	}	
	function render()
	{
		for($i=0, $ct=count($this->listItems); $i < $ct; $i++)
		{
			$this->addControl($this->listItems[$i]);			
		}	
		
		return parent::render();
	}
}
class phpOrderedList extends phpList
{
	function phpOrderedList()
	{
		parent::phpList("ol");			
	}	
}
class phpUnOrderedList extends phpList
{
	function phpUnOrderedList()
	{
		parent::phpList("ul");			
	}	
}
class htmlListItem extends webControl
{
	function htmlListItem($content='')
	{
		$this->controlTag = "li";
		$this->controlCloseTag = true;	
		$this->controlData = $content;
	}	
}
class phpListItem extends webControl
{		
	public $display;
	
	public function __construct($display='', $value='', $selected='')
	{		
		$this->value = trim($value);
		$this->display = trim($display);				
			
		$selected = (string)$selected;
		
		if (strlen($selected) && (!strcmp($selected,$display) || !strcmp($selected,$value)))
		{		
			$this->addParameter("selected");
		}
		
		$this->controlTag = "option";		
		$this->controlCloseTag = true;
	}
	public function render()
	{
		$this->controlData = $this->display;
		return parent::render();	
	}
}
class phpDDL extends formInput
{	
	public $listItems = array();
	public $selected;	
	
	public function __construct($name)
	{
		$this->name = $name;		
		$this->id = $name;
		
		$this->controlTag = "select";
		$this->controlCloseTag = true;
	}
	public function insertListItem($listItem, $offset = 0)
	{
		if ($offset)
		{
			$seg1 = array_splice($this->listItems, 0, $offset-1);
			$seg1[] = $listItem;
			
			$seg2 = array_splice($this->listItems, $offset);
			
			$this->listItems = array_merge($seg1, $seg2);
		}
		else
		{
			array_unshift($this->listItems, $listItem);	
		}
	}
	public function &addListItem($listItem='')
	{		
		if (!empty($listItem))
		{				
			$this->listItems[] = $listItem;		
		}
		else
		{
			return $this->listItems[] = new listItem();			
		}		
	}	
	public function bind($query, $dataOrdinal="id", $displayOrdinal="value")
	{
		if (gettype($query) != 'object') error("Not a valid mySQL resource.");
		
		while($entry = $query->fetch_assoc())
		{	
			$this->addListItem(new phpListItem($entry[$displayOrdinal], $entry[$dataOrdinal], $this->selected));
		}	
	}
	public function bindByArray($inArr)
	{
		foreach($inArr as $key=>$value)
		{
			$this->addListItem(new phpListItem($key, $value, $this->selected));	
		}			
	}
	public function clearListItems()
	{
		unset($this->listItems);
	}
	public function addControl($ctl) 
	{ 
		// Do nothing - handled by listItems array internally 
	}
	public function render()
	{
		if (isset($this->label)) $this->preContent = $this->label->render();
		if (isset($this->desc)) $this->postContent = $this->desc->render();		
		
		// Clear out anything potentially already added
		$this->controls = array();
		
		foreach($this->listItems as &$li)
		{
			if (strlen($this->selected) && (!strcmp($li->value, $this->selected) || !strcmp($li->data, $this->selected)))
			{	
				$li->addParameter("selected");				
			}
			
			if (!strlen($li->value)) $li->value = $li->display;
			if (!strlen($li->display)) $li->display = $li->value;		

			$this->controls[] = $li;
		}
		
		return parent::render();
	}
}
class phpHyperlink extends webControl
{
	//var $format;
	public $url;
	public $display;
	
	function __construct($url="", $display="", $class="", $newWindow=false)
	{		
		$url = preg_replace("/(\&)([^\#])/", "&amp;$2", $url);		

		// With no URL, this is probably a link being used for a UI control (i.e., no content)
		if (!$url) $url = "javascript:void(0)";
		
		$this->url = $url;
		$this->display = $display;
		
		//if (empty($display) && !is_null($display)) $this->display = $url;
		
		$this->addAttribute("href", $url);		

		$this->controlTag = "a";
		$this->controlCloseTag = true;
		
		if ($class) $this->addClass($class);
				
		if ($newWindow) $this->target = "_blank";
	}
	public function render()
	{
		$this->controlData = $this->display;
		
		return parent::render();
	}
}
class phpEmailLink extends phpHyperlink
{
	public function __construct($email, $caption='', $class='', $newWindow=false)
	{
		$eLnk = validation::encodeEmail("mailto:$email");
		$eDisp = $caption ? $caption : validation::encodeEmail($email);
		
		parent::__construct($eLnk, $eDisp, $class, $newWindow);
	}
}
class phpAnchor extends webControl
{
	function phpAnchor($linkText,$displayText)
	{
		$this->controlData = $displayText;
		$this->name = $this->id = $linkText;
		$this->controlTag = "a";
		$this->controlCloseTag = true;
	}	
}
class phpHeader extends webControl
{
	var $anchorName;
	
	function phpHeader($size, $content, $class='')
	{
		$this->controlTag = "H".$size;
		$this->controlData = $content;
		$this->controlCloseTag = true;
		
		if ($class) $this->class = $class;
	}
	function render()
	{
		if ($this->anchorName)
		{
			$this->addControl(new phpAnchor($this->anchorName, $this->controlData));
			$this->controlData = "";
		}
		
		return parent::render();
	}
}
class phpLabel extends webControl
{
	function phpLabel($content, $formField="", $class="")
	{
		$this->controlTag = "label";
		$this->controlData = $content;
		$this->controlCloseTag = true;
		
		if ($formField) $this->for = $formField;
		
		if ($class) $this->class = $class;
	}	
}
class phpDIV extends webControl
{
	function __construct($content='', $class='')
	{
		$this->controlTag = "DIV";
		$this->controlData = $content;
		$this->controlCloseTag = true;
		
		if ($class) $this->class = $class;
	}	
}
class phpFiller extends webControl
{
	function phpFiller()
	{		
	}
	function render()
	{
		return '';	
	}	
}
class phpSpan extends webControl
{
	function phpSpan($content, $class='')
	{
		$this->controlTag = "SPAN";
		$this->controlData = $content;
		$this->controlCloseTag = true;
		$this->class = $class;
	}		
}
class phpHRule extends webControl
{
	function phpHRule($width, $align)
	{
		$this->addAttribute("width",$width);	
		$this->addAttribute("align",$align);	
		
		$this->controlTag = "HR";		
	}	
}
class phpTextArea extends formInput
{		
	public function __construct($name, $value="", $rows="", $cols="", $class="")
	{		
		$this->id = $name;
		
		if ($class) $this->class = $class;
		
		$this->id = $name;		
		
		$this->rows = $rows;
		$this->cols = $cols;
		
		$this->controlData = $value;
		
		$this->controlTag = "TEXTAREA";
		$this->controlCloseTag = true;
	}	
	function render()
	{		
		if (isset($this->desc))
		{
			if ($this->breakAfter) 
			{
				$this->breakAfter = false;
				$this->desc->breakAfter = true;
			}
			
			$this->postContent = $this->desc->render();		
		}
		
		return parent::render();	
	}
}
class phpParagraph extends webControl
{
	var $content;
	
	function phpParagraph($content='', $class='', $align='')
	{		
		if ($align) $this->addAttribute("ALIGN",$align);
		if ($class) $this->class = $class;
		
		$this->controlTag = "P";
		$this->controlCloseTag = true;	
		
		$this->controlData = $content;
	}
}
class phpPassword extends formInput 
{
	function phpPassword($name, $value='', $size='', $maxSize='', $class='')
	{		
		$this->name = $this->id = $name; 
		$this->class = $class;
		
		$this->addAttribute("type",'password');
		$this->addAttribute("value",$value);		
		$this->addAttribute('size', $size);
		$this->addAttribute('maxlength', $maxSize);
		
		$this->controlTag = "INPUT";		
	}	
}
class phpLiteral
{
	var $literal;
	
	function phpLiteral($literal='')
	{
		$this->literal = $literal;
	}
	function render()
	{
		return $this->literal;	
	}
}

class phpSoap
{
	var $serverURL;
	var $ns;
	
	private $client;
	
	function phpSoap()
	{
		global $config;
		
		$this->ns = $config['namespace'];
		$this->serverURL = $this->ns."/soap_server.php";		
		
		$this->client = new SoapClient(null, array('location' => $this->serverURL,
                                     			   'uri'      => $this->ns,
												   'connection_timeout' => 3000,
												   'timeout' => 3000,
												   'target_timeout' => 3000,
												   'trace', 1,
												   'exceptions', 1));                                     			   		
	}	
	function call($operation, $params=array())
	{
		global $config;
		
		try
		{			
			//Append this for verification
			$params["scripturl"] = $config[scripturl];
			
			//$val = parent::call($operation, $params, $this->nameSpace); //print_r($val);
			@$val = $this->client->__soapCall($operation, $params);
			/*
			if(PEAR::iserror($val))
			{
				$error = "Error ".$val->getCode()." [ ".$val->getMessage()."]";
				die("Error communicating with master server: ".$error);
			}		
			*/
		}
		catch(SoapFault $e)
		{			
			return false;	
		}
		catch(Exception $e)
		{			
			return false;	
		}
		
		return $val;
	}
}
////////// Custom //////////////////
class pr_footer extends pageContent
{
	function pr_footer()
	{
		global $config;
		
		$fText = "Copyright&copy; ".date("Y")." $config[businessName] and Mainstay Technologies, LLC. All rights reserved.<br />
					<br /><strong>Powered By: Proteus CMS v3.0</strong> &copy; ".date("Y")." by <a href='http://www.mstech.com/'>Mainstay Technologies</a><br />
				All Rights Reserved. Website engine code is &copy; copyright by <a href='http://www.mstech.com/'>Mainstay Technologies</a>.";
				
		$renderTime = round(pageClass::getMicroTime() - START_TIME, 4);
		
		if ($renderTime)
		{
			$d = new DAL();		
			$d->suppressErrors = true;
			@$hitCount = $d->qryCount("select keyValue from config where keyName = 'hitCount'");
			
			$fText .= "<br /><br />".($hitCount ? number_format($hitCount)." total page reads.&nbsp;&nbsp;" : "")."Page rendered in $renderTime(s)";	
		}
		
		$hImg = new image("http://www.mstech.com/images/valid-xhtml.gif");
		$hImg->setAltTag("Valid XHTML 1.0 Transitional");
		$hLnk = new phpHyperlink("http://validator.w3.org/check?uri=referer", $hImg->render(), false, true);
		$hLnk->class = 'validHtml';
		
       	$cImg = new image("http://www.mstech.com/images/valid-css.gif");
		$cImg->setAltTag("Valid CSS!");
		$cLnk = new phpHyperlink("http://jigsaw.w3.org/css-validator/", $cImg->render(), false, true);
		$cLnk->class = 'validCss';       
		
		//$this->addControl($hLnk);		
		//$this->addControl($cLnk);
		$this->addControl(new phpParagraph($fText, "proteus_footer"));
	}
}
class editControl extends jqUI_button 
{
	var $origUrl;
	
	function __construct($url='', $customGif=false, $format=false, $newWindow=false)
	{
		parent::__construct("btnEdit_".md5($url ? $url : proteus_core::generateRandomString()),"","actionControl ui-state-highlight","ui-icon-pencil", true);
		$this->title = 'Edit';
		
		$this->origUrl = $url;		
		
		if (preg_match("/javascript:(.*)/is", $url, $matches))
		{
			$this->onclick = $matches[1];
		}
		else
		{
			$this->onclick = "window.location='$url';";
		}
	}	
	function getTextLink($display)
	{
		return new phpHyperlink($this->origUrl, $display);	
	}	
}
class saveControl extends jqUI_button 
{
	public $origUrl;	
	
	function __construct($url='', $customGif=false, $format=false, $newWindow=false)
	{	
		parent::__construct("btnSave_".md5($url ? $url : proteus_core::generateRandomString()),"","actionControl saveControl ui-state-highlight","ui-icon-disk", true);
		$this->title = 'Save';
		
		$this->origUrl = $url;		
		
		if (preg_match("/javascript:(.*)/is", $url, $matches))
		{
			$this->onclick = $matches[1];
		}
		else
		{
			$this->onclick = "window.location='$url';";
		}
	}	
	function getTextLink($display)
	{
		return new phpHyperlink($this->origUrl, $display);	
	}	
}
class deleteControl extends jqUI_button
{
	private $p_verify;
	
	function deleteControl($url='', $customGif=false, $format=false, $newWindow=false)
	{	
		parent::__construct("btnDel_".md5($url ? $url : proteus_core::generateRandomString()),"","actionControl deleteControl ui-state-error","ui-icon-trash", true);
		$this->title = 'Delete';		
		
		if (preg_match("/javascript:(.*)/is", $url, $matches))
		{
			$this->onclick = $matches[1];
		}
		else
		{
			$this->onclick = "window.location='$url';";
		}
	}	
	function setVerifyMessage($message)
	{
		$this->p_verify = $message;
	}
	function render()
	{
		if ($this->p_verify)
		{
			$this->onclick = "if (confirm('$this->p_verify')) { {$this->onclick} }";
		}
		
		return parent::render();
	}
}
class viewControl extends jqUI_button 
{
	public $origUrl;	
	
	function __construct($url='', $customGif=false, $format=false, $newWindow=false)
	{	
		parent::__construct("btnView_".md5($url ? $url : proteus_core::generateRandomString()),"","actionControl","ui-icon-search", true);
		$this->title = 'View';
		
		$this->origUrl = $url;		
		
		if (!$url) return;
		
		if (preg_match("/javascript:(.*)/is", $url, $matches))
		{
			$this->onclick = $matches[1];
		}
		else
		{
			$this->onclick = "window.location='$url';";
		}
	}	
	function getTextLink($display)
	{
		return new phpHyperlink($this->origUrl, $display);	
	}	
}
class phpButton extends webControl
{
	public function __construct($id="", $label="", $class="")
	{
		if ($id) $this->id = $id;
		if ($class) $this->class = $class;
		
		$this->controlTag = "button";
		$this->controlCloseTag = true;
		$this->controlData = $label;
		$this->type = "button";
	}
}
class phpTextBox extends formInput 
{
	public function __construct($name, $value='', $size='', $maxSize='', $class='')
	{		
		$this->name = $this->id = $name; 
		$this->class = $class;
		
		$this->type = "text";
		$this->value = $value;
		
		$this->size = $size;
		$this->maxlength = $maxSize;
		
		$this->controlTag = "input";		
	}	
}
class backBlock extends pageContent
{
	function backBlock($url, $width='200px')
	{
		$cont = new panelBlock(null, null, $width);
		$cont->id = 'backBlock';
		
		$cmdBack = new jqUI_button('backButton','Go Back', "", "ui-icon-seek-prev");		
		$cmdBack->onclick = "window.location='$url';";		
		
		$cont->addControl($cmdBack);			
		
		$this->addControl($cont);
	}	
}
class phpDivHeaderBlock extends phpDIV 
{
	var $p_title;
	var $heading;
	var $subheading;
	
	var $head_link;
	var $sub_link;	
	
	var $imgUrl;	
	
	var $postNote;
	
	var $showImage;	
	
	var $image;
	
	function phpDivHeaderBlock($title, $heading, $subheading, $width='550px')
	{
		global $config;
		
		parent::__construct('','headerBlock');
		
		if ($title)
		{
			$this->p_title = new phpHeader(2, $title);
		}
		else
		{
			$this->p_title = new phpFiller();	
		}
		
		if ($heading)
		{
			$this->heading = new phpHeader(3, $heading);
		}
		else 
		{
			$this->heading = new phpFiller();	
		}
		
		if ($subheading)
		{
			$this->subheading = new phpHeader(4, $subheading);
		}
		else 
		{
			$this->subheading = new phpFiller();	
		}
		
		if ($width)
		{			
			if (strlen($width))
			{
				if (!eregi("px|\%$", $width)) $width .= "px";
			}
			
			//Nasty hack. =(
			if ($width == '100%') $width = '98%';
			$this->addAttribute('style', "width: {$width};");			
		}
				
		$this->showImage = true;
		
		$this->imgUrl = $config[theme]."/images/logo_small.png";
		$this->showImage = true;
		
		$this->image = new image($this->imgUrl);
		$this->image->id = "headerImage";
		$this->image->setAltTag($title);
	}
	function render()
	{	
		if ($this->sub_link)
		{
			$subDisp = new phpHyperlink($this->sub_link, $this->subheading->render());
		}
		else
		{
			$subDisp = $this->subheading;
		}			
		
		if ($this->head_link)
		{
			$headDisp = new phpHyperlink($this->head_link, $this->heading->render());
		}
		else
		{
			$headDisp = $this->heading;
		}
		
		$titleDisp = $this->p_title;
		
		if ($this->image && $this->showImage)
		{		
			$this->addControl($this->image);
		}		
		
		$dvText = new phpDIV('','headerTextContainer');
		$dvText->breakAfter = true;
		
		$dvText->addControl($titleDisp);
		$dvText->addControl($headDisp);
		$dvText->addControl($subDisp);
		
		$this->addControl($dvText);
		
		if ($this->postNote)
		{
			$pNote = new phpDIV($this->postNote, 'headerNote');
			$pNote->breakAfter = true;
			$this->addControl($pNote);
		}		
		
		return parent::render();
	}
}
class phpHeaderBlock extends pageContent 
{
	var $p_title;
	var $heading;
	var $subheading;
	
	var $head_link;
	var $sub_link;	
	
	var $imgUrl;	
	
	var $postNote;
	
	var $showImage;	
	
	var $image;
	
	var $panel;
	
	function phpHeaderBlock($title, $heading='', $subheading='', $width='550px')
	{
		global $config;
		
		if ($title)
		{
			$this->p_title = new phpSpan($title,'headerTitle');
		}
		else
		{
			$this->p_title = new phpFiller();	
		}
		
		if ($heading)
		{
			$this->heading = new phpSpan($heading, 'headerHeading');
		}
		else 
		{
			$this->heading = new phpFiller();	
		}
		
		if ($subheading)
		{
			$this->subheading = new phpSpan($subheading, 'headerSubHeading');
		}
		else 
		{
			$this->subheading = new phpFiller();	
		}
		
		$this->panel = new panelBlock(null, null, $width);		
		$this->panel->ID = 'headerBlock';		
		
		$this->imgUrl = $config[theme]."/images/logo_small.png";
		$this->showImage = true;		
		
		$this->image = new image($this->imgUrl);
		$this->image->id = "headerImage";
	}
	function render()
	{	
		if ($this->sub_link)
		{
			$subDisp = new phpHyperlink($this->sub_link, $this->subheading->render());
		}
		else
		{
			$subDisp = $this->subheading;
		}			
		
		if ($this->head_link)
		{
			$headDisp = new phpHyperlink($this->head_link, $this->heading->render());
		}
		else
		{
			$headDisp = $this->heading;
		}
		
		$titleDisp = $this->p_title;
		
		if ($this->image && $this->showImage)
		{		
			$this->panel->addControl($this->image);
		}		
		
		$dvText = new phpDIV('','headerTextContainer');
		$dvText->breakAfter = true;
		
		$dvText->addControl($titleDisp);
		$dvText->addControl($headDisp);
		$dvText->addControl($subDisp);
		
		$this->panel->addControl($dvText);
		
		if ($this->panel->postNote)
		{
			$pNote = new phpParagraph($this->postNote);
			$pNote->id = "headerNote";
			
			$this->addControl($pNote);
		}		
		
		return $this->panel->render();
	}
}
class errorHeader extends phpHeaderBlock 
{
	function errorHeader($desc, $width=550)
	{
		global $config;		
		
		$this->p_title = new phpDIV("Error processing request!", 'errorTitle');
		$this->heading = new phpDIV($desc, 'errorDescription');
		$this->heading->breakAfter = true;
		
		$this->panel = new panelBlock(null, null, $width);		
		$this->panel->id = 'errorBlock';	
		
		$this->imgUrl = $config[theme].'/images/error.gif';
		$this->showImage = true;		
		
		$this->image = new image($this->imgUrl);
		$this->image->id = "errorImage";
	}		
}
class jqDatePicker extends phpTextBox
{
	public function __construct($id, $value='', $label='')
	{
		parent::phpTextBox($id, $value, 12, 10);
		
		$this->addClass('datePicker');
		if ($label) $this->setLabel($label);
		$this->addParameter('readonly');
		$this->id = $id;
	}
}
class datePicker extends phpDIV
{
	public $textControl;
	public $chooseControl;
	public $clearControl;
	public $showClear = true;
	
	public function __construct($controlName, $label='', $textData='', $customContainerClass='')
	{
		$this->textControl = new phpTextBox($controlName, $textData, 10, 10);		
		$this->textControl->setLabel($label);
		$this->textControl->addParameter('readonly');
		
		$this->chooseControl = new phpHyperlink("javascript:displayDatePicker('$controlName', null, 'EEMMMdy');","&nbsp;");
		$this->chooseControl->id = "pickdate_".time();
		$this->chooseControl->alt = "Pick Date";
		$this->chooseControl->class = 'date_pick';
		
		$this->clearControl = new phpHyperlink("#", "&nbsp;");
		$this->clearControl->onclick = "document.getElementById('$controlName').value = ''; return false;";
		$this->clearControl->id = "cleardate_".time();
		$this->clearControl->alt = "Clear";		
		$this->clearControl->class = 'date_clear';
		
		if (!$customContainerClass) $customContainerClass = 'datePickerContainer';
		
		parent::__construct('', $customContainerClass);
	}
	public function render()
	{
		$this->addControl($this->textControl);	
		$this->addControl($this->chooseControl);
		if ($this->showClear) $this->addControl($this->clearControl);
		
		return parent::render();
	}
}
class phpAcronym extends webControl
{
	function phpAcronym($acronym, $definition)
	{
		$this->controlTag = "acronym";
		$this->controlCloseTag = true;
		
		$this->addAttribute("title", $definition);
		$this->controlData = $acronym;
	}	
}
class phpAbbr extends webControl
{
	function phpAbbr($abbreviation, $definition)
	{
		$this->controlTag = "abbr";
		$this->controlCloseTag = true;
		
		$this->addAttribute("title", $definition);
		$this->controlData = $abbreviation;
	}	
}
////////// JQUERY /////////////
class jqUI_dialog extends phpDIV
{
	public $iconClass;	
	private $jsonData;
	private $jsonButtons = array();
	public $caption = '';
	public $removeOnClose = false;
	
	public function __construct($id, $removeOnClose=false, $width="", $height="", $title="")
	{
		parent::__construct();
		
		$this->id = $id;
		$this->name = $id;
		
		$this->jsonData = new json();
		$this->jsonData->autoQuote = false;
		$this->jsonData->renderMode = 1;
		
		$this->setOption("autoOpen", "false");
		$this->setOption("bgiframe", "true");
		$this->setOption("modal", "true");
		$this->setOption("draggable", "false");
		$this->setOption("resizable", "false");
		
		if ($width) $this->setOption("width", $width);
		if ($height) $this->setOption("height", $height);
		if ($title) $this->setOption("title","'$title'");		
		
		$this->removeOnClose = $removeOnClose;
	}
	public function fitToViewport($percentOfViewport=.98)
	{
		$this->setOption("height", "function() { return $(window).height() * $percentOfViewport; }()");
	}
	public function setOption($option, $value)
	{
		$this->jsonData->addObject($option, $value);	
	}
	public function addButton($caption, $function, $class='')
	{		
		$jsObj["text"] = "'$caption'";
		$jsObj["click"] = $function;		
		
		if ($class) $jsObj["class"] = "'$class'";
		
		//Got tired of dealing with json_encode and quotes (quotes everything with no option, wtf!)
		$this->jsonButtons[] = json::getUnformattedJSONFromArray($jsObj);
	}
	public function addCloseButton($caption="Cancel")
	{
		$this->addButton($caption, "function() { $(this).dialog('close'); }");	
	}
	public function chooseIcon($index)
	{
		//0 = none
		//1 = warn
		//2 = confirm

		switch($index)
		{
			case 0:
				$this->iconClass = null;
				break;
			case 1:	
				$this->iconClass = "ui-icon-alert";
				break;
			case 2:
				$this->iconClass = "ui-icon-circle-check";
				break;
		}
	}
	public function render()
	{
		if ($this->removeOnClose)
		{
			$this->setOption("close", "function() { $(this).remove(); }");	
		}
		
		if ($this->caption)
		{
			$par = new phpParagraph();
			
			if ($this->iconClass)
			{
				$par->addControl(new jqUI_label($this->caption, $this->iconClass));					
			}
			else
			{	
				$par->addControl(new phpLiteral($this->caption));
			}
			
			$this->addControl($par);		
		}
		
		$buttons = "[".implode(",",(array)$this->jsonButtons)."]";		
		
		$this->addControl(new jqLoader("
			var tID;
			var i=0;
			
			$('#".$this->id."').dialog(".$this->jsonData->render().").dialog('option', 'buttons', $buttons);
			$('.ui-tooltip').hide();"));
		
		return parent::render();		
	}
}
class jqUI_dismissableMessage extends jqUI_dialog
{
	private $p_sec;
	private $p_key;

	public function __construct(security $sec, $messageKey, $message, $title="")
	{
		parent::__construct("dismissableMessageWin", true, 550, "", $title ? $title : "System Message");
		
		$this->setOption("autoOpen", true);
		$this->setOption("autoResize", true);
		$this->setOption("modal", 1);
		
		$this->setOption("focus", "function() { $(this).closest('.ui-dialog').css('z-index', 999999); }");
				
		$this->addAttribute("data-key", $messageKey);
		
		$this->chooseIcon(1);
		
		$this->p_sec = $sec;		
		$this->p_key = $messageKey;
		
		$this->addControl(new phpParagraph($message));
	}
	public function render()
	{
		$sObj = $this->p_sec->notification_settings;
		$key = $this->p_key;
		
		if ($sObj->$key) return "";
		
		$this->addCloseButton("Ok");
		if ($this->p_sec->id) $this->addButton("Don`t Tell me Again", "function() { dismiss(); }");
		
		return parent::render();
	}
}
class jqUI_confirm extends jqUI_dialog
{	
	public function __construct($id, $caption, $show_warn=false, $defaultCancel=true)
	{		
		$this->caption = $caption;
		$this->chooseIcon($show_warn ? 1 : 2);
		
		parent::__construct($id);		
		
		if ($defaultCancel)
		{
			$this->addButton("Cancel", "function() { $(this).dialog('close'); }");			
		}
		
		$this->setOption("autoOpen", "false");
		//$this->setOption("height", 140);
		$this->setOption("title", "'Confirm'");		
	}
}
class jqUI_errorNotify extends jqUI_notify
{
	public function __construct(Exception $ex)
	{
		//TODO: Log this user error!		
		parent::__construct($ex->getMessage(), 0, "eWin", true, 1, true, "An error has occurred!");
	}
}
class jqUI_notify extends jqUI_dialog
{		
	public function __construct($message='', $timeout = 2, $id='', $autoOpen=true, $iconIdx=2, $removeOnClose=true, $title='')
	{
		// SWB - 2/13 Not sure why this was ever here, but new jQuery > 1.9 doesn't like it
		//if ($title) $title .= "<br />";
		
		$this->caption = $message;
		
		parent::__construct($id ? $id : 'jqui_notifyWindow');
		
		$this->chooseIcon($iconIdx);		
		
		$this->setOption("width", 425);
		
		$this->setOption("autoOpen", $autoOpen ? 'true' : 'false');
		
		if ($timeout)
		{
			$this->setOption("title", "'{$title} - Closing in $timeout seconds'");
			$this->setOption("open", "function() 
			{		
				$('p', this).css('display','block');
				
				tID = setInterval(function()
				{ 				
					i++;				
					if (i >= $timeout)
					{					
						$('#{$this->id}').dialog('close');
						clearTimeout(tID);
					}
					$('#{$this->id}').dialog('option', 'title','{$title} Closing in ' + ($timeout-i) + ' seconds');
				},1000);
			}");
		}
		else
		{
			$this->setOption("title", "'$title'");	
		}
		
		$this->removeOnClose = $removeOnClose;
		
		$this->addButton("Ok", "function() { $(this).dialog('close'); }");		
	}
}
class jqUI_datepicker extends phpTextBox
{
	private $jsonData;
	public $allowClear;
	public $container;
	
	public function __construct($id, $date='', $caption='', $allowClear=true, $minDate = "new Date()", $dateFormat = "mm-dd-yy")
	{
		parent::__construct($id, $date, 10, 10);
		
		global $config;
		
		$this->id = $id;
		$this->class = 'text';
		
		$this->jsonData = new json_a();		
		$this->jsonData->autoQuote = false;		

		if ($caption) $this->setLabel($caption);
		
		$this->addParameter('readonly');
		$this->allowClear = $allowClear;
		
		$this->setOption("showOn", "'both'");
		$this->setOption("buttonImage", "'proteus/jquery/ui/supplement/calendar.gif'");
		$this->setOption("buttonImageOnly", "true");
		$this->setOption("buttonText", "'Choose Date'");
		$this->setOption("dateFormat", "'$dateFormat'");
		$this->setOption("constrainInput", "true");
		$this->setOption("minDate",$minDate);	
		$this->setOption("showButtonPanel", "true");	
		
		$dvContainer = new phpDIV();
		$dvContainer->class = 'datePickerContainer';
		$dvContainer->id = $this->id."_container";
		
		$this->container = $dvContainer;		
	}
	public function setOption($option, $value)
	{
		if (!$value) $value = "''";
		$this->jsonData->addObject($option, $value);	
	}
	public function render()
	{
		global $config;		
		
		if ($this->allowClear)
		{
			$cImg = new image("proteus/jquery/ui/supplement/clear_date.gif");
			$cImg->setAltTag("Clear Date");			
			$cImg->style = ($this->value ? "" : "display: none; ")."width: 16px; height: 15px;";
			$cImg->class = 'ui-datepicker-clear';
			$cImg->onclick = "$(this).prevAll('input:eq(0)').val(''); $(this).fadeOut();";
			
			$this->setOption("onSelect", "function() { 
				var obj = $(this);
				var ctl = obj.nextAll('.ui-datepicker-clear');
					
				if (obj.val())
				{
					ctl.fadeIn();
					return;					
				}
				
				ctl.fadeOut();				
			}");
		}
		else
		{
			$cImg = new phpFiller();				
		}		
		
		$scrpt = new jqLoader("$('[name=\\\"{$this->id}\\\"]').datepicker(".$this->jsonData->render().").prop('readonly', true);");			
		
		$this->container->addControl(new phpLiteral(parent::render()));
		$this->container->addControl($cImg);
		$this->container->addControl($scrpt);
		
		return $this->container->render();
	}
}
class jqUI_button extends phpButton 
{	
	private $iconClass = '';
	private $intialize = false;
	private $p_title;
	
	public function __construct($name, $label, $class='', $iconClass='', $renderInitializer = false)
	{	
		parent::__construct($name, $label);
		$this->class = "jqui_button ".($class ? $class : "");
		
		$this->p_title = $label;
		
		if ($iconClass) $this->setIconClass($iconClass);
		$this->intialize = $renderInitializer;
	}
	public function setIconClass($class)
	{
		$this->addAttribute("data-iconclass", $class);
		$this->iconClass = $class;	
	}
	public function render()
	{
		if (!$this->title) $this->title = $this->p_title;
		
		if ($this->intialize)
		{
			if (!$this->controlData) $txt = ",text: false";
			
			$js = new jqLoader("initJQButtons()");		
			$this->postContent = $js->render();
		}
		
		return parent::render();
	}
}
class jqUI_icon extends phpSpan 
{
	public function __construct($iconClass, $class='')
	{
		parent::phpSpan('', "ui-icon $iconClass");
		if ($class) $this->addClass($class);		
	}
}
class jqUI_label extends phpSpan
{
	public $icon;
	
	public function __construct($label, $iconClass='', $class='')
	{	
		if ($iconClass)
		{
			$this->icon = new jqUI_icon($iconClass);
			$this->icon->style = "display: inline-block !important; margin-right: 4px;";
			
			$this->addControl($this->icon);	
		}
		
		parent::phpSpan($label, $class);
		$this->renderOrder = 1;
	}
}
class jqUI_hyperlink extends phpHyperlink 
{	
	public $iconClass;
	
	public function __construct($url, $display, $class="", $iconClass="", $newWindow="")
	{
		parent::__construct($url, $display, $class, $newWindow);	
		
		$this->iconClass = $iconClass;
	}
	public function render()
	{
		if (!$this->rendered)
		{
			$this->addClass('jqLink');
			
			$icon = new jqUI_icon($this->iconClass);
			$this->display = $icon->render().$this->display;
		}

		return parent::render();
	}
}
class jqUI_linkControl extends jqUI_hyperlink
{
	public function __construct($iconClass="ui-icon-closethick", $class="", $title="")
	{
		parent::__construct("", "", $class, $iconClass);
		
		$this->addClass("jqLinkControl");
		if ($title) $this->title = $title;
	}
}
class jqUI_uploader extends phpDIV
{
	public $uploadButton;
	public $formInput;
	
	public function __construct($elementID, $caption, $buttonID, $buttonCaption = 'Upload', $renderInitializer = false)
	{
		$this->uploadButton = new jqUI_button($buttonID, $buttonCaption, "", "ui-icon-link", true);
		$this->uploadButton->style = "display: none;";
					
		$this->formInput = new formInput($elementID,'file');	
		$this->formInput->onchange = "$('#$buttonID').show();";
		$this->formInput->setLabel($caption);
		
		parent::__construct("", "uploadDiv");
		
		$this->addControl($this->formInput);
		$this->addControl($this->uploadButton);
		if ($renderInitializer) $this->addControl(new jqLoader("$('#$elementID').fileinput();"));
	}
}
/**
 * jqLoader 
 * Meant to simplify using a new javaControl with the $(function() { }) JQuery syntax for running something after page finishes loading
 */
class jqLoader extends javascriptControl   
{
	public function __construct($stmnt)
	{
		parent::__construct("$(function() { $stmnt })");		
	}
}
class jqCarouselSlide extends phpDIV
{	
	public $caption;
	
	public function __construct($link, $caption, $image, $background, $desc)
	{
		parent::__construct();
		
		$this->class = 'slide';
		//$this->display = 'none';
		//$this->id = "slide_$ct";
		
		$this->caption = $caption;
		
		$this->style = "background-image: url(includes/js/jquery/themes/carousel/slides/$background);";	
		
		$img = new image($image);
		$img->setAltTag($caption);
		
		$hdr = new phpHeader(1, $caption);
		
		$lnk = new phpHyperlink($link, $img->render().$hdr->render());
		
		$dvFoot = new phpDIV('','slideFooter');
		$dvFoot->addControl($lnk);
		$dvFoot->addControl(new phpParagraph($desc));
		
		$this->addControl($dvFoot);
	}
}
class jqCarousel extends phpDIV
{	
	public $slides;
	public $innerDiv;
	public $slideCount;
	public $shuffle;
	private $jsonData;
	            
	public function __construct($id, $shuffle = false)
	{
		parent::__construct();
		$this->id = $id;
		
		$this->innerDiv = new phpDIV();
		$this->innerDiv->id = 'slide_holder_inner';
		
		$lImg = new image("includes/js/jquery/themes/carousel/images/ajax_loader.gif");
		$lImg->class = 'loading';
	
		$this->addControl($lImg);
		
		$this->slides = new phpDIV();
		$this->slides->id = 'row_of_slides';
		
		$this->slideCount = 0;
		$this->shuffle = $shuffle;
		
		$this->jsonData = new json();	
		$this->jsonData->renderMode = 1;
		$this->jsonData->autoQuote = false;	

		$this->setOption("first_last_buttons","'no'");
		$this->setOption("hover_next_prev_buttons","'yes'");
		$this->setOption("next_prev_buttons","'yes'");
		$this->setOption("pause_button","'yes'");
		$this->setOption("slide_buttons","'yes'");
		$this->setOption("slide_number_display","'no'");
		$this->setOption("timer_length","7500");
		$this->setOption("transition_duration","700");
		$this->setOption("transition_easing","'easeInBack'");
		$this->setOption("transition_type","'carousel'");
		$this->setOption("water_mark","'yes'");
	}
	public function addSlide($ctl)
	{
		//Don't like this, but also don't want to re-write the carousel to use proper indexing!
		$this->slideCount++;
		$ctl->id = 'slide_'.$this->slideCount;
		
		$this->slides->addControl($ctl);	
	}
	public function setOption($option, $value)
	{
		$this->jsonData->addObject($option, $value);	
	}
	public function render()
	{		
		if ($this->shuffle) shuffle($this->slides->controls);
		
		foreach($this->slides->controls as $slide)
		{
			$captions[] = $slide->caption;				
		}
		
		$captions = implode("|", $captions);
		$this->setOption("slide_captions", "'$captions'");
		
		$this->innerDiv->addControl($this->slides);
		
		$cDiv = new phpDIV();
		$cDiv->id = 'carousel_controls';
		
		$this->innerDiv->addControl($cDiv);
		
		$this->addControl($this->innerDiv);
		
		$this->addControl(new jqLoader('$("#slide_holder").agile_carousel('.$this->jsonData->render().');'));
		
		return parent::render();	
	}
}
class phpMeta extends webControl 
{
	public function __construct($name, $content)
	{
		$this->controlTag = "meta";		
		$this->content = $content;
		$this->name = $name;
	}
}
class shareBlock extends phpDIV
{
	public function __construct($url)
	{
		global $config;

		$controls[twitter] = "<a href=\"https://twitter.com/share\" class=\"twitter-share-button\" data-count=\"horizontal\">Tweet</a>
		<script type=\"text/javascript\" src=\"//platform.twitter.com/widgets.js\"></script>";
			
		$controls[googleplus] = "<g:plusone size=\"medium\" href=\"$config[scripturl]$url\"></g:plusone>
		<script type=\"text/javascript\">
		(function() {
		var po = document.createElement('script'); po.type = 'text/javascript'; po.async = true;
		po.src = 'https://apis.google.com/js/plusone.js';
		var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(po, s);
		})();
		</script>";
	
		$controls[facebook] = "<div class=\"fb-like\" data-href=\"$config[scripturl]$url\" data-send=\"false\" data-layout=\"button_count\" data-width=\"90\" data-show-faces=\"true\"></div>";
				
		parent::__construct("","shareBlock");
			
		foreach($controls as $ctl)
		{
			$this->addControl(new phpLiteral($ctl));
		}
	
		$this->addControl(new addThisControl());
	}
}
class facebookSDK extends phpDIV
{
	function __construct()
	{
		global $config;
		
		parent::__construct();
		
		$this->id = "fb-root";
		$this->addControl(new javascriptControl("","http://connect.facebook.net/en_US/all.js"));
		$this->addControl(new javascriptControl("FB.init({
				appId:'$config[facebook_id]',
				status:true,
				xfbml:true,
				oauth: true
		});"));
	}
}
class phpNavigation extends phpDIV
{	
	var $startPos;
	var $totalCount;
	var $linkTemplate;
	var $perPage;
	var $backForwardNav;
	
	function phpNavigation($startPos, $totalCount, $perPage, $linkTemplate, $bfNav=true)
	{
		$this->startPos = $startPos;
		$this->totalCount = $totalCount;
		$this->perPage = $perPage;
		$this->linkTemplate = $linkTemplate;
		$this->backForwardNav = $bfNav;
		
		parent::__construct('','ajaxNav');
		
		$nav = array();
		
		$currentPage = ($this->startPos ? ($this->startPos / $this->perPage)+1 : 1);		
		$totalPages = ($this->totalCount / $this->perPage);
		
		//Add a page if it doesn't divide evenly...
		if ($this->totalCount % $this->perPage) $totalPages++;		
		$this->totalCount++;
		
		$delim = 0;
		for($i=0; $i <= $totalPages-1; $i++)
		{			
			$pageVal = $i + 1;
			$sVal = $i * $this->perPage;
			
			if ($pageVal != $currentPage)
			{
				$link = str_replace("{position}", $sVal, $this->linkTemplate);
				$nLink = new phpHyperlink($link, number_format($pageVal), "ui-state-default navButton");
				$nLink->addAttribute('data-startpos', $sVal);								
			}
			else
			{
				$nLink = new phpSpan(number_format($currentPage), 'navCurrent');					
			}
			
			$nav[] = $nLink;			
		}	
		$this->addControl($nav[0]);
		
		if ($currentPage < 5)
		{			
			$start = 1;								
		}		
		else
		{
			$start = $currentPage - 4;										
		}	
		
		$end = $currentPage + 2;
		
		if ($start > 1) $this->addControl(new phpSpan("...",'middleSpan'));
		
		//Don't include the last element - it's popped off at the end.
		if ($end > ($totalPages-2)) $end = ($totalPages-2);		
		
		for($j = $start; $j <= $end; $j++)
		{
			$this->addControl($nav[$j]);	
		}
		
		if ($end < ($totalPages-3)) $this->addControl(new phpSpan("...",'middleSpan'));
		
		$last = array_pop($nav);
		$this->addControl($last);		
		
		if ($this->backForwardNav)
		{		
			if ($currentPage < $totalPages-1)
			{
				$sPos = ($currentPage) * $this->perPage;
				
				$link = str_replace("{position}", $sPos, $this->linkTemplate);
				
				$nIco = new jqUI_icon('ui-icon-seek-next');

				$nLink = new phpHyperlink($link, $nIco->render(), "bfNav");
				
				$nLink->addAttribute('data-startpos', $sPos);
				
				$this->addControl($nLink);				
			}			
			
			if ($currentPage != 1)
			{
				$sPos = ($currentPage-2)*$this->perPage;
				
				if ($sPos < 0) $sPos=0;
				
				$link = str_replace("{position}", $sPos, $this->linkTemplate);
				//$nLink = new phpHyperlink($link, "&lt;", "navButton");
				
				$nIco = new jqUI_icon('ui-icon-seek-prev');			
					
				$nLink = new phpHyperlink($link, $nIco->render(), "bfNav");
				$nLink->addAttribute('data-startpos', $sPos);
				
				$this->insertControl($nLink);				
			}						
		}
		
		$this->insertControl(new phpSpan("Go to:", "navLead"));
	}	
}
class tableFilter extends phpDIV
{
	const filter_text = 1;
	const filter_date = 2;
	const filter_dropdown = 3;
	const filter_date_range = 4;
	const filter_text_having = 5;
	
	public $customFilters = array();
	public $sortFilters = array();
	public $columnFilters = array();
	public $params = array();
	public $sortData = array();
	public $pagingData = array();
	public $jsMethod;
	public $tableSelector;
	public $showPager = true;
	public $header;
	public $filterButton;
	
	public $autoRun = 1;
	public $autoHide = 1;
	public $showFilteredColumns = 1;
	
	public $loaderDeferred = false;
	
	public function __construct($id, $tableSelector, $jsMethod="getData", $headerCaption="Filter Results", $showPager = true)
	{
		$this->tableSelector = $tableSelector;
		$this->params = $params;
		$this->jsMethod = $jsMethod;
		$this->id = $id;
		
		$this->filterButton = new jqUI_button("btnFilter_$id", "Filter Results", "filterButton", "ui-icon-search", true);
		$this->filterButton->onclick = "$('#{$this->id}').filterHelper()";
		
		$this->showPager = $showPager;
		
		$lnk = new phpHyperlink("#", $headerCaption);
		$lnk->addControl(new phpSpan("&nbsp;"));
		
		$this->header = new phpHeader(3, $lnk->render());		
		
		parent::__construct("", "tableFilterDiv");
	}
	public function setPaging($perPage = 50, $showFirstLast = true)
	{
		$this->pagingData[perPage] = $perPage;
		$this->pagingData[showFirstLast] = $showFirstLast;
		$this->pagingData[totalCount] = 0;
		$this->pagingData[startPos] = 0;		
	}
	public function addSortFilter($dbColumn, $domObject='', $defaultSort = "")
	{
		// domObject can be empty - hidden sort
		// defaultSort can be "asc" or "desc" to set the initial sort
		
		$arr = array("selector" => $domObject, "dbColumn"=>$dbColumn);
		if ($defaultSort) $arr[direction] = $defaultSort;
		
		$this->sortFilters[] = $arr;
		
	}
	public function addColumnFilter($dbColumn, $filterType, $selector, $default='', $filterQuery='', $mask='')
	{
		$ctlColumn = "filters[$dbColumn]";
		$this->columnFilters[] = array("dbColumn" => $dbColumn, "filterType" => $filterType, "defaultVal" => $default, "value" => "", "selector" => $selector, "mask" => $mask);
		
		switch($filterType)
		{
			case tableFilter::filter_text_having:
			case tableFilter::filter_text:
				$ctl = new phpTextBox($ctlColumn, $default, 10, 50,"filter");
				break;
			case tableFilter::filter_date:
				$ctl = new phpFiller();				
				break;
			case tableFilter::filter_dropdown:
				$d = new DAL(true);
				
				$ctl = new phpDDL($ctlColumn);
				$ctl->class = 'filter';
				$ctl->selected = $default;				
				
				if (is_array($filterQuery))
				{
					$ctl->bindByArray($filterQuery);	
				}
				else
				{
					$ctl->bind($filterQuery);
				}
				
				break;
		}
		
		$ctl->style = "display: none;";
		$this->addControl($ctl);
		
		return $ctl;
	}
	public function addCustomFilter($key, $filterType, $label, $filterQuery='')
	{
		$ctlColumn = "custom[$key]";
		$this->customFilters[] = array("key" => $key, "filterType" => $filterType, "label" => $label, "value" => "");
		
		switch($filterType)
		{
			case tableFilter::filter_text_having:
			case tableFilter::filter_text:
				$ctl = new phpTextBox($ctlColumn, "", 25, 50,"filter");
				if ($label) $ctl->setLabel($label);
				break;
			case tableFilter::filter_date:
				$ctl = new phpFiller();				
				break;
			case tableFilter::filter_date_range:
				$ctl = new pageContent();
				
				$dCtl1 = new jqUI_datepicker("dates[{$key}_1]", "", $label);
				$dCtl1->setOption("minDate", "");
					
				$dCtl2 = new jqUI_datepicker("dates[{$key}_2]", "", "and");
				$dCtl2->setOption("minDate", "");
				$dCtl2->label->addClass('range');				
				$dCtl2->container->addClass("between");
				
				$ctl->addControl($dCtl1);
				$ctl->addControl($dCtl2);
				
				break;
			case tableFilter::filter_dropdown:				
				$ctl = new phpDDL($ctlColumn);
				$ctl->class = 'filter';
				
				if ($label) $ctl->setLabel($label);
				
				if (is_array($filterQuery))
				{
					$ctl->bindByArray($filterQuery);	
				}
				elseif ($filterQuery)
				{
					$ctl->bind($filterQuery);
				}
				
				break;
		}		
		
		$this->addControl($ctl);
		
		return $ctl;
	}		
	public function render()
	{
		$dvRight = new phpDIV();
		$dvRight->class = 'filterRight';
		
		$dvRight->addControl($this->filterButton);
		
		if ($this->showPager)
		{
			$ddlPager = new phpDDL("pager_{$this->id}");
			$ddlPager->class = "pager";
			$ddlPager->setLabel("Per Page:");
			$ddlPager->selected = $this->pagingData[perPage];
			$ddlPager->addListItem(new phpListItem(25, 25));
			$ddlPager->addListItem(new phpListItem(50, 50));
			$ddlPager->addListItem(new phpListItem(75, 75));
			$ddlPager->addListItem(new phpListItem(100, 100));
			$ddlPager->addListItem(new phpListItem(200, 200));			
			
			$dvRight->addControl($ddlPager);
		}
				
		$this->insertControl($dvRight);
		
		$dvFilters = new phpDIV();		
		$dvFilters->controls = $this->controls;
		
		$this->controls = array();
		
		if (!count($this->customFilters) && !$this->showPager) $this->style .= "display: none;";
		
		$this->addControl($this->header);
		$this->addControl($dvFilters);			
		
		if (!$this->loaderDeferred)
		{
			$loader = new jqLoader($this->getLoader());		
			$this->postContent = $loader->render();
		}
		
		return parent::render();	
	}
	public function getLoader()
	{
		$params = json_encode($this->params);
		$sortCols = json_encode($this->sortFilters);
		$filterCols = json_encode($this->columnFilters);
		$customCols = json_encode($this->customFilters);
		$paging = json_encode($this->pagingData);
		
		$this->autoRun = (bool)$this->autoRun ? 1 : 0;
		
		return "$('#{$this->id}').filterHelper({
			tableSelector: '$this->tableSelector',
			sortColumns: $sortCols,
			jsMethodParams: $params,
			filterColumns: $filterCols,
			customColumns: $customCols,
			jsMethod: $this->jsMethod,
			paging: $paging,
			autoRun: $this->autoRun,
			autoHide: $this->autoHide,
			showFilteredColumns: $this->showFilteredColumns			
		});";
	}
	public static function getWhereArray($filterColumns, &$wh = array(), &$having = array())
	{
		foreach((array)$filterColumns as $filter)
		{			
			if (!$filter[value]) continue;
			
			if ($filter[filterType] == tableFilter::filter_text_having) 
			{
				$valArr =& $having;
			}
			else
			{
				$valArr =& $wh;	
			}
			
			switch($filter[filterType])
			{
				case tableFilter::filter_dropdown:
				case tableFilter::filter_date:
					$valArr[] = "$filter[dbColumn] = '{$filter["value"]}'";
					break;
				default:
					$valArr[] = "$filter[dbColumn] like '%{$filter["value"]}%'";
					break;
			}				
		}
		
		return $wh;
	}
	public static function getWhereClause($whArr, $andPrefix = true)
	{
		return is_array($whArr) && count($whArr) ? ($andPrefix ? " and " : "").implode(" and ", $whArr) : "";
	}
	public static function getHavingClause($havingArr)
	{
		return is_array($havingArr) && count($havingArr) ? "\nHAVING ".implode(" and ", $havingArr) : "";
	}
	public static function getOrderClause($sortData, $default="id asc")
	{
		if ($sortData[dbColumn])
		{
			$orderCl = "$sortData[dbColumn] $sortData[direction]";			
		}
		else
		{
			$orderCl = $default;	
		}	
		
		return $orderCl;
	}
	public static function getLimitClause($pagingData)
	{
		validation::ensureInt($pagingData[startPos], 0);
		validation::ensureInt($pagingData[perPage], 0);
		
		if (!$pagingData[startPos] && !$pagingData[perPage]) return;

		return "\nlimit $pagingData[startPos], $pagingData[perPage]";
	}
	public static function getDateBetweenClause($column, $filterValue)
	{
		$tmp = explode("_", $filterValue);
		if (count($tmp) != 2) return "";
					
		$dt1 = validation::convertToMySqlDate($tmp[0]);
		$dt2 = validation::convertToMySqlDate($tmp[1]);

		return "date($column) between '$dt1' and '$dt2'";
	}
}
class jqPopupMenu extends phpDIV 
{
	public $button;
	public $ul;
	
	public function __construct($iconClass="ui-icon-gear")
	{
		$this->button = new jqUI_button("", "", "jqPopupButton", $iconClass, true);
		$this->ul = new phpUnOrderedList();
		
		parent::__construct("", "jqPopupMenu");			
	}
	public function addMenuItem($url, $caption, $class="", $iconClass="")
	{
		if (!$url) $url = "javascript:void(0)";
		
		$lnk = new jqUI_hyperlink($url, $caption, $class, $iconClass);

		$li = new htmlListItem();
		$li->addControl($lnk);
		
		$this->ul->addListItem($li);
		
		return $lnk;
	}
	public function render()
	{
		$this->addControl($this->button);
		$this->addControl($this->ul);
		
		return parent::render();	
	}
}
class jqUI_tabControl extends phpDIV
{
	public $menu;
	public $tabs = array();
	public $options;
	public $anchorLocation;
	
	public function __construct($id, $selectedIndex = 0, $anchorLocation="")
	{
		parent::__construct("", "ui_tabContainer");
		
		$this->id = $id;
		$this->anchorLocation = $anchorLocation;
		
		$this->options = new json_a();
		$this->options->autoQuote = false;		
		
		$this->options->active = 0;
		
		// Use this to prevent the retarded behaviour that the UI tabs present when using base tags (http://bugs.jqueryui.com/ticket/7822)
		$this->options->beforeLoad = "function( event, ui ) { event.preventDefault(); return; }";
	}
	public function addTabPage(jqUI_tabPage $page)
	{
		$this->tabs[] = $page;	
	}
	public function render()
	{
		$ul = new phpUnOrderedList();
		
		foreach($this->tabs as $tab)
		{
			if ($this->anchorLocation) $tab->link->href = $this->anchorLocation.$tab->link->href;
			$ul->addListItem(new htmlListItem($tab->link->render()));

			$this->addControl($tab);
		}
		
		$this->insertControl($ul);
		
		$this->addControl(new jqLoader("$('#{$this->id}').tabs({$this->options->render()})"));	

		return parent::render();
	}
}
class jqUI_tabPage extends phpDIV
{
	public $link;
	
	public function __construct($id, $caption, $iconClass="")
	{		
		parent::__construct("", "ui_tabPage");
		
		$this->id = $id;
		$this->link = new jqUI_hyperlink("#$id", $caption, "", $iconClass);
		//$this->link = new phpHyperlink("#$id", $caption);
	}
}
?>
