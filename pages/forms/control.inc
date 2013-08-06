<?php 
	proteus_core::includeAllModules();
	
	showForm($config["params"][0]);
	
function showForm($formID)
{
	global $config;
	
	$a = new proteusPage("");
	$a->addPageModule(new customFormsModule());
	
	$g_sec = $a->authUser;	
	
	validation::ensureInt($formID, 0);
	
	$formObj = new customForm($formID);
	if (!$formObj->id) error("Sorry, but the requested form could not be found.");
	
	$a->title = $formObj->title;
	
	$frmControl = $formObj->getFormObject();
	$frmControl->addControl(new hiddenInput("action", "submitForm"));
	
	$btnSubmit = new jqUI_button("btnSubmit", "Submit Form", "", "ui-icon-play");
	$btnSubmit->onclick = "submitForm()";
	
	$p = new fileParser("pages/forms/views/view.tpl");
	$p->pushParam("form_title", validation::prepForDisplay($formObj->title));
	$p->pushParam("form_description", $formObj->description);
	$p->pushParam("form_data", $frmControl->render());
	$p->pushParam("form_submit", $btnSubmit->render());
	
	$dvCont = new phpDIV();
	$dvCont->id = "customFormContainer";
	$dvCont->addControl($p);
	
	$a->addControl($dvCont);
	$a->render();
}
?>