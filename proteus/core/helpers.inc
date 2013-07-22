<?php 
class fileHelper
{
	public static function getFileExtension($file)
	{
		$parts = explode(".", $file);
		return strtolower($parts[count($parts)-1]);
	}	
	public static function getMimeType($file)
	{		
		$finfo = finfo_open(FILEINFO_MIME_TYPE);
		$mime = finfo_file($finfo, $file);
		
		return $mime;
	}
}
class imageHelper
{
	const aspect_clip = 0;
	const aspect_retain = 1;
	const aspect_stretch = 2;
	const aspect_dynamic = 3;
	
	public static function scaleImage($sourceImage, $width, $height, $aspect, $destinationImage="")
	{
		global $config;	
	
		$imgLoc = $config["baseAppDir"].$sourceImage;
		$destLoc = $destFolder ? $destFolder : $imgLoc;
		
		$imType = imageHelper::getExtensionByMIME($imgLoc);
		
		// Save the original specified width/height
		$o_width = $width;
		$o_height = $height;
	
		$im = imageHelper::getImageObject($imgLoc);		
		if (!$im) throw new Exception("Unable to obtain image object - ensure file is a .gif or a .jpg and try again");	
	
		$imgSize = getimagesize($imgLoc);		

		$xRatio = ($imgSize[0] > $width ? $imgSize[0]/$width : $width/$imgSize[0]);
		$yRatio = ($imgSize[1] > $height ? $imgSize[1]/$height : $height/$imgSize[1]);
	
		$src_x = 0;
		$src_y = 0;
	
		// Destination ratio
		$dst_ratio = ($width > $height ? $width / $height : $height / $width);
	
		// Source ratio
		$ratio = ($imgSize[0] > $imgSize[1] ? $imgSize[0] / $imgSize[1] : $imgSize[1] / $imgSize[0]);
	
		$src_width = $imgSize[0];
		$src_height = $imgSize[1];
		
		$adjHeight = ($imgSize[0] < $imgSize[1] ? ($width*$ratio) : ($width/$ratio));

		switch($aspect)
		{
			case 0:
				// Clip
				if ($adjHeight < $height)
				{		
					$diff = $height - $adjHeight;

					$src_width = $src_width - ($diff*$xRatio);
					
					//Adjust position to 1/2 the difference in new width
					$src_x = ($imgSize[0] - $src_width) / 2;					
				}
				else
				{
					$yRatio = $imgSize[1]/$adjHeight;
					
					$diff = $adjHeight - $height;
					$src_height = $src_height - ($diff*$yRatio);
					
					$src_y = $imgSize[1] * .15;
					
					if (($src_y + $src_height) > $imgSize[1])
					{
						$src_y = ($src_y + $src_height) - $imgSize[1];
					}
				}
				
				break;
			case 1:
				// Retain
				if ($adjHeight > $height)
				{
					$diff = $adjHeight - $height;					
					
					$width = floor(($diff*$xRatio)/$ratio);
					
					$src_y = ($imgSize[1]/2) - ($src_height/2);
				}
				else
				{		
					$height = $adjHeight;					
				}
				
				break;
			case 2:
				// Stretch
				$src_width = $width;
				$src_height = $height;
				
				break;
				
			case 3:
				$width = $imgSize[0];
				$height = $imgSize[1];
				
				$wRatio = $width / $height;
				$hRatio = $height / $width;
				
				// Make 2 passes at most to attempt a ratio reduction within dynamic boundaries
				$ct = 0;
		
				while(($width > $o_width || $height > $o_height) && ++$ct < 2)
				{			
					if ($width > $o_width)
					{
						$width = $o_width;
						$height = $width * $hRatio;	
					}
					
					if ($height > $o_height)
					{
						$height = $o_height;
						$width = $height * $wRatio;	
					}
				}
				
				$o_width = $width;
				$o_height = $height;
				
				break;
				
			default:
				throw new Exception("Invalid aspect specified.");
				
		}
	
		$dst_width = round($width);
		$dst_height = round($height);	
	
		$iObj = imagecreatetruecolor($o_width, $o_height);
		imagefill($iObj, 0, 0, 0);	
	
		$dst_x = ($o_width / 2) - ($dst_width/2);
		$dst_y = ($o_height / 2) - ($dst_height/2);

		if ($dst_x < 0) $dst_x = 0;
		if ($dst_y < 0) $dst_y = 0;	
	
		imagecopyresampled($iObj, $im, $dst_x, $dst_y, $src_x, $src_y, $dst_width, $dst_height, $src_width, $src_height);	
	
		switch($imType)
		{
			case "jpg":							
			case "jpeg":
				imagejpeg($iObj, $destLoc, 95);
				break;
			case "gif":
				imagegif($iObj, $destLoc, 95);
				break;
			case "png":			
				imagepng($iObj, $destLoc);
				break;
			default:
				throw new Exception("Invalid image type for scaling ($imType)");
		}
		
		imagedestroy($im);

		return true;
	}
	public static function getImageObject($fileName)
	{	
		switch(imageHelper::extensionByMIME($fileName))
		{
			case "jpg";
			case "jpeg":
				return imagecreatefromjpeg($fileName);
			case "gif":
				return imagecreatefromgif($fileName);				
			case "png":
				return imagecreatefrompng($fileName);			
		}
	}
	public static function getExtensionByMIME($file)
	{
		// This function is designed to get the MIME type of images ONLY
		$mime = fileHelper::getMimeType($file);
		
		switch($mime)
		{
			case "image/jpg":
			case "image/jpeg":
				return "jpg";
			case "image/png":
				return "png";
			case "image/bmp":
			case "image/x-windows-bmp":
			case "image/x-ms-bmp":
				return "bmp";
			case "image/gif":
				return "gif";
			default:
				return false;
		}
	}	
}
?>