(function()
{	
	$.fn.updateHelper = function(func, options)
	{
		var parent = $(this);
		
		defaults = {
			selector: "input,select,textarea,.redactor_editor",
			disableControls: false,
			autoSave: true,
			leftOffset: 0,
			closeConfirmOnly: 0
		};
		
		var chFunc = function(evt)
		{			
			var opts = parent.data("options");
			var func = parent.data("func");
			
			var dfd = new $.Deferred();
			
			$(opts.selector, parent).prop("readonly","readonly").fadeTo(1, .5); 
			
			$.when(func()).fail(function()
			{
				$(opts.selector, parent).removeProp("readonly").fadeTo(1, 1);
				dfd.reject();
			}).done(function()
			{				
				resetDirty();
				dfd.resolve();
			});
			
			return dfd.promise();
		};
		
		var resetDirty = function()
		{
			var opts = parent.data("options");
			$(opts.selector, parent).removeProp("readonly").fadeTo(1, 1).each(function()
			{
				var thisObj = $(this);					
				if (!thisObj.data("isDirty")) return;
				
				thisObj.data("isDirty", false);
				
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
		}
		
		var isDirty = function()
		{
			var opts = parent.data("options");
			var retval = false;
			
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
			var opts = parent.data("options");
			var dfd = $.Deferred();
			
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
									$.when(chFunc()).fail(function() { dfd.reject(); }).done(function() { dfd.resolve(); });									
								},
								"Discard Changes": function() 
								{
									$(this).dialog("close");
									resetDirty();
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
								win.dialog('close');
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
					
					if (obj.prop("tagName").match(/textarea|input/i))
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
					else if (obj.is(".redactor_editor"))
					{						
						obj.keyup(dirtyFunc);
					}
					else
					{
						obj.change(dirtyFunc);
					}
				});	
			
				parent.data("options", options).data("func", func);
				
				return $(this);
		}
	};
	
	// Define KeyEvent for browsers that don't implement it (Chrome)
	if (typeof KeyEvent == "undefined") {
	    var KeyEvent = {
	        DOM_VK_CANCEL: 3,
	        DOM_VK_HELP: 6,
	        DOM_VK_BACK_SPACE: 8,
	        DOM_VK_TAB: 9,
	        DOM_VK_CLEAR: 12,
	        DOM_VK_RETURN: 13,
	        DOM_VK_ENTER: 14,
	        DOM_VK_SHIFT: 16,
	        DOM_VK_CONTROL: 17,
	        DOM_VK_ALT: 18,
	        DOM_VK_PAUSE: 19,
	        DOM_VK_CAPS_LOCK: 20,
	        DOM_VK_ESCAPE: 27,
	        DOM_VK_SPACE: 32,
	        DOM_VK_PAGE_UP: 33,
	        DOM_VK_PAGE_DOWN: 34,
	        DOM_VK_END: 35,
	        DOM_VK_HOME: 36,
	        DOM_VK_LEFT: 37,
	        DOM_VK_UP: 38,
	        DOM_VK_RIGHT: 39,
	        DOM_VK_DOWN: 40,
	        DOM_VK_PRINTSCREEN: 44,
	        DOM_VK_INSERT: 45,
	        DOM_VK_DELETE: 46,
	        DOM_VK_0: 48,
	        DOM_VK_1: 49,
	        DOM_VK_2: 50,
	        DOM_VK_3: 51,
	        DOM_VK_4: 52,
	        DOM_VK_5: 53,
	        DOM_VK_6: 54,
	        DOM_VK_7: 55,
	        DOM_VK_8: 56,
	        DOM_VK_9: 57,
	        DOM_VK_SEMICOLON: 59,
	        DOM_VK_EQUALS: 61,
	        DOM_VK_A: 65,
	        DOM_VK_B: 66,
	        DOM_VK_C: 67,
	        DOM_VK_D: 68,
	        DOM_VK_E: 69,
	        DOM_VK_F: 70,
	        DOM_VK_G: 71,
	        DOM_VK_H: 72,
	        DOM_VK_I: 73,
	        DOM_VK_J: 74,
	        DOM_VK_K: 75,
	        DOM_VK_L: 76,
	        DOM_VK_M: 77,
	        DOM_VK_N: 78,
	        DOM_VK_O: 79,
	        DOM_VK_P: 80,
	        DOM_VK_Q: 81,
	        DOM_VK_R: 82,
	        DOM_VK_S: 83,
	        DOM_VK_T: 84,
	        DOM_VK_U: 85,
	        DOM_VK_V: 86,
	        DOM_VK_W: 87,
	        DOM_VK_X: 88,
	        DOM_VK_Y: 89,
	        DOM_VK_Z: 90,
	        DOM_VK_CONTEXT_MENU: 93,
	        DOM_VK_NUMPAD0: 96,
	        DOM_VK_NUMPAD1: 97,
	        DOM_VK_NUMPAD2: 98,
	        DOM_VK_NUMPAD3: 99,
	        DOM_VK_NUMPAD4: 100,
	        DOM_VK_NUMPAD5: 101,
	        DOM_VK_NUMPAD6: 102,
	        DOM_VK_NUMPAD7: 103,
	        DOM_VK_NUMPAD8: 104,
	        DOM_VK_NUMPAD9: 105,
	        DOM_VK_MULTIPLY: 106,
	        DOM_VK_ADD: 107,
	        DOM_VK_SEPARATOR: 108,
	        DOM_VK_SUBTRACT: 109,
	        DOM_VK_DECIMAL: 110,
	        DOM_VK_DIVIDE: 111,
	        DOM_VK_F1: 112,
	        DOM_VK_F2: 113,
	        DOM_VK_F3: 114,
	        DOM_VK_F4: 115,
	        DOM_VK_F5: 116,
	        DOM_VK_F6: 117,
	        DOM_VK_F7: 118,
	        DOM_VK_F8: 119,
	        DOM_VK_F9: 120,
	        DOM_VK_F10: 121,
	        DOM_VK_F11: 122,
	        DOM_VK_F12: 123,
	        DOM_VK_F13: 124,
	        DOM_VK_F14: 125,
	        DOM_VK_F15: 126,
	        DOM_VK_F16: 127,
	        DOM_VK_F17: 128,
	        DOM_VK_F18: 129,
	        DOM_VK_F19: 130,
	        DOM_VK_F20: 131,
	        DOM_VK_F21: 132,
	        DOM_VK_F22: 133,
	        DOM_VK_F23: 134,
	        DOM_VK_F24: 135,
	        DOM_VK_NUM_LOCK: 144,
	        DOM_VK_SCROLL_LOCK: 145,
	        DOM_VK_COMMA: 188,
	        DOM_VK_PERIOD: 190,
	        DOM_VK_SLASH: 191,
	        DOM_VK_BACK_QUOTE: 192,
	        DOM_VK_OPEN_BRACKET: 219,
	        DOM_VK_BACK_SLASH: 220,
	        DOM_VK_CLOSE_BRACKET: 221,
	        DOM_VK_QUOTE: 222,
	        DOM_VK_META: 224
	    };
	}
})();