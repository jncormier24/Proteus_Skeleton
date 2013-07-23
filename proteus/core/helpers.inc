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
		if (!file_exists($file)) throw new Exception("No file found to query: $file");
		
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
		$destLoc = $destinationImage ? $config["baseAppDir"].$destinationImage : $imgLoc;
		
		$imType = imageHelper::getExtensionByMIME($imgLoc);
		
		// Save the original specified width/height
		$o_width = $width;
		$o_height = $height;
	
		$im = imageHelper::getImageObject($imgLoc);		
		if (!$im) throw new Exception("Unable to obtain image object - ensure file is a .gif or a .jpg and try again");	
	
		$iObj = imagecreatetruecolor($o_width, $o_height);
		imagefill($iObj, 0, 0, 0);
		
		$imgSize = getimagesize($imgLoc);		

		$srcWidth = $imgSize[0];
		$srcHeight = $imgSize[1];
		
		$xRatio = ($srcWidth > $width ? $srcWidth/$width : $width/$srcWidth);
		$yRatio = ($srcWidth > $height ? $srcHeight/$height : $height/$srcHeight);
	
		$src_x = 0;
		$src_y = 0;
		
		$dst_x = 0;
		$dst_y = 0;
	
		// Destination ratio
		$dst_ratio = ($width > $height ? $width / $height : $height / $width);
	
		// Source ratio
		$ratio = ($srcWidth > $srcHeight ? $srcWidth / $srcHeight : $srcHeight / $srcWidth);
	
		/* $src_width = $imgSize[0];
		$src_height = $imgSize[1]; */
		
		$adjHeight = ($srcWidth < $srcHeight ? ($width*$ratio) : ($width/$ratio));
		$adjWidth = ($srcHeight < $srcWidth ? ($width*$ratio) : ($width/$ratio));

		switch($aspect)
		{
			case 0:
				// Clip - no ratio consideration, clip to closest boundaries				
				if ($adjHeight > $height)
				{					
					$diff = ($adjHeight - $height);
					$src_y = $diff / 2;
					
					//$height -= $diff;
				}
				else
				{
					$diff = ($height - $adjHeight);
					
					$height -= $diff;
					$dst_y = $diff / 2;
				}
				
				imagecopyresampled($iObj, $im, $dst_x, $dst_y, $src_x, $src_y, $width, $height, $srcWidth, $srcHeight);
				
				break;
			case 1:
				// Retain
				if ($width > $srcWidth)
				{
					// Specified width is greater; offset the destination x coordinate
					$dst_x = ($width - $srcWidth) / 2;
				}
				else
				{
					// Specified width is less; offset the source x coordinate
					$src_x = ($srcWidth - $width) / 2;
				}
				
				if ($height > $srcHeight)
				{
					// Specified height is greater; offset the destination y coordinate
					$dst_y = ($height - $srcHeight) / 2;
				}
				else
				{
					// Specified height is less; offset the source y coordinate
					$src_y = ($srcHeight - $height) / 2;
				}
				
				imagecopy($iObj, $im, $dst_x, $dst_y, $src_x, $src_y, $srcWidth, $srcHeight);
				
				break;
			case 2:
				// Stretch				
				imagecopyresampled($iObj, $im, $dst_x, $dst_y, $src_x, $src_y, $width, $height, $srcWidth, $srcHeight);
				
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
	
		/* $dst_width = round($width);
		$dst_height = round($height); */			
	
		/* $dst_x = ($o_width / 2) - ($dst_width/2);
		$dst_y = ($o_height / 2) - ($dst_height/2);

		if ($dst_x < 0) $dst_x = 0;
		if ($dst_y < 0) $dst_y = 0;	 */
	
		//imagecopyresampled($iObj, $im, $dst_x, $dst_y, $src_x, $src_y, $dst_width, $dst_height, $src_width, $src_height);	
	
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
		switch(imageHelper::getExtensionByMIME($fileName))
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