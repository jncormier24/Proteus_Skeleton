function getBlogs()
{
	var cont = $("#blogCategories");
	var filterObj = $("#blogCatFilter");	
	var ajaxOpts = { action: "getBlogs" };	
	
	$.showLoading("Loading Blog Categories");	
	
	$.getJSON("admin/ajax/blogs", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate;
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		
			initTableSort();
		}
	});
}
function getBlogCategoryEdit(blogID)
{
	$.showLoading("Loading Blog Category Interface");
	
	$.getJSON("admin/ajax/blogs", {action: "getBlogCategoryEdit", blogID: blogID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{			
			$(output.content).appendTo("body");
			
			$.blockContainer();
			
			var win = $("#blogCategoryWin");
			
			// Need to inialized redactor before initializing the updateHelper to check for dirty controls (accesses Redactor method "getEditor")
			$("textarea.wysiwyg", win).assetRedactor(8, blogID, blogID);
			
			win.dialog("option", "close", function() 
			{  
				$.unblockContainer();						
				$(this).remove();
			}).updateHelper(updateBlogCategory, {closeConfirmOnly: true, autoSave: false});
		}
	});
}
function updateBlogCategory()
{	
	var frm = $("#blogForm");
	var blogID = $("#blogID", frm).val();
	
	$.showLoading(parseInt(blogID) ? "Saving Blog Category" : "Adding new Blog Category");
	
	$.post("admin/ajax/blogs", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			if (parseInt(output.blogID))
			{
				$("#blogCategoryWin").dialog("close");
				getBlogCategoryEdit(output.blogID);
			}
			
			getBlogs();		
			getMessages();			
		}
	});
}
function getBlogEntries(blogID)
{
	var cont = $("#blogEntries");
	var filterObj = $("#blogEntryFilter");	
	var ajaxOpts = { action: "getBlogEntries", blogID: blogID };	
	
	$.showLoading("Loading Blog Entries");	
	
	$.getJSON("admin/ajax/blogs", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{		
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		}
	});
}
function ajaxFileUpload(blogID)
{
	var _element = "fileToUpload";	
	
	$.showLoading("Uploading Blog Image");
	
	$.ajaxFileUpload(
	{
		url: 'admin/ajax/blogs?action=uploadBlogImage&blogID=' + blogID, 			
		secureuri:false,
		fileElementId: _element,
		dataType: 'json',
		success: function (data, status)
		{	
			$.hideLoading();
			$("#" + _element).fileinput();
			
			if (data.error)
			{
				alert(data.error);
			}
			else
			{						
				getMessages();			
				
				$("#btnClear").fadeIn();
				//Have to do this here... for some reason the uploading process doesn't like anything that isn't just plain text. UGH!
				$("#blogImage").replaceWith("<img id='blogImage' src='" + data.image + "?ts=" + new Date().getTime() + "' />");
			}			
		},
		error: function (data, status, e)
		{			
			alert(e);
			$.hideLoading();
			$("#" + _element).fileinput();
		}
	});
	
	return false;
}
function getLinkedTopics(entryID)
{
	var cont = $("#linked_topics");
	
	cont.setWorking();
	
	$.getJSON("admin/ajax/blogs", {action: "getLinkedTopics", entryID: entryID}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$("#" + cont.attr("data-tablink")).removeClass('tabLoading').find(".tabLoaderIcon").replaceWith("<span class='commentCount tabLoaderIcon'> (" + output.topicCount + ")</span>");
		cont.html(output.content);
	});
}
function getLinkedFeeds(entryID)
{
	var cont = $("#linked_feeds");
	
	cont.setWorking();
	
	$.getJSON("admin/ajax/blogs", {action: "getLinkedFeeds", entryID: entryID}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$("#" + cont.attr("data-tablink")).removeClass('tabLoading').find(".tabLoaderIcon").replaceWith("<span class='commentCount tabLoaderIcon'> (" + output.feedCount + ")</span>");
		cont.html(output.content);
	});
}
function checkLinkTopic()
{
	var txtBox = $("#newTopic");
	
	if ($("#linkTopic").prop("selectedIndex") == 0)
	{
		txtBox.fadeIn();
	}
	else
	{
		txtBox.fadeOut();
	}
}
function doLinkTopic(entryID)
{
	$.showLoading("Linking Blog Topic");
	
	$.post("admin/ajax/blogs", {
		action: "linkTopic", 
		entryID: entryID,
		topicID: $("#linkTopic").val(),
		customTopic: $("#newTopic").val()
	}, null, "json").then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getLinkedTopics(entryID);
	});
}
function unlinkTopic(linkID)
{
	$.jqConfirm("Are you sure you want to unlink this topic?", function()
	{
		$.showLoading("Removing Topic Link");
		
		$.getJSON("admin/ajax/blogs", {action: "unlinkTopic", linkID: linkID}).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getLinkedTopics(output.entryID);
		});
	});
}
function doLinkFeed(entryID)
{
	$.showLoading("Linking Blog Feed");
	
	$.post("admin/ajax/blogs", {
		action: "linkFeed", 
		entryID: entryID,
		feedID: $("#linkFeed").val()		
	}, null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
			getLinkedFeeds(entryID);
		}
	});
}
function unlinkFeed(linkID)
{
	$.jqConfirm("Are you sure you want to unlink this blog feed?", function()
	{
		$.showLoading("Removing Blog Feed Link");
		
		$.getJSON("admin/ajax/blogs", {action: "unlinkFeed", linkID: linkID}).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getLinkedFeeds(output.entryID);
		});
	});
}
function clearBlogIcon(blogID)
{
	$.jqConfirm("Are you sure you want to clear the icon image for this blog?", function()
	{	
		$.showLoading("Clearing Image");
		
		$.getJSON("admin/ajax/blogs", {action: "clearBlogIcon", blogID: blogID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{
				getMessages();
				getBlogs();
			}
		});
	});
}
function clearBlogEntryIcon(entryID)
{
	$.jqConfirm("Are you sure you want to clear the image for this blog entry?", function()
	{	
		$.showLoading("Clearing Image");
		
		$.getJSON("admin/ajax/blogs", {action: "clearBlogEntryIcon", entryID: entryID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{			
				getMessages();
				getBlogEntries();
			}
		});
	});
}
function generateTagCloud(entryID)
{
	if (!confirm('This will save the current blog and attempt to generate a Tag Cloud from all associated data elements. Are you sure you want to do this right now?')) return;
	
	saveBlogEntry(entryID, 0, true).then(function() 
	{ 	
		$.showLoading("Generating Tag Cloud");
		
		$.post("admin/ajax/blogs", {action: "generateTagCloud", entryID: entryID}, null, "json").then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{	
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getTagCloud(entryID);
		});
	});
}
function getTagCloud(entryID, hideLoading)
{
	var cont = $("#tag_cloud");
	
	if (!hideLoading) cont.setWorking();
	
	$.getJSON("admin/ajax/blogs",{action: "getTagCloud", entryID: entryID}).then(function(output)
	{
		if (!$.ajaxError(output, cont))
		{		
			cont.html(output.content);		
			if (parseInt(output.locked)) return;
		
			$("span.tag", cont).each(function()
			{
				$(this).hover(function()
				{
					var tagID = $(this).data("tagid");
					var spn = $("<span class='ui-icon ui-icon-close'></span>");
					
					spn.click(function(e)
					{
						removeTag(tagID);
						e.stopImmediatePropagation();
					});
					
					$(this).addClass('tagHover').append(spn);
				}, function()
				{
					$(this).removeClass('tagHover').find("span.ui-icon-close").remove();
				});
			});
		}
	});
}
function addTag(entryID)
{
	$.showLoading("Adding Tag");
	
	$.post("admin/ajax/blogs",{
		action: "addTag",
		entryID: entryID,
		tag: $("#txtTag").val(),
		weight: $("#tagWeight").val()
	},null,"json").then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{	
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		getTagCloud(entryID, true);
	});
}
function removeTag(tagID)
{
	$.jqConfirm("Are you sure you want to remove this Tag?", function()
	{
		$.showLoading("Removing Tag");
		
		$.getJSON("admin/ajax/blogs", {action: "removeTag", tagID: tagID}).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{	
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			getTagCloud(output.entryID, true);
		});
	});
}
function getBlogFeeds()
{
	var cont = $("#blogFeeds");
	var filterObj = $("#blogFeedFilter");	
	var ajaxOpts = { action: "getBlogFeeds" };	
	
	$.showLoading("Loading Blog Feeds");	
	
	$.getJSON("admin/ajax/blogs", filterObj.filterHelper("getFilterData", ajaxOpts)).then(function(output)
	{	
		if (!$.ajaxError(output, $))
		{
			cont.html(output.content).tableRowAlternate;
			
			initJqPopupMenus();
			
			filterObj.filterHelper("setPaging", { totalCount: output.count }).filterHelper("initAll");
		
			initTableSort();
		}
	});
}
function getBlogFeedEdit(feedID)
{
	var name = $("#feedTitle" + (feedID ? "" : "_new"));
	
	$.showLoading("Loading Blog Feed Edit Window");
	
	$.getJSON("admin/ajax/blogs", {action: "getBlogFeedEdit", feedID: feedID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			$(output.content).appendTo("body");
		}
	});			
}
function updateBlogFeed()
{
	var frm = $("#frmFeed");
	var feedID = $("#feedID", frm);
	
	$.showLoading(feedID ? "Saving Blog Feed" : "Adding Blog Feed");
	
	$.post("admin/ajax/blogs", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			$("#feedEditWin").dialog("close");
		
			getBlogFeeds();
		}
	});			
}
function deleteBlogFeed(feedID)
{	
	$.jqConfirm("Are you sure you want to remove this Blog Feed? This will only remove the feed container, no blog entries will be affected.", function()
	{
		$.showLoading("Removing Blog Feed");
		
		$.getJSON("admin/ajax/blogs", {action: "deleteBlogFeed", feedID: feedID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{		
				getMessages();		
				getBlogFeeds();
			}
		});
	});
}
function getBlogEntryEdit(entryID, blogID)
{
	var cont = $("#entriesWin");
	
	$.showLoading("Loading Blog Edit Interface");
	
	$.getJSON("admin/ajax/blogs", {action: "getBlogEntryEdit", entryID: entryID, blogID: blogID}).then(function(output)
	{
		if (!$.ajaxError(output, $))
		{	
			$.hideDialogs(true);
			
			$(output.content).appendTo("body");
			
			var win = $("#entryWin");
			var frm = $("#entryForm", win);
			
			$(".wysiwyg", frm).assetRedactor(-5, output.blogID, entryID)
			
			win.dialog("option", "close", function()
			{
				$.showDialogs(true);				
				$(this).remove();
			})
			
			frm.updateHelper(updateBlogEntry, {closeConfirmOnly: true, autoSave: false});
			
			if (entryID)
			{
				getLinkedFeeds(entryID);
				getLinkedTopics(entryID);
				getTagCloud(entryID);
				
				getComments(entryID, 1, "#blog_approved_comments");
				getComments(entryID, 1, "#blog_pending_comments");
			}
		}
	});
}
function updateBlogEntry()
{	
	var dfd = $.Deferred();
	
	var win = $("#entryWin");
	var frm = $("#entryForm", win);	
	var entryID = $("#entryID", frm).val();
	
	$.showLoading(entryID ? "Saving Blog Entry" : "Adding Blog Entry");
	
	$.post("admin/ajax/blogs", frm.serialize(), null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{		
			getMessages();
		
			frm.updateHelper("reset");
			
			if (!entryID)
			{
				win.dialog("close");				
				getBlogEntryEdit(output.entryID);
			}
			
			getBlogEntries();
			
			dfd.resolve();
		}
		else
		{
			dfd.reject();
		}
	});
	
	return dfd.promise();
}
function getBlogEntryWin(blogID)
{	
	$.showLoading("Loading Blog Entries");
	
	$.getJSON("admin/ajax/blogs", {action: "getBlogEntryWin", blogID: blogID}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
	});			
}
function deleteBlog(inactive, blogID)
{
	var action;
	var msg;
	
	if (inactive)
	{
		msg = "Are you SURE you want to permanently delete this blog and all blog entries? This cannot be undone!";
		action = "Removing";
	}
	else
	{
		msg = "Are you sure you want to deactivate this blog?";
		action = "Deactivating";		
	}
	
	$.jqConfirm(msg, function()
	{	
		$.showLoading(action + " Blog Category");
		
		$.getJSON("admin/ajax/blogs", {action: "deleteBlog", blogID: blogID}).then(function(output)
		{
			if (!$.ajaxError(output, $))
			{			
				getMessages();			
				getBlogs();
			}
		});
	});
}
function deleteBlogEntry(inactive, entryID)
{
	var msg;
	var action;
	var cont = $("#entriesWin");	
	
	if (inactive)
	{
		msg = "Are you SURE you want to permanently delete this Blog Entry all associated data? This cannot be undone!";
		action = "Removing";
	}
	else
	{
		msg = "Are you sure you want to deactivate this Blog Entry?";
		action = "Deactivating";		
	}
	
	$.showLoading(action + " Blog Entry");
	
	$.jqConfirm(msg, function()
	{
		$.getJSON("admin/ajax/blogs", {action: "deleteBlogEntry", entryID: entryID}).then(function(output)
		{
			cont.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}
			
			getMessages();
			
			cont.dialog('close');
			getBlogEntryWin(output.blogID);
		});
	});
}
function uploadBlogIcon(blogID)
{
	var _element = "blogCatIcon_" + blogID;
	
	$.showLoading("Uploading Blog Icon");
	
	$.ajaxFileUpload( {
		url : 'admin/ajax/blogs?action=uploadBlogIcon&blogID=' + blogID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			$.hideLoading();
			
			$("#" + _element).val("").fileinput();
			
			if (data.error)
			{
				$.jqAlert(data.error);
				return;
			}			
			
			getMessages();
			getBlogs();			
		},
		error : function(data, status, e)
		{
			$.hideLoading();
			alert(e);
		}
	});

	return false;
}
function uploadBlogEntryIcon(entryID)
{
	var _element = "entryIcon_" + entryID;
	var cont = $("#entriesWin");
	
	$.showLoading("Uploading Blog Entry Icon");
	
	$.ajaxFileUpload( {
		url : 'admin/ajax/blogs?action=uploadBlogEntryIcon&entryID=' + entryID,
		secureuri : false,
		fileElementId : _element,
		dataType : 'json',
		success : function(data, status)
		{
			$.hideLoading();
			
			$("#" + _element).val("").fileinput().nextAll("button").fadeOut();			
			
			if (data.error)
			{
				$.jqAlert(data.error);
				return;
			}			
			
			getMessages();
			getBlogEntries();
		},
		error : function(data, status, e)
		{
			cont.hideLoading();
			alert(e);
		}
	});

	return false;
}
function getSettingsEdit()
{
	$.showLoading("Loading Settings Interface");
	
	$.getJSON("admin/ajax/blogs", {action: "getSettingsEdit"}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		$(output.content).appendTo("body");
	});	
}
function saveSettings()
{
	var win = $("#settingsWin");
	
	win.showLoading("Saving Settings");
	
	$.post("admin/ajax/blogs?action=saveSettings", $("#settingsForm").serialize(), null, "json").then(function(output)
	{
		win.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		win.dialog('close');
	});
}
function checkEntryEditClose(obj)
{
	var dirty = $(obj).data("dirty");	
	if (!dirty || (dirty && confirm('Are you sure you want to close this window without saving your changes?')))
	{
		$(obj).data("dirty", 0);
		return true;
	}
	
	return false;
}
function activateBlog(blogID)
{
	$.showLoading("Activating Blog Category");
	
	$.getJSON("admin/ajax/blogs", {action: "activateBlog", blogID: blogID}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		
		//Refresh Blogs
		getBlogs(0);
		getBlogs(1);
	});
}
function activateBlogEntry(entryID)
{	
	var cont = $("#entriesWin");
	cont.showLoading("Activating Blog Entry");
	
	$.getJSON("admin/ajax/blogs", {action: "activateBlogEntry", entryID: entryID}).then(function(output)
	{
		cont.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		
		cont.dialog('close');
		getBlogEntryWin(output.blogID);
	});
}