function getMessages()
{
	var ttl = 8000;
	
	//Get any message boxes...
	$.getJSON("admin/ajax/common", {action: "getMessages"}).then(function(output)
	{
		if (output.message) $.jGrowl("<span class='ui-icon ui-icon-circle-check' style='float:left; margin:1px 7px 5px 0px;'></span><span class='systemNotif'>" + output.message + "</span>", { life: ttl, header: "System Notification:" });
	});	
}
function dismiss()
{
	var win = $("#dismissableMessageWin");
	var key = win.attr("data-key");
	
	$.getJSON("admin/ajax/common", {action: "dismissMessage", notificationKey: key}).then(function(output)
	{
		if (!$.ajaxError(output))
		{
			getMessages();
			win.dialog("close");
		}
	});
}
function initJQButtons()
{
	$(".jqui_button").not('.ui-button').each(function()
	{
		var iconClass = $(this).attr("data-iconclass");
		var txtVal = $(this).html() ? true : false;
		
		$(this).button({icons: {primary: iconClass}, text: txtVal });
	});	
}
$(document).ready(function()
{	
	// JSB: Restrict browsers from caching AJAX requests (IE 10)
	$.ajaxSetup({ cache: false });
	
	//Put a non-breaking space in every empty TD (there you go, Internet Exporer - happy now?)
	$("td:empty").html("&nbsp;");
	
	//Disable auto-submit via enter key on forms inside dialog windows
	//$(document).one("keypress", ".ui-dialog form input", function(event) { if (event.which == 13) event.preventDefault(); });
	
	initJQButtons();
	getMessages();
	
	$(document).tooltip({items: "[title]:not(:button)"});
});

