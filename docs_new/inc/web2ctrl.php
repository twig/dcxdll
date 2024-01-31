<?php
function get_intro_web2ctrl() {
	echo "WebView2 HTML control.<br/>
NB: This control requires WebView2Loader.dll to be present somewhere LoadLibrary() can find it, or in the same directory as dcx.dll.<br/>
NB: The control also requires WebView2 installed & will try to install it if not found.<br/>
<a href='https://developer.microsoft.com/en-us/microsoft-edge/webview2/'>Download WebView2</a>";
}




function get_xdid_web2ctrl(&$XDID) {
	$XDID = array(
	    'g' => array(
	        '__desc' => 'This command is like hitting the Home button in a browser.',
	        '__cmd' => '(URL)',
	        '__eg' => "http://dcx.scriptsdb.org",
			'__notes' => "If a url is supplied then it is set as the home url for all future calls.",
		),
		'i' => array(
	        '__desc' => 'This command is like hitting the Forward button in a browser.',
		),
		'j' => array(
	        '__desc' => 'This command lets you execute some javascript code.',
	        '__cmd' => '[JAVASCRIPT CODE]',
	        '__eg' => "alert('Hello')",
		),
		'k' => array(
	        '__desc' => 'This command is like hitting the Back button in a browser.',
		),
		'm' => array(
	        '__desc' => 'This does the same as [cmd]/xdid -n[/cmd] but allows setting some flags too.',
	        '__cmd' => '[+FLAGS] [+MASK] (URL)',
	        '__eg' => "+bf +b http://dcx.scriptsdb.org",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Can be any combination of',
					'__values' => array(
						'm' => 'toggle Managing new windows.',
						'u' => 'toggle downloads.',
						'U' => 'toggle downloads dialog.',
						'f' => 'toggle fullscreen on/off',
						's' => 'toggle statusbar on/off',
						'A' => 'toggle Mute on/off',
						'C' => 'toggle Default context menus on/off',
						'D' => 'toggle Scripting Dialog on/off',
						'S' => 'toggle Scripting on/off',
						'W' => 'toggle Web Messages on/off',
					),
				),
				'+MASK' => 'is used to set the flags on or off [v]/xdid -m dname +bf +b[/v] will turn the address bar on, & disable fullscreen.',
				'URL' => 'is optional, if not supplied then you can use the command to just set flags.',
			),
		),
		'n' => array(
	        '__desc' => 'This command lets you navigate to an url.',
	        '__cmd' => '[URL]',
	        '__eg' => "http://dcx.scriptsdb.org",
		),
		'r' => array(
	        '__desc' => 'This command is like hitting the Refresh button in a browser.',
		),
		't' => array(
	        '__desc' => 'This command is like hitting the Stop button in a browser.',
		),
		'c' => array(
	        '__desc' => 'Clear the cache.',
	        '__cmd' => '(TYPE)',
	        '__eg' => "cookies",
			'__params' => array(
				'TYPE' => array(
					'__desc' => 'Can be any of',
					'__values' => array(
						'empty' => 'If no argument supplied all caches are cleared.',
						'downloads' => 'Only clear the downloads history.',
						'browsing' => 'Only clear the browsing history.',
						'cookies' => 'Only clear the cookies.',
					),
				),
			),
		),
		'C' => array(
	        '__desc' => 'Capture a preview of the current website.',
	        '__cmd' => '[+FLAGS] (FILENAME)',
	        '__eg' => "+ sample.png",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Can be any one of',
					'__values' => array(
						'p' => 'png format.',
						'j' => 'jpeg format.',
					),
				),
				'FILENAME' => 'The filename to save the image as.',
			),
		),
		'P' => array(
	        '__desc' => 'Open the print dialog for the current website.',
	        '__cmd' => '[+FLAGS]',
	        '__eg' => "+b",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Can be any one of',
					'__values' => array(
						'b' => 'Browser dialog.',
						's' => 'System dialog (default).',
					),
				),
			),
		),
	);
}

