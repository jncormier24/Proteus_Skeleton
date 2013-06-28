function validateLogin(ref)
{
	$.showLoading("Validating Login Credentials");
	
	$.getJSON("ajax/login", $("#frmLogin").serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			$(output.content).appendTo("body");			
		}
	});
}