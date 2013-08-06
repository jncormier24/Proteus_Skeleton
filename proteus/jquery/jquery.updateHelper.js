(function()
{	
	$.fn.updateHelper = function(func, options)
	{
		var parent = $(this);
		var optionKey = "updatehelper-options";
		
		defaults = {
			selector: "input,select,textarea",
			disableControls: false,
			autoSave: false,
			leftOffset: 0,
			closeConfirmOnly: 0,
			attemptCallbackClose: false
		};
		
		var chFunc = function(evt)
		{			
			var opts = parent.data(optionKey);
			var func = parent.data("func");
			
			var dfd = new $.Deferred();
			
			// Don't automatically gray/disable fields if the closeConfirmOnly option is set; handled by the window using it.
			if (!opts.closeConfirmOnly) $(opts.selector, parent).prop("readonly","readonly").fadeTo(1, .5); 
			
			$.when(func()).fail(function()
			{				
				dfd.reject();
			}).done(function()
			{
				if (!opts.closeConfirmOnly) $(opts.selector, parent).removeProp("readonly").fadeTo(1, 1);
				
				resetDirty();
				dfd.resolve();
			});
			
			return dfd.promise();
		};
		
		var resetDirty = function()
		{
			var opts = parent.data(optionKey);
			
			if (!opts)
			{
				alert("f");
			}
			opts.isDirty = false;
			
			$(opts.selector, parent).each(function()
			{
				var thisObj = $(this);					
				if (!thisObj.data("isDirty")) return;
				
				thisObj.data("isDirty", false);				
				
				// Leave hidden elements alone (just for the sake of cleanliness - wysiwyg placeholders, etc)
				if (!thisObj.is(":visible")) return;
				
				if (thisObj.attr("type") == "checkbox")
				{
					thisObj.data("originalContent", thisObj.is(":checked"));
				}
				else
				{
					thisObj.data("originalContent", thisObj.val());
				}
			});
			
			$("span.jqSaveIcon").fadeOut('normal', function() { $(this).remove(); });
			
			parent.data(optionKey, opts);
		}
		
		var isDirty = function()
		{
			var opts = parent.data(optionKey);
			var retval = false;
			
			if (opts.isDirty) return true;
			
			$(opts.selector, parent).each(function()
			{	
				if ($(this).data("isDirty"))
				{
					retval = true;
					return false;
				}
			});
			
			return retval;
		};
		
		var openDialog = function()
		{
			var opts = parent.data(optionKey);
			var dfd = $.Deferred();
			
			var attemptCloseDialog = function()
			{
				var dlg = parent.closest(".ui-dialog-content");
				if (dlg.length) dlg.dialog("close");
			}
			
			var conf = $("<div><p>You have unsaved changes, would you like to save now?</p></div>").dialog({
							resizable: false,
							height: 260,
							width: 425,
							modal: true,
							autoOpen: true,
							title: "** Warning - Unsaved Changes Pending **",
							buttons: {
								"Save Changes": function() 
								{
									$(this).dialog("close");
									$.when(chFunc()).fail(function() { dfd.reject(); }).done(function() { attemptCloseDialog(); dfd.resolve(); });									
								},
								"Discard Changes": function() 
								{
									$(this).dialog("close");
									resetDirty();
									attemptCloseDialog();
									dfd.resolve();									
								},
								"Cancel": function()
								{
									$(this).dialog("close");
									dfd.reject();
								}								
							},
							close: function() { $(this).remove(); }
						});	
			
			return dfd.promise();
		};
		
		switch(func)
		{
			case "setDirty":
				var opts = parent.data(optionKey);
				opts.isDirty = true;
				
				parent.data(optionKey, opts);
				break;
			case "promptSave":				
				if (isDirty())
				{
					return openDialog();
				}	
				else
				{
					var dfd = new $.Deferred();
					dfd.resolve();
					
					return dfd.promise();
				}
				break;
			case "reset":				
				resetDirty();
				return parent;				
			
			default:
				options = $.extend(defaults, options);
				
				if (!options.disableControls)
				{				
					var win = parent.closest(".ui-dialog-content");
					
					if (win.length)
					{
						win.bind("dialogbeforeclose", function(event, ui)
						{
							if (!isDirty()) return true;							
							
							$.when(openDialog(options)).then(function()
							{
								// Attempt to close the containing dialog if the option is set (default: false)
								if (options.attemptCallbackClose && win.is(":visible")) win.dialog('close');
							});
							
							return false;
						});
					}
					else
					{
						$(window).bind("beforeunload", function() 
						{ 
							if (isDirty())
							{
								openDialog();
								return '** You have unsaved changes! **\nAre you sure you want to navigate away from this page without saving?';
							}
						});
					}
				}
				
				$(options.selector, this).each(function()
				{
					var obj = $(this);			
					
					if (obj.is(".updateExclude")) return;
					if (options.disableControls) obj.prop("disabled", true);					
					
					if (obj.attr("type") == "checkbox")
					{
						obj.data("originalContent", obj.is(":checked"));
					}
					else
					{
						obj.data("originalContent", obj.val());					
					}
					
					var dirtyFunc = function(evt)
					{							
						var resetControlDirty = function() { obj.data("isDirty", false).next(".jqSaveIcon").fadeOut('normal'); };
						
						// Don't do anything if the controls are disabled (stops prompting for save when no save access)
						if (options.disableControls) return false;
						
						if (options.closeConfirmOnly || (obj.prop("tagName").match(/textarea|input|select/i) && !options.autoSave))
						{		
							if (options.closeConfirmOnly)
							{
								if (obj.is(".redactor_editor")) obj = obj.next("textarea");
								
								obj.data("isDirty", true);
								return;
							}
							
							if (obj.attr("type") == "checkbox") 
							{
								if (obj.data("originalContent") == obj.is(":checked"))
								{
									resetControlDirty();
									return;
								}
							}
							else if (obj.data("originalContent") == obj.val())
							{
								resetControlDirty();
								return;
							}
							
							if (obj.data("isDirty") || options.disableControls) return;
							
							// This is for DatePickers - might still need this? (moving save icon inside input area, SWB 7/13)
							//var offset = obj.is(".datePicker,.hasDatepicker") ? -16 : 0;
							
							var pos = obj.position();
							var span = obj.next(".jqSaveIcon");
								
							if (!span.length)
							{
								span = $("<span>&nbsp;</span>").addClass("ui-icon ui-icon-disk jqSaveIcon").attr("title", "Save Pending Changes").click(chFunc);
							
								var ctlOffset = span.outerWidth() + options.leftOffset;
								
								obj.after(span).css(
								{
									paddingRight: ctlOffset,
									width: "-=" + (ctlOffset - parseInt(obj.css("padding-right")) - parseInt(obj.css("border-right-width"))) 
								});
								
								var iconLeft;
								var iconTop;
								
								if (obj.attr("type") == "checkbox")
								{
									iconLeft = pos.left + obj.outerWidth() + options.leftOffset + 5;
									iconTop = pos.top + parseInt(obj.css("margin-top")) - 2;
								}
								else
								{
									iconLeft = pos.left + obj.outerWidth() + options.leftOffset - (span.outerWidth());
									iconTop = pos.top + ((obj.outerHeight() / 2) - (span.outerHeight() / 2));
								}
								
								span.css(
								{
									position: "absolute",
									top: iconTop,
									left: iconLeft,
									cursor: "pointer"
								});
							}
							else
							{
								span.fadeIn()
							}
							
							obj.data("isDirty", true);
						}
						else
						{
							clearTimeout(obj.data("updateTimeout"));
						 	obj.data("updateTimeout", setTimeout(chFunc, 1500)).blur(function()
						 	{
						 		clearTimeout(obj.data("updateTimeout"));
						 		chFunc();
						 	});
						}
					};			
					
					if (obj.is(".wysiwyg"))
					{
						// Hook to the Redactor keyup event (does not work w/ iframe editor!! Need to bind keyupCallback by hand)
						var editor = obj.redactor("getEditor");
						
						editor.on("keyup.redactor", $.proxy(dirtyFunc, obj));
					}
					else if (obj.prop("tagName").match(/textarea|input/i))
					{
						if (obj.is(".datePicker,.hasDatepicker"))
						{
							var func = obj.datepicker("option","onSelect");
							// If the onSelect binding exists, proxy it to use the same object as the context (so that internally "this" is correct)
							if (typeof func != "undefined") func = $.proxy(obj.datepicker("option","onSelect"), obj);
							
							obj.datepicker("option","onSelect", function() 
							{
								dirtyFunc()
								// Carry any existing functionality forward into this chain
								if (typeof func != "undefined") func(); 
							});
								
							//obj.datepicker("option","onSelect", dirtyFunc);							
						}
						else if (obj.attr("type") == "checkbox")
						{							
							obj.change(dirtyFunc);
						}
						else
						{
							obj.keyup(dirtyFunc);
						}
					}					
					else
					{
						obj.change(dirtyFunc);
					}
				});	
			
				parent.data(optionKey, options).data("func", func);
				
				return $(this);
		}
	};
})();