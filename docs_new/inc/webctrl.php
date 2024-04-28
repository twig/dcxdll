<?php
function get_intro_webctrl() {
	echo "Web HTML control.<br/>Root control: <a href='https://learn.microsoft.com/en-gb/windows/win32/controls/static-controls'>Static</a>";
}




function get_xdid_webctrl(&$XDID) {
	$XDID = array(
		'c' => array(
	        '__desc' => 'Send commands to the browser.',
	        '__cmd' => '[+FLAGS] [CMD] (ARGS)',
	        '__eg' => "+s opticalzoom 120",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Can be any of',
					'__values' => array(
						's' => 'Set the value.',
						'd' => 'Display the dialog for setting this value. (may not exist on the version you have)',
					),
				),
				'CMD' => array(
					'__desc' => 'The command to execute.',
					'__values' => array(
						'zoom [N]' => 'Set the zoom factor. (limited to 0 to 4)',
						'opticalzoom [N]' => 'Set the optical zoom percentage. (limited to 10 to 1000)',
						'save' => 'Save the current webpage.',
						'saveas' => 'Save the current webpage via a save dialog.',
						'print' => 'Print the current webpage.',
						'printpreview' => 'View a preview of the current webpage being printed.',
						'pagesetup' => 'Open the print setup dialog.',
						'find' => 'Opens the browsers find dialog.',
						'cut' => 'Cut selected content to clipboard.',
						'copy' => 'Copy selected content to clipboard.',
						'paste' => 'Paste clipboard to carets location in browser.',
						'undo' => 'Undo last action.',
						'redo' => 'Redo action that was previously undone.',
						'selectall' => 'Select all the browsers content.',
						'clearselection' => 'Clear the current selection.',
						'stopdownload' => 'Stop the current download.',
					),
				),
                'ARGS' => "The arguments for the given [v]CMD[/v]",
			),
		),
	    'g' => array(
	        '__desc' => 'This command is like hitting the Home button in IE.',
		),
		'i' => array(
	        '__desc' => 'This command is like hitting the Forward button in IE.',
		),
		'j' => array(
	        '__desc' => 'This command lets you execute some javascript code.',
	        '__cmd' => '[JAVASCRIPT CODE]',
	        '__eg' => "alert('Hello')",
		),
		'k' => array(
	        '__desc' => 'This command is like hitting the Back button in IE.',
		),
		'm' => array(
	        '__desc' => 'This does the same as [cmd]/xdid -n[/cmd] but allows setting some flags too.',
	        '__cmd' => '[+FLAGS] [+MASK] (URL)',
	        '__eg' => "+bf +b http://dcx.scriptsdb.org",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Can be any combination of',
					'__values' => array(
						'h' => 'Disable adding url to history',
						'r' => 'Disables reading url from cache.',
						'w' => 'Disables adding this url to cache.',
						'a' => 'Enabled Auto Search when url fails.',
						'e' => 'Forces url to be opened in the Restricted Zone.',
						'm' => 'Enabled Popup Manager.',
						'u' => 'Disabled downloads.',
						'x' => 'Enable ActiveX Installing (You will still be prompted for allow/disallow)',
						'b' => 'toggle address bar on/off',
						'f' => 'toggle fullscreen on/off',
						's' => 'toggle statusbar on/off',
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
	        '__desc' => 'This command is like hitting the Refresh button in IE.',
		),
		't' => array(
	        '__desc' => 'This command is like hitting the Stop button in IE.',
		),
	);
}

function get_xdidprops_webctrl(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"url" => array(
		    '__desc' => "This property lets you retrieve the current loaded url.",
		),
		"ready" => array(
		    '__desc' => 'This property lets you retrieve the ready state. If it is [v]$true[/v] the control is ready for another command, else you should wait.',
		),
		"statusbar" => array(
		    '__desc' => 'This property lets you retrieve the enabled state of the statusbar. If it is [v]$true[/v] the statusbar is displayed, otherwise it\'s hidden.',
		),
		"statustext" => array(
		    '__desc' => 'This property lets you retrieve the text displayed in the statusbar.',
		),
		"script" => array(
		    '__desc' => 'This property lets you call a script that returns a result.',
			'__cmd' => 'scriptcommand',
			'__eg' => 'document.title',
		),
		"zoom" => array(
		    '__desc' => 'This property lets you retrieve the current zoom factor.',
		),
		"opticalzoom" => array(
		    '__desc' => 'This property lets you retrieve the current optical zoom percentage.',
		),
	);
}

function get_events_webctrl(&$EVENTS) {
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
		"nav_error" => array(
			'__desc' => "When the control has encountered an error navigating to the URL.",
			'__cmd' => 'CODE URL',
			'__eg' => '404 http://dcx.scriptsdb.org/',
			'__notes' => array(
				"The code shown is an example only, don't expect that value.",
			),
		),
		"doc_complete" => array(
			'__desc' => "When the document has been downloaded.",
			'__cmd' => 'URL',
			'__eg' => 'http://dcx.scriptsdb.org/',
		),
		"dl_begin" => "When downloading is about to begin.",
		"dl_progress" => array(
			'__desc' => "Downloading progress update.",
			'__cmd' => 'CURRENT TOTAL',
			'__eg' => '259700 1000000',
			'__params' => array(
				'CURRENT' => 'Current dowload position.',
				'TOTAL' => 'Maximum download position.'
			),
			'__notes' => array(
				'[p]CURRENT[/p] can be [v]-1[/v]. If anyone knows why this occurs, please let us know <a href="http://dcx.scriptsdb.org/bug/?do=details&id=447">here</a>.',
				'[p]TOTAL[/p] can be [v]0[/v], so make sure to check this before performing any calculation with it.',
				'It is advised you use the values provided by [p]CURRENT[/p] and [p]TOTAL[/p] to display progress as these values may change with later revisions of Internet Explorer.',
			),
		),
		"dl_complete" => "When the downloading is complete.",
		"win_open" => array(
			'__desc' => "When a new window is attempted to be opened.",
			'__return' => "[r]cancel[/r] Return this value to block the new window from opening.",
		),
		"win_close" => array(
			'__desc' => "When a window is attempted to be closed.",
			'__cmd' => '[child|main]',
			'__eg' => 'child',
			'__return' => "[r]cancel[/r] Return this value to block the window from closing.",
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
		"forward" => array(
			'__desc' => 'When the state of the forward button changes.',
			'__cmd' => 'ENABLED',
			'__eg' => '$false',
			'__notes' => '[p]ENABLED[/p] can be either [v]$true[/v] or [v]$false[/v] to indiciate the enabled state of the forward button.',
		),
		"back" => array(
			'__desc' => 'When the state of the back button changes.',
			'__cmd' => 'ENABLED',
			'__eg' => '$true',
			'__notes' => '[p]ENABLED[/p] can be either [v]$true[/v] or [v]$false[/v] to indicate the enabled state of the back button.',
		),
		"help" => "Launched when you click on a control using the [v]?[/v] contexthelp button.",
	);
}
?>
