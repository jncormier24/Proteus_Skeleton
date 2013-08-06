<?php	
	switch($action)
	{			
		case "showBlog":
			showBlog($blogID);
			break;			
			
		default:
			showCategories();
	}
	
function showCategories()
{
	global $config;
	
	$a = new adminPage("Manage Blog Categories");	
	$a->addPageModule(new jqTableSortModule());
	$a->addPageModule(new jqUploadModule());	
	$a->addPageModule(new jqRedactorModule());
	$a->addPageModule(new jqPopupMenuModule());
	$a->addPageModule(new jqFilterHelperModule());
	$a->addPageModule(new jqUpdateHelperModule());
	$a->addPageModule(new jqMaskedInputModule());
	
	$a->addPageModule(new commentsModule());
	$a->addPageModule(new customContentModule());	
	$a->addPageModule(new subscriptionModule());
	
	$g_sec = $a->authUser;
	
	if (!$g_sec->hasAccess(permissions::$manage_blogs, permissions::read) &&
		!$g_sec->hasAccess(permissions::$modify_blog_category, permissions::read, 0) &&
		!$g_sec->hasAccess(permissions::$manage_blog_feeds, permissions::read))
	{
		$g_sec->security_error();
	}
	
	$bBlock = new panelBlock("", "Configured Blog Categories");
	
	$mnu = new jqPopupMenu();
	
	if ($g_sec->hasAccess(permissions::$manage_blogs, permissions::write))
	{
		$ctl = $mnu->addMenuItem("", "Blog Settings", "", "ui-icon-wrench");
		$ctl->onclick = "getSettingsEdit()";
		
		if ($g_sec->hasAccess(permissions::$manage_blogs, permissions::insert))
		{	
			$ctl = $mnu->addMenuItem("", "Add a new Blog Category", "", "ui-icon-folder-open");
			$ctl->onclick = "getBlogCategoryEdit(0)";
		}
		
		if ($g_sec->hasAccess(permissions::$manage_blog_feeds, permissions::insert))
		{
			$ctl = $mnu->addMenuItem("", "Add a new Blog Feed", "", "ui-icon-signal-diag");
			$ctl->onclick = "getBlogFeedEdit(0)";
		}
		
		$bBlock->header->addControl($mnu);
	}
	
	$tabs = new jqUI_tabControl("blogCatTabs", 0, "admin/blogs");		

	if ($g_sec->hasAccess(permissions::$manage_blogs, permissions::read) || $g_sec->is(permissions::$modify_blog_category))
	{
		$tblFilter = new tableFilter("blogCatFilter", "#blogCatTable", "getBlogs");			
		$tblFilter->setPaging(25);	
	
		$tblFilter->addCustomFilter("status", tableFilter::filter_dropdown, "Status:", array("Active"=>0, "Inactive"=>1));	
	
		$dv = new phpDIV();
		$dv->id = "blogCategories";		

		$tpCategories = new jqUI_tabPage("blog_categories", "Blog Categories", "ui-icon-folder-open");
		$tpCategories->addControl($tblFilter);
		$tpCategories->addControl($dv);
		
		$tabs->addTabPage($tpCategories);
	}
	
	if ($g_sec->hasAccess(permissions::$manage_blogs, permissions::read) || $g_sec->hasAccess(permissions::$manage_blog_feeds, permissions::read))
	{
		$tblFilter = new tableFilter("blogFeedFilter", "#blogFeedTable", "getBlogFeeds");			
		$tblFilter->setPaging(25);	
	
		$tblFilter->addCustomFilter("status", tableFilter::filter_dropdown, "Status:", array("Active"=>0, "Inactive"=>1));	
	
		$dv = new phpDIV();
		$dv->id = "blogFeeds";
		
		$tpFeeds = new jqUI_tabPage("blog_feeds", "Blog Feeds","ui-icon-signal-diag");
		
		$tpFeeds->addControl($tblFilter);
		$tpFeeds->addControl($dv);
		
		$tabs->addTabPage($tpFeeds);
	}
	
	$bBlock->addControl($tabs);
	
	$a->addControl($bBlock);
	$a->render();	
}
function showBlog($blogID)
{
	global $config, $gBB, $g_sec;
	
	$a = new pTemplate();
	$a->addPageModule(new tabPageModule());
	$a->addPageModule(new jqUploadModule());	
	$a->addPageModule(new bbCodeModule());
	$a->addPageModule(new jqElasticTextModule());
	
	$a->addPostScriptObject(new javaControl("$('textarea').elasticText({maxHeight: 500, focusNextOnTab: false, autoSize: true});"));
	
	try
	{
		$bObj = new customBlog($blogID, true, 0);
	}
	catch(Exception $ex)
	{
		error($ex->getMessage());	
	}
	
	if (!$g_sec->hasAccess(permissions::$manage_blogs, permissions::read) &&
		!$g_sec->hasAccess(permissions::$modify_blog_category, permissions::read, $blogID)) $g_sec->security_error();
	
	$writeAccess = $g_sec->hasAccess(permissions::$manage_blogs, permissions::write) ||
				   $g_sec->hasAccess(permissions::$modify_blog_category, permissions::write, $blogID);
		
	$tabs = new tabControl("blogTabs","Configure Blog");
	
	$txtTitle = new customText("title", validation::prepTextForInput($bObj->title), 50, 255, "floating textBox");
	$txtTitle->setLabel("Blog Title:","longLabel floating");
	$txtTitle->breakAfter = true;
	
	$txtDesc = new phpTextArea("description", validation::prepTextForInput($bObj->description), 4, 60);
	$txtDesc->addClass('floating');
	$txtDesc->setLabel("Blog Description:", "longLabel floating");
	$txtDesc->breakAfter = true;

	$chkInactive = new formInput("inactive", "checkbox",1);
	if ($bObj->inactive) $chkInactive->addParameter("checked");
	$chkInactive->class = 'floating';
	$chkInactive->setLabel("Category Inactive?","longLabel floating");
	$chkInactive->breakAfter = true;
	
	$cmdSave = new jqUI_button("btnSaveBlog", "Save Changes", "", "ui-icon-disk");
	$cmdSave->onclick = "saveBlog($blogID);";
	
	$bbDiv = $gBB->getBBCodeBlock('description');
	$bbDiv->style = "float: left; position: relative; left: 145px; border-color: #808080";
	
	$frmUpdate = new phpForm('post','blogForm');	
	$frmUpdate->addControl($txtTitle);
	$frmUpdate->addControl($bbDiv);
	$frmUpdate->addControl($txtDesc);
	$frmUpdate->addControl($chkInactive);
	
	if ($writeAccess)
	{
		$frmUpdate->addControl($cmdSave);
	}
	
	$tpDetail = new tabPage("Blog Detail","#blog_detail");
	$tpDetail->addControl($frmUpdate);
	
	$tabs->addTabPage($tpDetail);	
	
	$pc = new phpDIV();
	$pc->style = "text-align: center;";
	
	$img = $bObj->image ? new image($config[scripturl]."images/blogs/$blogID/$bObj->image") : new phpSpan("No Image Uploaded");
	$img->id = "blogImage";
	$img->breakAfter = true;
	
	$pc->addControl($img);
	
	if ($writeAccess)
	{
		$btnUpload = new jqUI_button("btnUpload", "Upload File", "", "ui-icon-image");
		$btnUpload->style = "display: none;";
		$btnUpload->onclick = "return ajaxFileUpload($blogID);";
		
		$btnClear = new jqUI_button("btnClear", "Clear Image", "", "ui-icon-closethick");
		$btnClear->onclick = "clearBlogImage($blogID)";
		if (!$bObj->image) $btnClear->style = "display: none;";	
		
		$fInput = new formInput("fileToUpload",'file');	
		$fInput->onchange = "$('#btnUpload').show();";
		$fInput->setLabel("Upload a new image: &nbsp;");
		
		$dvUpload = new phpDIV();
		$dvUpload->class = 'uploadDiv';
			
		$dvUpload->addControl($fInput);
		$dvUpload->addControl($btnUpload);
		$dvUpload->addControl($btnClear);
		$dvUpload->addControl($loadingImg);
		
		$pc->addControl($dvUpload);
	}
	
	$tpImage = new tabPage("Blog Image","#blog_image");
	$tpImage->addControl($pc);
	
	$tabs->addTabPage($tpImage);
	
	$hdr = new phpHeaderBlock("Manage Blog Entries","Modify Blog");
	
	$a->addControl($hdr);	
	$a->addControl($tabs);
	$a->addControl(new backBlock("blogs.php"));
	$a->render();
}
function deactivateEntry($entryID)
{
	global $g_sec;
	
	$a = new pTemplate();
	
	try
	{	
		$entry = new customBlogEntry($entryID);
	
		if (!$g_sec->hasAccess(permissions::$manage_blogs, permissions::delete) &&
			!$g_sec->hasAccess(permissions::$modify_blog_category, permissions::delete, $entry->blogID)) $g_sec->security_error();
		
		if ($entry->inactive)
		{
			$cObj = new customComments($entryID, 1);
			$cObj->clearComments();
			
			$a->qry("delete from blogEntryLink where blogEntryID=$entryID");
			$a->qry("delete from blogEntry where id=$entryID");
			
			$a->setMessage("Successfully removed Blog Entry and all Comments");	
		}
		else
		{
			$cv[inactive] = 1;
			
			$affRows = $a->qryUpdateByArray("blogEntry", $cv, "id=$entryID");
			
			if ($affRows) $a->setMessage("Successfully deactivated blog entry.");
		}
		
		$a->redirect("blogs.php?action=showBlog&blogID=$entry->blogID");
		$a->render();
	}
	catch(Exception $ex)
	{
		error($ex->getMessage());
	}
}
?>