function get_xdidprops_web2ctrl(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"url" => array(
		    '__desc' => "This property lets you retrieve the current loaded url.",
		),
		"ready" => array(
		    '__desc' => "This property lets does nothing atm.",
		),
		"statusbar" => array(
		    '__desc' => 'This property lets you retrieve the enabled state of the statusbar. If it is [v]$true[/v] the statusbar is displayed, otherwise it\'s hidden.',
		),
		"fullscreen" => array(
		    '__desc' => 'This property lets you retrieve the fullscreen state of the webview. [v]$true[/v] if fullscreen is enabled.',
		),
		"scripts" => array(
		    '__desc' => 'This property lets you retrieve the scripting state. [v]$true[/v] if scripting enabled.',
		),
		"scriptdialog" => array(
		    '__desc' => 'This property lets you retrieve the scripting dialog state. [v]$true[/v] if the dialog is enabled.',
		),
		"webmessage" => array(
		    '__desc' => 'This property lets you retrieve the web message state. [v]$true[/v] if web messages are enabled.',
		),
		"downloads" => array(
		    '__desc' => 'This property lets you retrieve the downloads state. [v]$true[/v] if downloads are enabled.',
		),
		"downloaddialog" => array(
		    '__desc' => 'This property lets you retrieve the web message state. [v]$true[/v] if the dialog is enabled.',
		),
		"managed" => array(
		    '__desc' => 'This property lets you retrieve the managed window state. [v]$true[/v] if opeing new windows is managed.',
		),
		"statustext" => array(
		    '__desc' => 'This property lets you retrieve the text displayed in the statusbar.',
		),
		"mute" => array(
		    '__desc' => 'This property lets you retrieve the audio mute state. [v]$true[/v] if the audio is muted.',
		),
		"datafolder" => array(
		    '__desc' => 'This property lets you retrieve the data folder being used.',
		),
		"contextmenus" => array(
		    '__desc' => 'This property lets you retrieve the default context menus state. [v]$true[/v] if the menus are enabled.',
		),
		"version" => array(
		    '__desc' => 'This property lets you retrieve the webview2 version.',
		),
	);
}

