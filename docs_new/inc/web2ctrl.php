<?php
function get_intro_web2ctrl() {
	echo "WebView2 HTML control.<br/>
NB: This control requires WebView2Loader.dll to be present somewhere LoadLibrary() can find it.<br/>
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
	        '__desc' => 'This does the same as /xdid -n but allows setting some flags too.',
	        '__cmd' => '[+FLAGS] [+MASK] (URL)',
	        '__eg' => "+bf +b http://dcx.scriptsdb.org",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Can be any combination of',
					'__values' => array(
						'f' => 'toggle fullscreen on/off',
						's' => 'toggle statusbar on/off',
					),
				),
				'+MASK' => 'is used to set the flags on or off /xdid -m dialog +bf +b will turn the address bar on, & disable fullscreen.',
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
		'C' => array(
	        '__desc' => 'Capture a preview of the current website.',
	        '__cmd' => '[+FLAGS] (FILENAME)',
	        '__eg' => "+ sample.png",
			'__params' => array(
				'+FLAGS' => array(
					'__desc' => 'Can be any combination of',
					'__values' => array(
						'p' => 'png format, flags mean nothing atm, its always png.',
					),
				),
				'FILENAME' => 'The filename to save the image as.',
			),
		),
	);
}

function get_xdidprops_web2ctrl(&$XDIDPROPS) {
	$XDIDPROPS = array(
		"url" => array(
		    '__desc' => "This property lets you retrieve the current loaded url.",
		),
		"statusbar" => array(
		    '__desc' => 'This property lets you retrieve the enabled state of the statusbar. If it is [v]$true[/v] the statusbar is displayed, otherwise it\'s hidden.',
		),
		"statustext" => array(
		    '__desc' => 'This property lets you retrieve the text displayed in the statusbar.',
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
			'__return' => "[r]save filename[/r] return this value to save the favicon."
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
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
		"dl_begin" => array(
			'__desc' => "When downloading is about to begin.",
			'__cmd' => 'TOTALBYTES FILENAME',
			'__eg' => '1000000 c:\downloads\file.zip',
			'__return' => "[r]cancel[/r] return this value to block the download."
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
	);
}
?>
