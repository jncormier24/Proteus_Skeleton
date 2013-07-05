function getUsers()
{		
	var cont = $("#users");
	var filterObj = $("#usersFilter");	
	var ajaxOpts = { action: "getUsers" };	
	
	cont.showLoading("Loading System Users");	
	
	$.getJSON("admin/ajax/users", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{		
			cont.html(output.content).tableRowAlternate();
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		}
	});	
}
function showEditUser(userID)
{
	var dfd = new $.Deferred();
	
	$.showLoading("Loading User Edit Interface");
	
	return $.getJSON("admin/ajax/users", {action: "showEditUser", userID: userID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
			
			$("[name='details[dob]']").datepicker("option", {
				changeMonth: true, 
				changeYear: true,
				yearRange: "-80:-10"});
			
			if (userID)
			{				
				$("#userForm").updateHelper(function() { return updateUser(); }, {autoSave: false, disableControls: output.disableEdit});
			}
				
			dfd.resolve();
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd;
}
function updateUser()
{
	var cont = $("#userWin");
	var cb = cont.data("save_cb");
	var dfd = $.Deferred();
	
	cont.showLoading("Saving");
	
	$.post("admin/ajax/users", $("#userForm").serializeArray(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{				
			// Callback functionality
			if (cb)
			{
				cb(output.userID);
			}
			else
			{
				getMessages();
				getUsers();	
			}
			
			cont.dialog('close');
			
			dfd.resolve();
		}
		else
		{
			// Update failed, mark the deferred as rejected so the save icon doesn't disappear (reset)
			dfd.reject();
		}
	});
	
	return dfd.promise();
}
function getPermissions(userID)
{
	var cont = $("#permissions > div");
	$.showLoading("Loading Permissions Interface");
	
	$.getJSON("admin/ajax/users", {action: "getPermissions", userID: userID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content);
		}
	});			
}
/************ Permissions ***************/
function savePermissions(userID, ctl)
{
	var assnID;
	var cont = $("#permissionWin");
	cont.showLoading("Saving Assignment");
	
	$(ctl).closest("tr").attr("dirty", 1).addClass('warnRow');
	
	var perms = new Object;
	var ct = 0;
	
	$("tr[dirty=1]").each(function()
	{
		var permKey = $(this).attr("data-permkey");		
		var bitVal = 0;
		
		assnID = $(this).attr("data-assnid");
		
		$("input:checked", this).each(function()
		{
			bitVal += parseInt($(this).val());
		});
		
		perms[permKey] = bitVal;
		
		ct++;
	});
	
	if (!ct) return;		
	
	$.post("admin/ajax/users", {action: "savePermissions", "perms[]": perms, userID: userID, assnID: assnID}, null, "json").then(function(output)		
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$("tr").removeClass("warnRow").attr("dirty", 0);
		getMessages();
		
		if (output.reload)
		{
			cont.dialog('close');
			getPermissions(userID);
		}
		
	});	 
}
function addSecurityAssn(userID, matrixID)
{
	var cont = $("#permissionWin");
	cont.showLoading("Adding Security Assignment");
	
	$.post("admin/ajax/users", {
		action: "addSecurityAssn", 
		userID: userID, 
		matrixID: (matrixID ? matrixID : $("#accessSec").val()), 
		value: (matrixID ? $("#menuSec").val() : "") 
	}, null, 'json').then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		
		cont.dialog('close');
		
		getPermissions(userID);		
	});		
}
function deleteSecurityAssn(keyID)
{
	var cont = $("#permissionWin");
	cont.showLoading("Removing Security Assignment");
	
	$.getJSON("admin/ajax/users", {action: "deleteSecurityAssn", keyID: keyID}).then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		
		cont.dialog('close');
		
		getPermissions(output.userID);		
	});			
}
function addMenuAccess(userID)
{
	addSecurityAssn(userID, 2);			
}
function getLinkedOptions()
{
	var cont = $("#permissionWin");
	cont.showLoading("Loading Available Options");
	
	$.getJSON("admin/ajax/users", {action: "getLinkedOptions", permKey: $("#permKey").val()}).then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$("#permOptions").html(output.content);		
		//$("#btnAddSpec").css("display", (output.content ? "block" : "none"));
	});
}
function addPermissionEntry(userID)
{
	var cont = $("#permissionWin");
	cont.showLoading("Adding permission entry");
	
	$.getJSON("admin/ajax/users", {action: "addPermissionEntry", userID: userID, permissionKey: $("#permKey").val(), dataID: $("#linkData").val()}).then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		cont.dialog('close');
		
		getPermissions(userID);
	});
}
function removeUser(userID)
{
	$.jqConfirm("Are you sure you want to deactivate this user?", function()
	{	
		$.showLoading("Removing System User");
		
		$.getJSON("admin/ajax/users", {action: "removeUser", userID: userID}).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getUsers();		
		});
	});
}