/* JQuery UI Thickbox
 * 
 * Built off of Codey Lindey's Thickbox flow model, but utilizing the JQuery UI Framework for the actual dialog window
 * 
 */

var tb_pathToImage = "themes/controls/thickbox/loadingAnimation.gif";
var qry;

//on page load call tb_init
$(document).ready(function()
{   
	/*$(".ui-widget-overlay").click(function()
	{
		$(".uitb_window").dialog('close');
	});*/
	
	ui_tb_init('.uithickbox, a.thickbox, area.thickbox, input.thickbox');//pass where to apply thickbox
	
	imgLoader = new Image();// preload image
	imgLoader.src = tb_pathToImage;
});

//add thickbox to href & area elements that have a class of .thickbox
function ui_tb_init(domChunk)
{
	$(document).on("click", domChunk, function()
	{
		var t = this.title || this.name || null;
		var a = this.href || this.alt || this.src || $(this).attr("alt");
		var g = this.rel || false;
		
		ui_tb_show(t,a,g);
		
		this.blur();
		
		return false;
	});
}
function ui_tb_show(caption, url, group)
{
	var content;
	var baseURL;
	var elem;
	var winWidth = 0;
	var winHeight = 0;
	
	var elem = $("<div class='ui-widget-overlay'>&nbsp;</div>").css("z-index", 500);
	elem.click(function() { $(this).remove(); });
	
	$("body").append(elem);
	
	if(url.indexOf("?")!==-1)
	{ //ff there is a query string involved
		baseURL = url.substr(0, url.indexOf("?"));
	}
	else
	{ 
		baseURL = url;
	}
	
	var urlString = /\.jpg$|\.jpeg$|\.png$|\.gif$|\.bmp$/;
	var urlType = baseURL.toLowerCase().match(urlString);

	var dvName = "jqui_thickbox_" + Math.round(Math.random()*1000);
	$("body").append("<div class='uitb_window' id='" + dvName + "'></div>");
	
	if(urlType == '.jpg' || urlType == '.jpeg' || urlType == '.png' || urlType == '.gif' || urlType == '.bmp')
	{	
		//content = "<img src='" + baseURL + "' alt='" + caption + "' />";
		imgPreloader = new Image();
		imgPreloader.onload = function()
		{		
			imgPreloader.onload = null;
				
			// Resizing large images - orginal by Christian Montoya edited by me.
			var pagesize = tb_getPageSize();
			var x = pagesize[0] - 150;
			var y = pagesize[1] - 150;
			var imageWidth = imgPreloader.width;
			var imageHeight = imgPreloader.height;
			
			if (imageWidth > x) 
			{
				imageHeight = imageHeight * (x / imageWidth); 
				imageWidth = x;
				
				if (imageHeight > y) 
				{ 
					imageWidth = imageWidth * (y / imageHeight); 
					imageHeight = y; 
				}
			} 
			else if (imageHeight > y) 
			{ 
				imageWidth = imageWidth * (y / imageHeight); 
				imageHeight = y;
				
				if (imageWidth > x) 
				{ 
					imageHeight = imageHeight * (x / imageWidth); 
					imageWidth = x;
				}
			}
			// End Resizing
			
			winWidth = imageWidth + 30;
			winHeight = imageHeight + 120;
			
			$("#"+dvName).append("<img src='" + url + "' alt='" + caption + "' width=" + imageWidth + " height=" + imageHeight + " />");
			init_dialog(dvName, caption, winWidth, winHeight, false);
		}
		
		imgPreloader.src = url;
	}	
	else
	{		
		qry = tb_parseQuery(url.replace(/^[^\?]+\??/,''));
		
		if (qry['TB_iframe'])
		{
			urlNoQuery = url.split('TB_');
			
			var frameWidth = (qry['width']*1);
			var frameHeight = (qry['height']*1);
			
			$("#"+dvName+" p").replaceWith("<iframe frameborder='0' hspace='0' src='"+urlNoQuery[0]+"' id='uitb_iframe' name='uitb_iframe' style='width:100%; min-height:25px'> </iframe>");
			
			winWidth = frameWidth + 2;
			winHeight = frameHeight + 80;
			
			init_dialog(dvName, caption, winWidth, winHeight, true);
		}
		else
		{
			var frameWidth = (qry['width']*1);
			var frameHeight = (qry['height']*1);
			
			$("#"+dvName+" p").html($("#"+qry['TB_elem']).html()).css({padding: "0px 25px 0px 0px", textAlign: "justify"});
			
			init_dialog(dvName, caption, frameWidth, frameHeight, false);
		}
	}
}	
function init_dialog(dvName, caption, winWidth, winHeight, iFrame)
{
	$(".ui-widget-overlay").remove();
	
	/*buttons: qry['noClose'] == 1 ? {} : 
	{
		Close: function()
		{
			$(this).dialog('close');
		}
	},*/
	//.css({minWidth: winWidth, minHeight: winHeight})
	$("#"+dvName).dialog(
	{			
		modal: true,
		draggable: true,
		autoResize: true,
		width: winWidth,		
		title: caption,
		buttons:  
		{
			Close: function()
			{
				$(this).dialog('close');
			}
		},
		close: function()
		{
			$(this).remove();
			ui_tb_closeCallBack();
		},
		open: function(event, ui)
		{
			$("iframe", this).css({height: "100%", width: winWidth});			
		},
		resizeStop: function(event, ui)
		{		
			$("iframe", this).css({height: "100%", width: ui.size.width});
		}		
	});
	
	if (iFrame) 
	{
		$("#"+dvName).css({overflow: 'hidden', padding: '0px'});
		$(".ui-dialog-buttonpane").css("margin", "0px");
	}
	
	//$(".ui-widget-overlay").css("background-image","none");	
}
function ui_tb_remove()
{
	$(".uitb_window").dialog('close');
}
function ui_tb_closeCallBack() {}

function tb_getPageSize()
{
	var de = document.documentElement;
	var w = window.innerWidth || self.innerWidth || (de&&de.clientWidth) || document.body.clientWidth;
	var h = window.innerHeight || self.innerHeight || (de&&de.clientHeight) || document.body.clientHeight;
	
	arrayPageSize = [w,h];
	
	return arrayPageSize;
}
function tb_parseQuery ( query ) 
{
   var Params = {};
   if (!query) return Params;// return empty object
   
   var Pairs = query.split(/[;&]/);
   
   for (var i = 0; i < Pairs.length; i++) 
   {
      var KeyVal = Pairs[i].split('=');
      
      if (!KeyVal || KeyVal.length != 2) continue;
      
      var key = unescape(KeyVal[0]);
      var val = unescape(KeyVal[1]);
      val = val.replace(/\+/g, ' ');
      
      Params[key] = val;
   }
   
   return Params;
}