// Limit scope pollution from any deprecated API
(function() 
{
	var matched, browser;
	
	// Use of jQuery.browser is frowned upon.
	// More details: http://api.jquery.com/jQuery.browser
	// jQuery.uaMatch maintained for back-compat
	jQuery.uaMatch = function( ua ) 
	{
		ua = ua.toLowerCase();
	
		var match = /(chrome)[ \/]([\w.]+)/.exec( ua ) ||
					/(webkit)[ \/]([\w.]+)/.exec( ua ) ||
					/(opera)(?:.*version|)[ \/]([\w.]+)/.exec( ua ) ||
					/(msie) ([\w.]+)/.exec( ua ) ||
					ua.indexOf("compatible") < 0 && /(mozilla)(?:.*? rv:([\w.]+)|)/.exec( ua ) ||
					[];
		
		return {
			browser: match[ 1 ] || "",
			version: match[ 2 ] || "0"
		};
	};
	
	matched = jQuery.uaMatch( navigator.userAgent );
	browser = {};
	
	if ( matched.browser ) 
	{
		browser[ matched.browser ] = true;
		browser.version = matched.version;
	}

	// Chrome is Webkit, but Webkit is also Safari.
	if ( browser.chrome ) 
	{
		browser.webkit = true;
	}
	else if ( browser.webkit ) 
	{
		browser.safari = true;
	}
	
	jQuery.browser = browser;

	jQuery.sub = function() {
		function jQuerySub( selector, context ) 
		{
			return new jQuerySub.fn.init( selector, context );
		}
	
		jQuery.extend( true, jQuerySub, this );
		jQuerySub.superclass = this;
		jQuerySub.fn = jQuerySub.prototype = this();
		jQuerySub.fn.constructor = jQuerySub;
		jQuerySub.sub = this.sub;
		jQuerySub.fn.init = function init( selector, context ) 
		{
			if ( context && context instanceof jQuery && !(context instanceof jQuerySub) ) 
			{
				context = jQuerySub( context );
			}
			
			return jQuery.fn.init.call( this, selector, context, rootjQuerySub );
		};
		
		jQuerySub.fn.init.prototype = jQuerySub.fn;
		var rootjQuerySub = jQuerySub(document);
		
		return jQuerySub;
	};
	
	// Define KeyEvent for browsers that don't implement it (Chrome)
	if (typeof KeyEvent == "undefined") 
	{
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
(function()
{	
	jQuery.query = function() {
	    var r = {};
	    var params = location.search.replace(/^\?/,'').split('&');
	    for( var i = params.length-1;  i >= 0;  i-- ) {
	       var p = params[i].split('='), key = p[0];
	       if( key ) r[key] = p[1];
	    }
	    return r;
	 };
	 
	 $.fn.setWorking = function(caption)
	 {		 
		// Setting to 0 overrides the caption entirely and prevents any text from being rendered, only the image is shown
		if (caption != 0)
		{
			if (!caption) caption = "Loading, please wait";	
			caption = "<em>.: " + caption + " :.</em>";		 
		}
		else
		{
			caption = "&nbsp;";
		}
		
		var content = $("<div class='jqWorking'>" + caption + "</div>");		 
		
		$(this).html(content);
		 
		return this;
	 };	 
	 $.fn.clearWorking = function()
	 {
		 $(".pr_waitingDiv", this).fadeOut('normal', function() { $(this).remove(); });
	 };
	 $.fn.enable = function() { $(this).removeAttr("disabled").css("cursor","pointer"); return this; };	 
	 $.fn.disable = function() { $(this).attr("disabled", "disabled").css("cursor","not-allowed"); return this; };
}) ();
(function($) 
{	
	$.fn._fadeIn = $.fn.fadeIn;
	
	var noOp = function() {};
	
	// this bit is to ensure we don't call setExpression when we shouldn't (with extra muscle to handle
	// retarded userAgent strings on Vista)
	var mode = document.documentMode || 0;
	var setExpr = $.browser.msie && (($.browser.version < 8 && !mode) || mode < 8);
	var ie6 = $.browser.msie && /MSIE 6.0/.test(navigator.userAgent) && !mode;
	
	// global $ methods for blocking/unblocking the entire page
	$.blockUI   = function(opts) { install(window, opts); };
	$.unblockUI = function(opts) { remove(window, opts); };
	
	// convenience method for quick growl-like notifications  (http://www.google.com/search?q=growl)
	$.growlUI = function(title, message, timeout, onClose) {
		var $m = $('<div class="growlUI"></div>');
		if (title) $m.append('<h1>'+title+'</h1>');
		if (message) $m.append('<h2>'+message+'</h2>');
		if (timeout == undefined) timeout = 3000;
		$.blockUI({
			message: $m, fadeIn: 700, fadeOut: 1000, centerY: false,
			timeout: timeout, showOverlay: false,
			onUnblock: onClose, 
			css: $.blockUI.defaults.growlCSS
		});
	};
	
	// plugin method for blocking element content
	$.fn.block = function(opts) {
		return this.unblock({ fadeOut: 0 }).each(function() {
			if ($.css(this,'position') == 'static')
				this.style.position = 'relative';
			if ($.browser.msie)
				this.style.zoom = 1; // force 'hasLayout'
			install(this, opts);
		});
	};
	
	// plugin method for unblocking element content
	$.fn.unblock = function(opts) {
		return this.each(function() {
			remove(this, opts);
		});
	};
	
	$.blockUI.version = 2.35; // 2nd generation blocking at no extra cost!
	
	// override these in your code to change the default behavior and style
	$.blockUI.defaults = {
		// message displayed when blocking (use null for no message)
		message:  '<h1>Please wait...</h1>',
	
		title: null,	  // title string; only used when theme == true
		draggable: true,  // only used when theme == true (requires jquery-ui.js to be loaded)
		
		theme: false, // set to true to use with jQuery UI themes
		
		// styles for the message when blocking; if you wish to disable
		// these and use an external stylesheet then do this in your code:
		// $.blockUI.defaults.css = {};
		css: {
			padding:	0,
			margin:		0,
			width:		'30%',
			top:		'40%',
			left:		'35%',
			textAlign:	'center',
			color:		'#000',
			border:		'3px solid #aaa',
			backgroundColor:'#fff',
			cursor:		'wait'
		},
		
		// minimal style set used when themes are used
		themedCSS: {
			width:	'30%',
			top:	'40%',
			left:	'35%'
		},
	
		// styles for the overlay
		overlayCSS:  {
			backgroundColor: '#000',
			opacity:	  	 0.6,
			cursor:		  	 'wait'
		},
	
		// styles applied when using $.growlUI
		growlCSS: {
			width:  	'350px',
			top:		'10px',
			left:   	'',
			right:  	'10px',
			border: 	'none',
			padding:	'5px',
			opacity:	0.6,
			cursor: 	'default',
			color:		'#fff',
			backgroundColor: '#000',
			'-webkit-border-radius': '10px',
			'-moz-border-radius':	 '10px',
			'border-radius': 		 '10px'
		},
		
		// IE issues: 'about:blank' fails on HTTPS and javascript:false is s-l-o-w
		// (hat tip to Jorge H. N. de Vasconcelos)
		iframeSrc: /^https/i.test(window.location.href || '') ? 'javascript:false' : 'about:blank',
	
		// force usage of iframe in non-IE browsers (handy for blocking applets)
		forceIframe: false,
	
		// z-index for the blocking overlay
		baseZ: 9000,
	
		// set these to true to have the message automatically centered
		centerX: true, // <-- only effects element blocking (page block controlled via css above)
		centerY: true,
	
		// allow body element to be stetched in ie6; this makes blocking look better
		// on "short" pages.  disable if you wish to prevent changes to the body height
		allowBodyStretch: true,
	
		// enable if you want key and mouse events to be disabled for content that is blocked
		bindEvents: true,
	
		// be default blockUI will supress tab navigation from leaving blocking content
		// (if bindEvents is true)
		constrainTabKey: true,
	
		// fadeIn time in millis; set to 0 to disable fadeIn on block
		fadeIn:  200,
	
		// fadeOut time in millis; set to 0 to disable fadeOut on unblock
		fadeOut:  400,
	
		// time in millis to wait before auto-unblocking; set to 0 to disable auto-unblock
		timeout: 0,
	
		// disable if you don't want to show the overlay
		showOverlay: true,
	
		// if true, focus will be placed in the first available input field when
		// page blocking
		focusInput: true,
	
		// suppresses the use of overlay styles on FF/Linux (due to performance issues with opacity)
		applyPlatformOpacityRules: true,
		
		// callback method invoked when fadeIn has completed and blocking message is visible
		onBlock: null,
	
		// callback method invoked when unblocking has completed; the callback is
		// passed the element that has been unblocked (which is the window object for page
		// blocks) and the options that were passed to the unblock call:
		//	 onUnblock(element, options)
		onUnblock: null,
	
		// don't ask; if you really must know: http://groups.google.com/group/jquery-en/browse_thread/thread/36640a8730503595/2f6a79a77a78e493#2f6a79a77a78e493
		quirksmodeOffsetHack: 4,
	
		// class name of the message block
		blockMsgClass: 'blockMsg'
	};
	
	// private data and functions follow...
	
	var pageBlock = null;
	var pageBlockEls = [];
	
	function install(el, opts) {
		var full = (el == window);
		var msg = opts && opts.message !== undefined ? opts.message : undefined;
		opts = $.extend({}, $.blockUI.defaults, opts || {});
		opts.overlayCSS = $.extend({}, $.blockUI.defaults.overlayCSS, opts.overlayCSS || {});
		var css = $.extend({}, $.blockUI.defaults.css, opts.css || {});
		var themedCSS = $.extend({}, $.blockUI.defaults.themedCSS, opts.themedCSS || {});
		msg = msg === undefined ? opts.message : msg;
	
		// remove the current block (if there is one)
		if (full && pageBlock)
			remove(window, {fadeOut:0});
	
		// if an existing element is being used as the blocking content then we capture
		// its current place in the DOM (and current display style) so we can restore
		// it when we unblock
		if (msg && typeof msg != 'string' && (msg.parentNode || msg.jquery)) {
			var node = msg.jquery ? msg[0] : msg;
			var data = {};
			$(el).data('blockUI.history', data);
			data.el = node;
			data.parent = node.parentNode;
			data.display = node.style.display;
			data.position = node.style.position;
			if (data.parent)
				data.parent.removeChild(node);
		}
	
		var z = opts.baseZ;
	
		// blockUI uses 3 layers for blocking, for simplicity they are all used on every platform;
		// layer1 is the iframe layer which is used to supress bleed through of underlying content
		// layer2 is the overlay layer which has opacity and a wait cursor (by default)
		// layer3 is the message content that is displayed while blocking
	
		var lyr1 = ($.browser.msie || opts.forceIframe) 
			? $('<iframe class="blockUI" style="z-index:'+ (z++) +';display:none;border:none;margin:0;padding:0;position:absolute;width:100%;height:100%;top:0;left:0" src="'+opts.iframeSrc+'"></iframe>')
			: $('<div class="blockUI" style="display:none"></div>');
		var lyr2 = $('<div class="blockUI blockOverlay" style="z-index:'+ (z++) +';display:none;border:none;margin:0;padding:0;width:100%;height:100%;top:0;left:0"></div>');
		
		var lyr3, s;
		if (opts.theme && full) {
			s = '<div class="blockUI ' + opts.blockMsgClass + ' blockPage ui-dialog ui-widget ui-corner-all" style="z-index:'+z+';display:none;position:fixed">' +
					'<div class="ui-widget-header ui-dialog-titlebar ui-corner-all blockTitle">'+(opts.title || '&nbsp;')+'</div>' +
					'<div class="ui-widget-content ui-dialog-content"></div>' +
				'</div>';
		}
		else if (opts.theme) {
			s = '<div class="blockUI ' + opts.blockMsgClass + ' blockElement ui-dialog ui-widget ui-corner-all" style="z-index:'+z+';display:none;position:absolute">' +
					'<div class="ui-widget-header ui-dialog-titlebar ui-corner-all blockTitle">'+(opts.title || '&nbsp;')+'</div>' +
					'<div class="ui-widget-content ui-dialog-content"></div>' +
				'</div>';
		}
		else if (full) {
			s = '<div class="blockUI ' + opts.blockMsgClass + ' blockPage" style="z-index:'+z+';display:none;position:fixed"></div>';
		}			
		else {
			s = '<div class="blockUI ' + opts.blockMsgClass + ' blockElement" style="z-index:'+z+';display:none;position:absolute"></div>';
		}
		lyr3 = $(s);
	
		// if we have a message, style it
		if (msg) {
			if (opts.theme) {
				lyr3.css(themedCSS);
				lyr3.addClass('ui-widget-content');
			}
			else 
				lyr3.css(css);
		}
	
		// style the overlay
		if (!opts.applyPlatformOpacityRules || !($.browser.mozilla && /Linux/.test(navigator.platform)))
			lyr2.css(opts.overlayCSS);
		lyr2.css('position', full ? 'fixed' : 'absolute');
	
		// make iframe layer transparent in IE
		if ($.browser.msie || opts.forceIframe)
			lyr1.css('opacity',0.0);
	
		//$([lyr1[0],lyr2[0],lyr3[0]]).appendTo(full ? 'body' : el);
		var layers = [lyr1,lyr2,lyr3], $par = full ? $('body') : $(el);
		$.each(layers, function() {
			this.appendTo($par);
		});
		
		if (opts.theme && opts.draggable && $.fn.draggable) {
			lyr3.draggable({
				handle: '.ui-dialog-titlebar',
				cancel: 'li'
			});
		}
	
		// ie7 must use absolute positioning in quirks mode and to account for activex issues (when scrolling)
		var expr = setExpr && (!$.boxModel || $('object,embed', full ? null : el).length > 0);
		if (ie6 || expr) {
			// give body 100% height
			if (full && opts.allowBodyStretch && $.boxModel)
				$('html,body').css('height','100%');
	
			// fix ie6 issue when blocked element has a border width
			if ((ie6 || !$.boxModel) && !full) {
				var t = sz(el,'borderTopWidth'), l = sz(el,'borderLeftWidth');
				var fixT = t ? '(0 - '+t+')' : 0;
				var fixL = l ? '(0 - '+l+')' : 0;
			}
	
			// simulate fixed position
			$.each([lyr1,lyr2,lyr3], function(i,o) {
				var s = o[0].style;
				s.position = 'absolute';
				if (i < 2) {
					full ? s.setExpression('height','Math.max(document.body.scrollHeight, document.body.offsetHeight) - (jQuery.boxModel?0:'+opts.quirksmodeOffsetHack+') + "px"')
						 : s.setExpression('height','this.parentNode.offsetHeight + "px"');
					full ? s.setExpression('width','jQuery.boxModel && document.documentElement.clientWidth || document.body.clientWidth + "px"')
						 : s.setExpression('width','this.parentNode.offsetWidth + "px"');
					if (fixL) s.setExpression('left', fixL);
					if (fixT) s.setExpression('top', fixT);
				}
				else if (opts.centerY) {
					if (full) s.setExpression('top','(document.documentElement.clientHeight || document.body.clientHeight) / 2 - (this.offsetHeight / 2) + (blah = document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop) + "px"');
					s.marginTop = 0;
				}
				else if (!opts.centerY && full) {
					var top = (opts.css && opts.css.top) ? parseInt(opts.css.top) : 0;
					var expression = '((document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop) + '+top+') + "px"';
					s.setExpression('top',expression);
				}
			});
		}
	
		// show the message
		if (msg) {
			if (opts.theme)
				lyr3.find('.ui-widget-content').append(msg);
			else
				lyr3.append(msg);
			if (msg.jquery || msg.nodeType)
				$(msg).show();
		}
	
		if (($.browser.msie || opts.forceIframe) && opts.showOverlay)
			lyr1.show(); // opacity is zero
		if (opts.fadeIn) {
			var cb = opts.onBlock ? opts.onBlock : noOp;
			var cb1 = (opts.showOverlay && !msg) ? cb : noOp;
			var cb2 = msg ? cb : noOp;
			if (opts.showOverlay)
				lyr2._fadeIn(opts.fadeIn, cb1);
			if (msg)
				lyr3._fadeIn(opts.fadeIn, cb2);
		}
		else {
			if (opts.showOverlay)
				lyr2.show();
			if (msg)
				lyr3.show();
			if (opts.onBlock)
				opts.onBlock();
		}
	
		// bind key and mouse events
		bind(1, el, opts);
	
		if (full) {
			pageBlock = lyr3[0];
			pageBlockEls = $(':input:enabled:visible',pageBlock);
			if (opts.focusInput)
				setTimeout(focus, 20);
		}
		else
			center(lyr3[0], opts.centerX, opts.centerY);
	
		if (opts.timeout) {
			// auto-unblock
			var to = setTimeout(function() {
				full ? $.unblockUI(opts) : $(el).unblock(opts);
			}, opts.timeout);
			$(el).data('blockUI.timeout', to);
		}
	};
	
	// remove the block
	function remove(el, opts) {
		var full = (el == window);
		var $el = $(el);
		var data = $el.data('blockUI.history');
		var to = $el.data('blockUI.timeout');
		if (to) {
			clearTimeout(to);
			$el.removeData('blockUI.timeout');
		}
		opts = $.extend({}, $.blockUI.defaults, opts || {});
		bind(0, el, opts); // unbind events
		
		var els;
		if (full) // crazy selector to handle odd field errors in ie6/7
			els = $('body').children().filter('.blockUI').add('body > .blockUI');
		else
			els = $('.blockUI', el);
	
		if (full)
			pageBlock = pageBlockEls = null;
	
		if (opts.fadeOut) {
			els.fadeOut(opts.fadeOut);
			setTimeout(function() { reset(els,data,opts,el); }, opts.fadeOut);
		}
		else
			reset(els, data, opts, el);
	};
	
	// move blocking element back into the DOM where it started
	function reset(els,data,opts,el) {
		els.each(function(i,o) {
			// remove via DOM calls so we don't lose event handlers
			if (this.parentNode)
				this.parentNode.removeChild(this);
		});
	
		if (data && data.el) {
			data.el.style.display = data.display;
			data.el.style.position = data.position;
			if (data.parent)
				data.parent.appendChild(data.el);
			$(el).removeData('blockUI.history');
		}
	
		if (typeof opts.onUnblock == 'function')
			opts.onUnblock(el,opts);
	};
	
	// bind/unbind the handler
	function bind(b, el, opts) {
		var full = el == window, $el = $(el);
	
		// don't bother unbinding if there is nothing to unbind
		if (!b && (full && !pageBlock || !full && !$el.data('blockUI.isBlocked')))
			return;
		if (!full)
			$el.data('blockUI.isBlocked', b);
	
		// don't bind events when overlay is not in use or if bindEvents is false
		if (!opts.bindEvents || (b && !opts.showOverlay)) 
			return;
	
		// bind anchors and inputs for mouse and key events
		var events = 'mousedown mouseup keydown keypress';
		b ? $(document).bind(events, opts, handler) : $(document).unbind(events, handler);
	
	// former impl...
	//		   var $e = $('a,:input');
	//		   b ? $e.bind(events, opts, handler) : $e.unbind(events, handler);
	};
	
	// event handler to suppress keyboard/mouse events when blocking
	function handler(e) {
		// allow tab navigation (conditionally)
		if (e.keyCode && e.keyCode == 9) {
			if (pageBlock && e.data.constrainTabKey) {
				var els = pageBlockEls;
				var fwd = !e.shiftKey && e.target === els[els.length-1];
				var back = e.shiftKey && e.target === els[0];
				if (fwd || back) {
					setTimeout(function(){focus(back)},10);
					return false;
				}
			}
		}
		var opts = e.data;
		// allow events within the message content
		if ($(e.target).parents('div.' + opts.blockMsgClass).length > 0)
			return true;
	
		// allow events for content that is not being blocked
		return $(e.target).parents().children().filter('div.blockUI').length == 0;
	};
	
	function focus(back) {
		if (!pageBlockEls)
			return;
		var e = pageBlockEls[back===true ? pageBlockEls.length-1 : 0];
		if (e)
			e.focus();
	};
	
	function center(el, x, y) {
		var p = el.parentNode, s = el.style;
		var l = ((p.offsetWidth - el.offsetWidth)/2) - sz(p,'borderLeftWidth');
		var t = ((p.offsetHeight - el.offsetHeight)/2) - sz(p,'borderTopWidth');
		if (x) s.left = l > 0 ? (l+'px') : '0';
		if (y) s.top  = t > 0 ? (t+'px') : '0';
	};
	
	function sz(el, p) {
		return parseInt($.css(el,p))||0;
	};
	
	$.fn.showLoading = function(caption, ctlToDisable, themeStyle)
	{
		if (!caption)
		{
			caption = "Loading";
		}
		
		caption = ".: " + caption + " :.";
		 
		var content = $("<div class='jqWorking' style='text-align: center;'><em>" + caption + "</em></div>");
		var blockObj = $(this);
		
		if ($(this).is(".ui-dialog-content"))
		{
			blockObj = $(this).closest(".ui-dialog");
		}
		
		var defStyle = { width: '45%' };
		var hashStyle = $.extend(defStyle, themeStyle);
		
		blockObj.block(
		{
			theme: true, 
			message: content,
			title: "Please wait...",
			themedCSS: hashStyle,
			baseZ: 10			
		});		
		
		if (ctlToDisable)
		{
			var btn = $(ctlToDisable);
			
			btn.each(function()
			{
				if ($(this).hasClass('ui-button'))
				{
					$(this).button('disable');
				}
				else
				{
					$(this).disable();
				}
			});
			
			$(this).data("disabledCtl", ctlToDisable);
		}
	}
	$.fn.hideLoading = function()
	{			
		var data = $(this).data("disabledCtl");
		
		var blockObj = $(this);
		
		if ($(this).is(".ui-dialog-content"))
		{			
			blockObj = $(this).closest(".ui-dialog");
		}
		
		if (data)
		{
			var ctl = $(data);
			
			ctl.each(function()
			{
				if ($(this).hasClass('ui-button'))
				{
					$(this).button('enable');
				}
				else
				{
					$(this).enable();
				}
			});
		}
		
		blockObj.unblock();
	}
	$.showLoading = function(caption)
	{		 
		if (!caption)
		{
			caption = "Loading";
		}
		
		caption = ".: " + caption + " :.";
	 
		var content = $("<div class='jqWorking' style='text-align: center;'><em>" + caption + "</em></div>");
		
		$.blockUI({theme: true, message: content, title: "Please wait..."});
	};
	$.hideLoading = function()
	{		
		//Just a wrapper for ease of use
		$.unblockUI();
	};
	$.ajaxError = function(output, loaderObj) 
	{ 
		// Can pass $ directly from call
		if (loaderObj) loaderObj.hideLoading();
		
		if (output.error)
		{
			$(output.error).appendTo("body");
			return true;
		}
		
		return false;
	};	
	$.jqAlert = function(description, opts)
	{
		var options = $.extend(
		{
			title: "Error",
			closeCallback: function() { $(this).remove(); },
			buttons: [{text: "Ok", click: function() { $(this).dialog('close'); }}],
			width: 600,
			height: 225,
			autoResize: true
		}, opts);
		
		$("<div>").html("<p>" + description + "</p>").dialog(
		{
			autoOpen: true,
			title: options.title,
			buttons: options.buttons,
			height: options.height,
			width: options.width,
			close: options.closeCallback,
			autoResize: options.autoResize,
			modal: true								
		});
	}
	$.fn.jqConfirm = function(desc, clickEvt, opts)
	{	
		$(this).not(".jqRemoveConfirmRendered").each(function()
		{			 
			var click;			
						
			if (!clickEvt)
			{
				click = this.onclick;
				$(this).prop("onclick", null);
			}
			else
			{
				click = clickEvt;
			}
			
			$(this).click(function(event)
			{				
				disp = $(this).attr("alt");
				if (!disp) disp = desc;
				
				event.stopImmediatePropagation();
				event.preventDefault();
				
				opts = $.extend(
				{ 
					title: "Please Confirm", 
					buttons:
					[{
						text: "Yes", 
						click: function() { $(this).dialog('close'); click(); }                                               	   
					},
					{
						text: "No",
						click: function() { $(this).dialog('close'); }
					}]
				}, opts);
				
				$.jqAlert(disp, opts);
				
				return false;
			}).addClass("jqRemoveConfirmRendered");
		});
		
		return $(this);
	}
	$.jqConfirm = function(desc, clickEvt, opts)
	{	
		opts = $.extend(
		{ 
			title: "Please Confirm",
			width: 550,
			buttons:
			[{
				text: "Yes", 
				click: function() { $(this).dialog('close'); clickEvt(); }                                               	   
			},
			{
				text: "No",
				click: function() { $(this).dialog('close'); }
			}]
		}, opts);
		
		$.jqAlert(desc, opts);		
		
		return $;
	}		
	$.blockContainer = function()
	{
		// Just a wrapper function to facilitate manually locking the container div for issues with Redactor in modal JQuery Dialogs (7/13)
		$("div:visible:first").block({message: "", baseZ: 10});
	}
	$.unblockContainer = function()
	{
		$("div:visible:first").unblock();
	}
	$.hideDialogs = function(blockContainer)
	{
		/*
		 * SWB 8/13
		 * This function hides all currently visible dialogs, or more specifically; closes them. In order to achieve this, the close and 
		 * beforeClose events for the dialogs are stored in data cache, so that when they're shown again these events can be re-bound.
		 * 
		 * Without this, dialogs that have pending changes and implement the updateHelper will warn about losing changes when closing the 
		 * dialog (beforeClose).
		 * 
		 * This function is used almost exlusively to combat the Modal Dialog issue with Redactor inside modal jQuery UI Dialogs
		 */
		
		if (blockContainer) $.blockContainer();
		
		$(".ui-dialog-content:visible").each(function()
		{				
			// Hide any open dialogs (Redactor), reset the close event				
			var win = $("#" + $(this).attr("id"));
			var options = win.dialog("option");
			
			win.data("stored_close", options.close);
			win.data("stored_beforeclose", options.beforeClose);
			
			win.dialog("option", { close: null, beforeClose: null });
			
			win.dialog("close");
		});
	}
	$.showDialogs = function (unblockContainer)
	{
		if (unblockContainer) $.unblockContainer();				
		
		/* 
		 * SWB 8/13
		 * Restore the close and beforeClose events for all of the dialogs, but only if they exist in the data cache snapshot from the time 
		 * they were hidden.
		*/
		
		$(".ui-dialog-content").each(function()
		{
			var win = $("#" + $(this).attr("id"));
			
			var closeFunc = win.data("stored_close");
			var beforeFunc = win.data("stored_beforeclose");
			
			// Have to do these individually, since it only makes sense to restore what was there before hiding; it's possible that other 
			// events have been hooked, or that the topmost window does not have them defined (since it was not hidden)
			if (closeFunc) win.dialog("option", "close", closeFunc);
			if (beforeFunc) win.dialog("option", "beforeClose", beforeFunc);
			
			win.dialog("open");
		});
	}
})(jQuery);