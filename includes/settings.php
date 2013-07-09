<?php 

	// Set the working folder for the application. For root installation, set as just /
	// This allows the framework to be installed in subdirectories as needed
	$config["application_root"] = "Proteus_Skeleton/";
	
	$config["db_user"] = "mstech"; 
	$config["db_pass"] = "n0th1ng";
	$config["db_host"] = "mstech-db.my.phpcloud.com";
	$config["db_name"] = "mstech";
	
	$config["server_timezone"] = "America/New_York";
	
	$config["theme"] = "default";
	$config["jsrev"] = 1;
	$config["auto_include_folders"] = Array("scripts"=>Array("js"), "styles"=>Array("css"));
	
	// Define the permission class used for this site. To implement custom security options, use the bundled "permissions" as a template 
	// and extend from that class.
	$config["permission_class"] = "permissions";
	
	// Set this to true to issue technical errors instead of generic. This is also required for other forms of error reporting
	// that is typically suppressed.
	$config["debug_mode"] = true;
	
	// Setting this value to something other than blank will issue an email whenever a database error of any kind occurs
	$config["debug_email_address"] = "sbyers@mstech.com";

	// Turn off the cache system-wide. To turn off cache on a particular page, add a noCache=1 to the Query String (can be disabled below)
	$config["cache_disable"] = false;
	
	// Setting this to True will allow disabling the cache using the Query String parameter above, setting to false will prevent 
	// the override and always retrieve cached data if cache_disable is set to false.
	// RECOMMENDED: When in production mode, set this to False for performance as any external links to a URL with that parameter in it
	// will always be fetching fresh data and could cause slowdown under heavy load
	$config["cache_allow_override"] = true;
	
	// Set the site to use zlib output compression; should be set to true in most installation cases
	$config["use_zlib_compression"] = true;
	
	// Set the custom error handler to be shown whenever an error occurs, or when calling the error() function from code
	$config["custom_error_handler"] = "error_handler";
	
	// Set the number of days to retain custom session data.
	$config["session_retention_days"] = "3";
?>