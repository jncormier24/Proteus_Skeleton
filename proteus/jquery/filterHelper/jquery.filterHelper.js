(function($)
{	
	$.fn.filterHelper = function(options, value) 
	{
		var obj = $(this);
		
		var defaults = { 
			autoRun: true,
			autoHide: true,
			filterColumns: [],
			customColumns: [],
			sortColumns: [],
			jsMethodParams: [],
			jsMethod: null,
			showFilteredColumns: true,
			sortData: {
				dbColumn: "",
				selector: "",
				direction: ""
			},
			paging: {
				perPage: 50,
				startPos: 0,
				totalCount: 0,
				showFirstLast: true
			}
		};
		
		var accordionOptions = { 
				active: false, 
				collapsible: true, 
				autoHeight: true, 
				header: "h3", 
				change: function() { refresh.fadeToggle(500); }
		};
		
		var alignSorts = function(opts)
		{	
			var current = opts.sortData;
			
			$("span.sortControl", opts.tableSelector).remove();
			
			$.each(opts.sortColumns, function(i, item)
			{
				var sortControl = $("<span></span>").addClass("ui-icon ui-icon-triangle-2-n-s sortControl").click(function()
				{					
					item.direction = (!current.direction || current.direction == "desc" ? "asc" : "desc");
					
					opts.sortData = item;
					
					obj.data("options", opts);
					obj.filterHelper();
				}).attr("title", "Sort Results");
				
				if (current && current.dbColumn == item.dbColumn)
				{
					sortControl.removeClass("ui-icon-triangle-2-n-s");
					
					if (!current.direction || current.direction == "desc")
					{						
						sortControl.addClass("ui-icon-triangle-1-s"); 
					}
					else
					{									
						sortControl.addClass("ui-icon-triangle-1-n");
					}
				}
				
				$(item.selector, opts.tableSelector).append(sortControl);
			});
			
			return true;
		};		
		
		var dirtyFunc = function(evt)
		{
			var thisObj = $(this);
			
			thisObj.removeClass("emptyFilter");			
			
			switch(evt.which)
			{
				case 13:
					obj.filterHelper();
					return;
					
				case KeyEvent.DOM_VK_UP:
				case KeyEvent.DOM_VK_DOWN:
				case KeyEvent.DOM_VK_LEFT:
				case KeyEvent.DOM_VK_RIGHT:
				case KeyEvent.DOM_VK_TAB:
				case KeyEvent.DOM_VK_SHIFT:
				case KeyEvent.DOM_VK_CONTROL:					
				case KeyEvent.DOM_VK_END:
				case KeyEvent.DOM_VK_HOME:
				case KeyEvent.DOM_VK_ALT:
				case KeyEvent.DOM_VK_INSERT:					
					return;											
			}
			
			var pos = thisObj.position();			
			
			if (!thisObj.data("isDirty") && !thisObj.is("select"))
			{
				var span = $("<span>&nbsp;</span>").addClass("ui-icon ui-icon-search jqFilterIcon").click(function() { obj.filterHelper() });
				
				thisObj.after(span);
				
				var width = thisObj.parent().width();
				width -= (thisObj.outerWidth() - thisObj.width());
				
				$("span", thisObj.parent()).each(function(){
					width -= $(this).width();
				});				
				
				thisObj.width(width);
			}
			
			thisObj.data("isDirty", true);
		};
		
		var toggleFilterRow = function(opts, row)
		{
			var dfd = $.Deferred();
			var visible = row.is(":visible");
			
			// Fade in/out, reoslve deferred on animation complete (allows for setting focus when clicking icon handler)
			row.stop().fadeToggle("normal", function() { dfd.resolve(); });
			
			// We're fading out - don't adjust again
			if (visible) return;
			
			$("th", row).each(function()
			{
				var cell = $(this);				
				var ctl = $(".filter", cell);
				
				cell.children().hide();
				
				var width = cell.width();
				width -= (ctl.outerWidth() - ctl.width());
				
				$("span", cell).each(function(){
					width -= $(this).width();
				});
				
				ctl.width(width);
				
				cell.children().show();
			});	
			
			return dfd.promise();
		}
		
		var initColumnFilters = function(opts)
		{
			var showFilter = false;			
			var nextRow;
			
			$.each(opts.filterColumns, function(i, item)
			{	
				var ctl = $("[name='filters[" + item.dbColumn + "]']", obj);
				var ctlClone = ctl.clone(true).css("position","relative");
				var label = (item.label ? item.label : "Search...");				
				
				if (item.selector)
				{
					var itemObj = $(item.selector, opts.tableSelector);
					
					nextRow = itemObj.parent().next().hide();
					if (!nextRow.has("th")) return;
					
					var idx = itemObj.index();									
					var filterCell = $("th", nextRow).eq(idx).addClass("sort");
					
					if (ctl.val().length) showFilter = true;
					
					var filterCtl = $("<span></span>").addClass("ui-icon ui-icon-tag sortControl").click(function()
					{
						$.when(toggleFilterRow(opts, nextRow)).then(function() { ctlClone.focus(); });						
					}).attr("title", "Filter results");
					
					itemObj.append(filterCtl);
					
					ctlClone.show().appendTo(filterCell)
					
					if (item.mask.length && ctlClone.setMask)
					{
						ctlClone.setMask(item.mask);
					}
				}				
			
				if (ctlClone.is("select"))
				{
					ctlClone.val(ctl.val());
				}
				
				if (!ctlClone.val() || (ctlClone.is("select") && ctlClone.prop("selectedIndex") == 0))
				{
					ctlClone.val(label).addClass("emptyFilter");
				}
				else if (!ctlClone.is("select"))
				{
					var spanx = $("<span>&nbsp;</span>").addClass("ui-icon ui-icon-close jqFilterIcon").click(function() 
					{
						ctlClone.val("").data("isDirty", true);
						obj.filterHelper(); 
					});
				
					ctlClone.after(spanx);
					
					/*var width = function() 
					{ 
						var width = ctlClone.outerWidth() - spanx.outerWidth(true) - 10;
						return width < 10 ? 10 : width;
					}();
					
					ctlClone.width(width);*/
				}				
				
				if (ctlClone.is("select"))
				{
					ctlClone.change(function() { $(this).data("isDirty", true); obj.filterHelper(); });
				}
				
				if (ctlClone.prop("tagName").match(/textarea|input/i))
				{
					ctlClone.keyup(dirtyFunc);				
				}
				else
				{
					ctlClone.change(dirtyFunc);
				}				
				
				ctlClone.focus(function()
				{
					var ctlObj = $(this);
					
					if (ctlObj.is(".emptyFilter"))
					{
						ctlObj.removeClass("emptyFilter").val("");
					}
				}).blur(function()
				{
					var ctlObj = $(this);
					
					if (!ctlObj.data("isDirty") && !ctlObj.val())
					{
						ctlObj.addClass("emptyFilter").val(label);						
					}
				});
			});
			
			if (showFilter && nextRow) toggleFilterRow(opts, nextRow);
		}		
				
		var initNavigation = function(opts)
		{
			// Used to format large numbers with commas
			var numberFormat = function numberWithCommas(x) 
			{
				return x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
			};			
			
			var navClick = function()
			{
				obj.filterHelper("setPaging", { startPos: parseInt($(this).attr("data-startpos")) }).filterHelper();
			};			
			
			$("[for='" + opts.tableSelector + "']").remove();
			
			if (opts.paging.totalCount <= opts.paging.perPage) return;
			
			var navDiv = $("<div>").addClass("ajaxNav").attr("for", opts.tableSelector);			
			var nav = [];
			var start = 0, end = 0;
			
			var currentPage = (opts.paging.startPos ? (opts.paging.startPos / opts.paging.perPage)+1 : 1);		
			var totalPages = (opts.paging.totalCount / opts.paging.perPage);
			
			//Add a page if it doesn't divide evenly...
			if (opts.paging.totalCount % opts.paging.perPage) totalPages++;		
			
			var delim = 0;
			for(var i=0; i <= totalPages-1; i++)
			{			
				var pageVal = i + 1;
				var sVal = i * opts.paging.perPage;
				var nLink;
				
				if (pageVal != currentPage)
				{					
					nLink = $("<a href='javascript:void(0)' title='Go to page " + pageVal + "'>").html(numberFormat(pageVal)).addClass("ui-state-default navButton").click(navClick).attr("data-startpos", sVal);
				}
				else
				{					
					nLink = $("<span>").html(numberFormat(currentPage)).addClass("navCurrent");
				}
				
				nav.push(nLink);			
			}	
			
			navDiv.append(nav[0]);			
			
			if (currentPage < 5)
			{			
				start = 1;								
			}		
			else
			{
				start = currentPage - 4;										
			}	
			
			end = currentPage + 2;
			
			if (start > 1) 
			{				
				navDiv.append($("<span>").html("...").addClass("middleSpan"));
			}
			
			//Don't include the last element - it's popped off at the end.
			if (end > (totalPages-2)) end = (totalPages-2);		
			
			for(var j = start; j <= end; j++)
			{
				navDiv.append(nav[j]);					
			}
			
			if (end < (totalPages-3))
			{
				navDiv.append($("<span>").html("...").addClass("middleSpan"));
			}
			
			var last = nav.pop();
			navDiv.append(last);
			
			if (opts.paging.showFirstLast)
			{		
				if (currentPage < totalPages && totalPages > 2)				
				{
					var sPos = currentPage * opts.paging.perPage;
					var iconObj = $("<span>").addClass("ui-icon ui-icon-seek-next");
					var linkObj = $("<a href='javascript:void(0)' title='Next Page'>").html(iconObj).addClass("first_last").click(navClick).attr("data-startpos", sPos);
					
					navDiv.append(linkObj);									
				}			
				
				if (currentPage != 1 && totalPages > 2)				
				{
					sPos = (currentPage-2) * opts.paging.perPage;
					
					if (sPos < 0) sPos=0;
					
					var iconObj = $("<span>").addClass("ui-icon ui-icon-seek-prev");
					var linkObj = $("<a href='javascript:void(0)' title='Previous Page'>").html(iconObj).addClass("first_last").click(navClick).attr("data-startpos", sPos);
					
					navDiv.prepend(linkObj);
				}						
			}
			
		 	if (nav.length)
		 	{
		 		navDiv.prepend($("<span>").html("Go to:").addClass("navLead"));
		 		$(opts.tableSelector).before(navDiv).after(navDiv.clone(true));
		 	}
		};
		
		var updateTotalDisplay = function(opts)
		{
			var dispObj = $("span.totalDisplay", obj);
			dispObj.html("Total rows: <strong>" + opts.paging.totalCount + "</strong>"); 
		};
		
		switch(options)
		{
			case "setPaging":
				opts = obj.data("options");
				if (typeof value.startPos != "undefined") opts.paging.startPos = parseInt(value.startPos);
				
				if (typeof value.totalCount != "undefined")
				{
					opts.paging.totalCount = value.totalCount ? parseInt(value.totalCount) : 0;				
					updateTotalDisplay(opts);
				}
				
				// Save the data, no action
				obj.data("options", opts);
				
				return obj;
			case "initAll":
				opts = obj.data("options");
				
				alignSorts(opts);
				initColumnFilters(opts);				
				initNavigation(opts);
				
				return obj;
			case "initColumnFilters":
				opts = obj.data("options");
				
				initColumnFilters(opts);				
				return obj;
				
			case "initSort":
				opts = obj.data("options");
			
				alignSorts(opts);
				return obj;
				
			case "getFilterData":
				opts = obj.data("options");
				
				opts = $.extend(opts, value);
				return opts;
				
			case "refreshAccordion":
				obj.accordion("destroy").accordion(accordionOptions);
				return true;
		}
		
		if (options)
		{	
			options = $.extend(defaults, options);
			var method = options.jsMethod;
			
			options.jsMethod = null;
			
			$.each(options.sortColumns, function(i, item)
			{
				if (item.direction) options.sortData = item;
			});
			
			// Append the container for the total row display
			$("> div:eq(0)", obj).append("<span class='totalDisplay'></span>");
			
			var refresh = $("<span>").addClass('ui-icon ui-icon-refresh filterRefresh').click(function() { obj.filterHelper(); }).hide();			
			
			// Attach to previous element before initializing Accordion so that the JQuery classes aren't added to it (affecting style)
			obj.parent().append(refresh);
			
			if (options.customColumns.length)
			{
				obj.accordion(accordionOptions);				
			}
			else
			{
				obj.addClass("ui-accordion ui-widget");
				$("h3", obj).css({
					height: 16,
					margin: 0
				}).addClass("ui-accordion-header ui-corner-all ui-widget-content").find("a").hide();	
			}
			
			// Move back to the now initialized Accordion control
			obj.prepend(refresh.fadeIn());
			
			// Attach another event handler to auto-close the filter if that option is iset
			if (options.autoHide) $(".filterButton", obj).click(function() { $("> h3:eq(0) a", obj).click(); });
			
			obj.data("options", options).data("jsMethod", method).fadeIn();
			
			if (options.autoRun) obj.filterHelper();
		}
		else
		{
			options = obj.data("options");
			var method = obj.data("jsMethod");
			var cols = [];
			var pager = $(".filterRight .pager", obj);

			$.each(options.filterColumns, function(i, item)
			{
				var tableCtl = $("[name='filters[" + item.dbColumn + "]']", options.tableSelector);
				var originalCtl = $("[name='filters[" + item.dbColumn + "]']", obj);				
				
				if ((!tableCtl.length && item.default && item.filterType==3) || tableCtl.data("isDirty"))
				{
					item.value = tableCtl.length ? tableCtl.val() : item.default;
					originalCtl.val(item.value);
					
					if (item.value) cols.push(item.dbColumn.indexOf(".") ? item.dbColumn.split(".").pop() : item.dbColumn );
				}
			});
			
			$.each(options.customColumns, function(i, item)
			{	
				if (parseInt(item.filterType) == 4)
				{
					var val1 = $("[name='dates[" + item.key + "_1]']", obj).val();
					var val2 = $("[name='dates[" + item.key + "_2]']", obj).val();
					
					if (val1 && val2)
					{
						item.value = val1 + "_" + val2;
						cols.push(item.key);
					}
					else
					{
						item.value = "";
					}
				}
				else
				{
					item.value = $("[name='custom[" + item.key + "]']", obj).val();
					if (item.value && item.value.trim()) cols.push(item.key);
				}
			});			
			
			var fContent;
			
			if (cols.length && options.showFilteredColumns)
			{
				fContent = "&nbsp;&nbsp; (" + cols.join(", ") + ")";
			}
			else
			{
				fContent = "&nbsp;";
			}
			
			$("> h3 > a > span", obj).html(fContent);
			
			if (pager.length) options.paging.perPage = pager.val();
			
			// Save the data before calling method
			obj.data("options", options);
			
			method();			
		}
		
		return obj;
	}
})(jQuery);