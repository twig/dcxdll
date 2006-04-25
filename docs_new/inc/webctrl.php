<?php
function get_intro_webctrl() {
	echo "Web HTML control.";
}




function get_xdid_webctrl(&$XDID) {
	$XDID = array(
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
		    '__desc' => "This property lets you retreive the current loaded url.",
		),
		"ready" => array(
		    '__desc' => 'This property lets you retreive the ready state. If it is [v]$true[/v] the control is ready for another command, else you should wait.',
		),
	);
}

function get_events_webctrl(&$EVENTS) {
	$EVENTS = array(
	    "nav_begin" => array(
			'__desc' => "When the text is clicked.",
            '__cmd' => 'URL',
			'__return' => "[r]cancel[/r] return this value to block the navigation."
		),
	    "nav_complete" => array(
			'__desc' => "When the control is finished navigating to the URL.",
			'__cmd' => 'URL',
		),
	    "doc_complete" => array(
			'__desc' => "When the document has been downloaded.",
			'__cmd' => 'URL',
		),
	    "dl_begin" => "When downloading is about to begin.",
	    "dl_progress" => array(
			'__desc' => "Downloading progress update.",
			'__cmd' => 'CURRENT TOTAL',
			'__params' => array(
			    'CURRENT' => 'Current dowload position.',
			    'TOTAL' => 'Maximum download position.'
			),
		),
	    "dl_complete" => "When the downloading is complete.",
	    "win_open" => array(
			'__desc' => "When a new window is attempted to be opened.",
			'__return' => "[r]cancel[/r] Return this value to block the new window from opening.",
		),
	    "status" => array(
			'__desc' => "When the statusbar changes.",
			'__cmd' => 'NEW_TEXT',
		),
	    "title" => array(
			'__desc' => "when the title changes.",
			'__cmd' => 'NEW_TEXT',
		),
	    "forward" => array(
			'__desc' => 'When the state of the forward button changes.',
			'__cmd' => 'IS_ENABLED',
			'__notes' => array(
			    '[p]IS_ENABLED[/p] can be either [v]$true[/v] or [v]$false[/v] indicating the enabled state of the button.',
			),
		),
	    "back" => array(
			'__desc' => 'When the state of the back button changes.',
			'__cmd' => 'IS_ENABLED',
			'__notes' => array(
			    '[p]IS_ENABLED[/p] can be either [v]$true[/v] or [v]$false[/v] indicating the enabled state of the button.',
			),
		),
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
