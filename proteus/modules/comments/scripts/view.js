$(function()
{
	$("div.commentContainer").each(function()
	{
		var dataID = $(this).attr("data-id");
		var typeID = $(this).attr("type-id");
		
		getComments(dataID, typeID, this);
	});
}); 
function getComments(dataID, typeID, container)
{
	var cont = $(container);
	
	$.getJSON("admin/module/comments", {action: "getComments", dataID: dataID, typeID: typeID}).then(function(output)
	{
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		cont.html(output.content);

		$("#" + cont.closest(".tabContent").attr("data-tablink")).removeClass('tabLoading').find(".tabLoaderIcon").replaceWith("<span class='commentCount tabLoaderIcon'> (" + output.commentCount + ")</span>");
		
		cont.updateHelper(function() { return addComment(dataID, typeID, container); }, {autoSave: false});
		
		$("textarea", cont).click(function() 
		{ 
			var obj = $(this);
			
			if (obj.is(".empty")) obj.removeClass("empty").val("");
			obj.addClass("focused");
		}).blur(function()
		{
			var obj = $(this);
			
			if (!$(this).val().length)
			{	
				// Needed the extra keyup to trigger the updateHelper dirty mechanism *after* setting the content back to the default string
				obj.val("Post a comment...").addClass("empty").keyup();
			}
			
			obj.removeClass("focused");
		}).autogrow();
	});
}
function addComment(dataID, typeID, cont)
{
	$.showLoading("Adding Comment");
	
	$.post("admin/module/comments", {
		action: "addComment", 
		comment: $("#adminComment").val(),
		dataID: dataID,
		typeID: typeID
	}, null, "json").then(function(output)
	{
		if (!$.ajaxError(output, $))
		{
			getMessages();
			getComments(dataID, typeID, cont);
		}
	});
}
function deleteComment(commentID)
{
	$.jqConfirm("Are you sure you want to remove this comment? This cannot be undone.", function()
	{
		$.showLoading("Removing Comment");
		
		$.getJSON("admin/module/comments", {action: "deleteComment", commentID: commentID}).then(function(output)
		{
			$.hideLoading();
			
			if (output.error)
			{
				$(output.error).appendTo("body");
				return;
			}		
			
			$("#commentRow_" + commentID).fadeOut('slow', function() 
			{ 
				var commCt = $("tr", $(this).parent()).length-1;
				
				$("span.commentCount").html("(" + commCt + ")");
				$(this).remove();
			});
			
			getMessages();		
		});
	});
}
function approveComment(commentID)
{
	$.showLoading("Approving Comment");
	
	$.getJSON("admin/module/comments", {action: "approveComment", commentID: commentID}).then(function(output)
	{
		$.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return;
		}
		
		getMessages();
		
		getComments(output.dataID, output.typeID, 0, "#blog_pending_comments");
		getComments(output.dataID, output.typeID, 1, "#blog_approved_comments");		
	});
}