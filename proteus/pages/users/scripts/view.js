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
				
				getPermissions(userID, true);
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
			
			dfd.resolve();
			
			if (parseInt(output.userID))
			{
				cont.dialog('close');
				showEditUser(output.userID);
			}
		}
		else
		{
			// Update failed, mark the deferred as rejected so the save icon doesn't disappear (reset)
			dfd.reject();
		}
	});
	
	return dfd.promise();
}
function getPermissions(userID, hideLoader)
{
	var cont = $("#permissions > div");
	if (!hideLoader) $.showLoading("Loading Permissions");
	
	$.getJSON("admin/ajax/users", {action: "getPermissions", userID: userID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content);
			
			checkPermissionRequisites();
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
		if (!$.ajaxError(output, cont))
		{		
			$("tr").removeClass("warnRow").attr("dirty", 0);
			getMessages();
		
			var parent = $("tr[data-permkey='" + output.setRead + "']");
			
			checkPermissionRequisites();
		}		
	});	 
}
function checkPermissionRequisites()
{
	$("#permissions table tr").has("input[type=checkbox]").each(function()
	{		
		var readCtl = $("input[type=checkbox]:eq(0)", this);
		var bitVal = 0;
		var obj = $(this);
		
		$("input:checked", this).each(function()
		{
			bitVal += parseInt($(this).val());
		});
		
		if (bitVal > 1)
		{
			readCtl.prop("checked", true).prop("disabled", true);
		}
		else
		{
			readCtl.prop("disabled", false);
		}	
		
		if (obj.attr("data-assnrow") && !readCtl.is(":checked")) obj.fadeOut();
	});
}
function addPermission(userID)
{
	var cont = $("#permissionWin");
	cont.showLoading("Adding Security Assignment");
	
	$.post("admin/ajax/users", {
		action: "addPermission", 
		userID: userID,
		permissionKey: $("#accessSec").val()	 
	}, null, 'json').then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{
			getMessages();
			
			cont.dialog('close');
			
			getPermissions(userID);	
		}		
	});		
}
function removePermission(assnID)
{
	$.jqConfirm("Are you sure you want to remove this permission assignment?", function()
	{
		var cont = $("#permissionWin");
		cont.showLoading("Removing Security Assignment");
		
		$.getJSON("admin/ajax/users", {action: "removePermission", assnID: assnID}).then(function(output)
		{
			if (!$.ajaxError(output, cont))
			{
				getMessages();
			
				cont.dialog('close');
			
				getPermissions(output.userID);
			}
		});
	});
}
function addMenuAccess(userID)
{
	addSecurityAssn(userID, 2);			
}
function getLinkedOptions(userID)
{
	var cont = $("#permissionWin");
	cont.showLoading("Loading Available Options");
	
	$.getJSON("admin/ajax/users", {action: "getLinkedOptions", permKey: $("#permKey").val(), userID: userID}).then(function(output)
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