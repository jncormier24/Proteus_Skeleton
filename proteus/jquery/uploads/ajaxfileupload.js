
jQuery.extend({    
    ajaxFileUpload: function(s) 
    {   		
        s = $.extend({}, $.ajaxSettings, s);
        
        var id = new Date().getTime()        
        
        //create form	
		var formId = 'jUploadForm' + id;
		var fileId = 'jUploadFile' + id;
		
		var form = $('<form class="jqUpload" method="POST" id="' + formId + '" enctype="multipart/form-data"></form>');
		var element = $('#' + s.fileElementId);
				
		//set attributes
		form.css('position', 'absolute');
		form.css('top', '-1200px');
		form.css('left', '-1200px');
		form.appendTo('body');		
		
		var orig = element.clone();
		orig.attr("id", "elem"+id);
		
		element.before(orig);		
		element.appendTo(form);
		
		form.ajaxSubmit(
    	{   
    		url: s.url,
    		beforeSubmit: s.beforeSubmit ? s.beforeSubmit : null,
    		data: s.data ? s.data : null,
    		success: function(data)
    		{    			
    			data = $.parseJSON(data.substring(data.indexOf("{"), data.lastIndexOf("}") + 1));
    			
    			var nID = element.attr("id");
    			form.remove();
    			element.remove();
    			    			
    			orig.attr("id", nID);
    			
    			if (typeof data != "undefined")
    			{
    				if (data == null) data = {};
    				s.success(data);
    			}
    			else
    			{
    				alert("No content returned - possible malformed request.");
    			}
    		}    		
    	});
		
		return form;        
    }
});