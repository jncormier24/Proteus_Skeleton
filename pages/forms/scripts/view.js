function submitForm()
{
	var frm = $("#customForm");
	
	$.showLoading("Submitting Form");
	
	$.post("ajax/forms", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$(output.content).appendTo("body");
		}
		else
		{
			$(".invalidField").removeClass("invalidField");
			
			if (output.error_fields)
			{
				var eArr = output.error_fields.split(",");
						
				for( var key in eArr)
				{
					$("[name='fields[" + eArr[key] + "]']").addClass("invalidField");
				}
			}
		}
	});
}
function submissionComplete_callback()
{	
	window.location = "index";
}