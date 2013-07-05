function validateLogin(ref)
{
	$.showLoading("Validating Login Credentials");
	
	$.getJSON("admin/ajax/login", $("#frmLogin").serialize(), null, "json").then(function(output)
	{
		$("#password").val("");
		
		if (!$.ajaxError(output, $))
		{
			window.location = ref;			
		}
	});
}