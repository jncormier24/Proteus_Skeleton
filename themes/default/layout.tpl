<!DOCTYPE html>
<html>
<head>
	<title>{TITLE}</title>
 	<BASE href="{scripturl}">
 	
 	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
 	<meta name="robots" content="index,follow" /> 	
 	
 	{HEAD_OBJECTS}
 	
 	<link rel="stylesheet" type="text/css" href="themes/default/style.css" />
 	
 	{STYLESHEET_OBJECTS}
 	 	
 	<link rel="contents" href="index.php" title="{TITLE}" /> 	 	
 	<link rel="shortcut icon" href="{scripturl}favicon.ico" /> 	 	

 	{SCRIPT_OBJECTS} 	
</head>
<body>
	<div id="container">
		{HEADER_CONTENT}
		{NAV_CONTENT}
		<div id="mainContent">{PAGE_CONTENT}</div>
		{FOOTER_CONTENT}	
	</div>		
	{POST_SCRIPT_OBJECTS}
</body>
</html>