function get_events_web2ctrl(&$EVENTS) {
	$EVENTS = array(
	    "nav_begin" => array(
			'__desc' => "When the text is clicked.",
			'__cmd' => 'URL',
			'__eg' => 'http://dcx.scriptsdb.org/',
			'__return' => "[r]cancel[/r] return this value to block the navigation."
		),
		"nav_complete" => array(
			'__desc' => "When the control is finished navigating to the URL.",
			'__cmd' => 'URL',
			'__eg' => 'http://dcx.scriptsdb.org/',
		),
		"status" => array(
			'__desc' => "When the statusbar text changes.",
			'__cmd' => 'NEW_TEXT',
			'__eg' => 'New statusbar text',
		),
		"title" => array(
			'__desc' => "when the title changes.",
			'__cmd' => 'NEW_TEXT',
			'__eg' => 'The other page title',
		),
		"favicon" => array(
			'__desc' => "When the favicon changes.",
			'__cmd' => '[changed|saved|failed] [URL|FILE]',
			'__eg' => 'changed https://www.sample.com/favicon.ico',
			'__return' => array(
				"save filename" => "- return this value to save the favicon. (filename must be the full path to the file)"
			),
			'__notes' => 'The [r]save[/r] return value is only valid for [e]changed[/e] events',
		),
		"fullscreen" => array(
			'__desc' => "When the fullscreen state changes.",
			'__cmd' => '[fullscreen state]',
			'__eg' => '1',
			'__return' => "[r]\$false[/r] return this value to stop the change."
		),
		"script" => array(
			'__desc' => "When the script command finishes.",
			'__cmd' => '[Script result]',
			'__eg' => 'whatever the script returned',
		),
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
		"dl_begin" => array(
			'__desc' => "When downloading is about to begin.",
			'__cmd' => 'TOTALBYTES FILENAME',
			'__eg' => '1000000 c:\downloads\file.zip',
			'__return' => array(
				"cancel" => "- return this value to block the download.",
				"change [filename]" => "- return this value to change the filename. (MUST be the full filepath)",
			),
		),
		"dl_progress" => array(
			'__desc' => "Downloading progress update.",
			'__cmd' => 'CURRENT TOTAL FILENAME',
			'__eg' => '259700 1000000 c:\downloads\file.zip',
			'__params' => array(
				'CURRENT' => 'Current dowload position.',
				'TOTAL' => 'Maximum download position.',
				'FILENAME' => 'The filename being downloaded too.',
			),
			'__notes' => array(
				'[p]TOTAL[/p] can be [v]0[/v], so make sure to check this before performing any calculation with it.',
			),
			'__return' => "[r]cancel[/r] return this value to stop the download."
		),
		"dl_canceled" => array(
			'__desc' => "Download has been canceled.",
			'__cmd' => 'REASON FILENAME',
			'__eg' => '1 c:\downloads\file.zip',
			'__params' => array(
				'REASON' => 'The reason WebView2 gave.',
				'FILENAME' => 'The filename being downloaded too.',
			),
		),
		"dl_complete" => array(
			'__desc' => "When the downloading is complete.",
			'__cmd' => 'FILENAME',
			'__eg' => 'c:\downloads\file.zip',
		),
		"cache" => array(
			'__desc' => "Sent when the cache has been cleared.",
			'__cmd' => 'ERRORCODE',
			'__eg' => '1',
		),
		"win_open" => array(
			'__desc' => "When a new window is attempting to open.",
			'__cmd' => 'URL',
			'__eg' => 'https://www.sample.com/',
			'__return' => "[r]cancel[/r] Return this value to block the new window from opening.",
			'__notes' => array(
				'This event ONLY triggers if new window managment is enabled via [cmd]xdid -m[/cmd][v] dname id +m +m[/v].',
			),
		),
		"win_close" => array(
			'__desc' => "When a script is requesting a window be closed.",
			'__cmd' => 'URL',
			'__eg' => 'https://www.sample.com/',
			'__notes' => array(
				'This event ONLY triggers if new window managment is enabled via [cmd]xdid -m[/cmd][v] dname id +m +m[/v].',
				'From the WebView docs:',
				"WindowCloseRequested triggers when content inside the WebView requested to close the window,",
				"such as after window.close is run.The app should close the WebView and related app window if that makes sense to the app.",
			),
		),
		"externaluri" => array(
			'__desc' => "When an external uri scheme is clicked.",
			'__cmd' => 'URI',
			'__eg' => 'irc://irc.sample.com/',
			'__return' => "[r]cancel[/r] Return this value to block navigation.",
		),
		"source_changed" => array(
			'__desc' => "When a source page changes.",
			'__cmd' => 'URL',
			'__eg' => 'https://www.sample.com/',
		),
		"contextmenu" => array(
			'__desc' => "When a context menu is about to be opened.",
			'__cmd' => 'KIND X Y',
			'__eg' => 'COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND_IMAGE 10 10',
			'__return' => "[r]cancel[/r] Return this value to block opening the menu.",
		),
		"proc_error" => array(
			'__desc' => "When an error occurs in the browser process.",
			'__cmd' => 'KIND',
			'__eg' => 'COREWEBVIEW2_PROCESS_FAILED_KIND_BROWSER_PROCESS_EXITED',
			'__return' => "[r]reload[/r] Return this value to attempt to fix the issue and display the page.",
		),
		"devtools" => array(
			'__desc' => "When the browser state changes.",
			'__cmd' => 'STATE',
			'__eg' => 'securityState=secure',
		),
	);
}
?>
