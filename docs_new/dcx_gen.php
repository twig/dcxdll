<?php
require('dcx_inc.php');
require('dcx_changes.php');

// globals
$VERSION = "1.3.1";

$DOCPATH = "./doc/";
$INCPATH = "./inc/";

define('SECTION_INTRO', 1);
define('SECTION_GENERAL', 2);
define('SECTION_STYLES', 3);
define('SECTION_XDID', 4);
define('SECTION_STYLES', 5);
define('SECTION_EVENTS', 8);
define('SECTION_XDIDPROPS', 9);
define('SECTION_XDIALOG', 10);
define('SECTION_XDIALOGPROPS', 11);

$PAGES = array(
    "index" => "DCX",
    'changes' => 'Version History',
    "button" => "Button",
    "pbar" => "Progress Bar",
    "line" => "Line",
    "text" => "Text",
    'link' => 'Link',
    'image' => 'Image',
    'check' => 'Check',
    'radio' => "Radio",
    'calendar' => "Calendar",
    'webctrl' => "Web Control",
    'updown' => 'UpDown',
    'ipaddress' => "IP Address",
    'colorcombo' => "ColorCombo",
    'richedit' => 'RichEdit',
    'trackbar' => 'TrackBar',
    'comboex' => 'ComboEx',
    'statusbar' => "StatusBar",
    'dialog' => "Dialog",
    'window' => "Window",
    'toolbar' => "ToolBar",
    'list' => 'List',
    'scroll' => 'Scroll',
    'edit' => 'Edit',
);

$XDID = array();
$XDIDPROPS = array();
$XDIALOG = array();
$XDIALOGPROPS = array();
$EVENTS = array();
$GENERAL = array();
$STYLES = array();

array_walk_recursive($CHANGES, "wikiData");

$SECTION = 0;

// ----------------------------------------------------------------------------
// page generation begins here
// function generate() {}
foreach ($PAGES as $page => $pagelabel) {
	$start = explode(" ", microtime());
	$start = $start[0] + $start[1];

	// create/open file
    $hfile = fopen($DOCPATH . "$page.htm", "w");
    
    // couldnt open
    if (!$hfile) {
		error_log("Could not open file for writing. Terminating batch.");
		return;
	}

	// include the data files required for generation
	// $STYLES, $XDID (command flags), $INFO (property flags), $EVENTS
	if (!in_array($page, array('changes'))) {
		require_once($INCPATH . $page . ".php");
	}

	// retrieve data for processing
	loadSection($XDID, "get_xdid_$page");
	loadSection($XDIALOG, "get_xdialog_$page");
	loadSection($EVENTS, "get_events_$page");
	loadSection($XDIDPROPS, "get_xdidprops_$page");
	loadSection($XDIALOGPROPS, "get_xdialogprops_$page");
	loadSection($GENERAL, "get_general_$page");
	loadSection($STYLES, "get_styles_$page");

	// start output buffer
	ob_start();
	
	dcxdoc_header($page, $pagelabel);
	dcxdoc_menu_left();

	// center info cell
	echo "<td>";
	
	// info/intro
	dcxdoc_print_intro($page);
	
	if ($page == "index") {
        dcxdoc_print_description("Latest Changes", format_changes_latest());
	    intro2_index();
	    callback_index();
	}
	
	if ($page == "changes") {
        echo format_changes();
	}

	// general commands
	if ($GENERAL) {
		$SECTION = SECTION_GENERAL;
		$count = 1;

		dcxdoc_print_description("General Commands", "These commands are general DCX commands and not oriented on any particular DCX controls.");

		foreach ($GENERAL as $cmd => $data) {
       		dcxdoc_format_general($cmd, $data, $count);
      		$count++;
		}
	}
	
	// general commands
	if ($STYLES) {
		$SECTION = SECTION_STYLES;

		dcxdoc_print_description("Control Styles", "These control styles are available when creating a $pagelabel control. Remember that the general styles [s]disabled[/s], [s]group[/s], [s]notheme[/s], and [s]tabstop[/s] apply to all DCX controls.");

   		dcxdoc_format_styles($STYLES);
	}

	// /xdialog commands
	if ($XDIALOG) {
		$SECTION = SECTION_XDIALOG;
		$count = 1;

		dcxdoc_print_description("/xdialog flags", "The /xdialog command is used to modify a DCX marked dialog.");

		foreach ($XDIALOG as $flag => $data) {
       		dcxdoc_format_xdialog($flag, $data, $count);
      		$count++;
		}
	}

	// /xdialog properties
	if ($XDIALOGPROPS) {
		$SECTION = SECTION_XDIALOGPROPS;
		$count = 1;

		dcxdoc_print_description('$xdialog() Properties', 'The $xdialog identifier is a given mIRC alias that communicates with the DCX DLL to extract information in DCX dialogs.');

		foreach ($XDIALOGPROPS as $prop => $data) {
	        dcxdoc_format_xdialogprop($prop, $data, $count);
	        $count++;
  		}
	}

	// /xdid commands
	if ($XDID) {
		$SECTION = SECTION_XDID;
		$count = 1;

		dcxdoc_print_description("/xdid flags", "Control commands are input to the control with the <b>/xdid</b> command.");

		foreach ($XDID as $flag => $data) {
	        dcxdoc_format_xdid($flag, $data, $count);
	        $count++;
  		}
	}

	// /xdid properties
	if ($XDIDPROPS) {
		$SECTION = SECTION_XDIDPROPS;
		$count = 1;

		dcxdoc_print_description('$xdid() Properties', 'The $xdid identifier is a given mIRC alias that communicates with the DCX DLL to extract information in DCX controls.');

		foreach ($XDIDPROPS as $prop => $data) {
	        dcxdoc_format_xdidprop($prop, $data, $count);
	        $count++;
  		}
	}

	// events
	if ($EVENTS) {
		$SECTION = SECTION_EVENTS;
		$count = 1;

		if ($page == 'index')
			$str = "These events are fired when events occur in the dialog itself.";
		else
		    $str = "These events are fired when activity occurs in the $pagelabel control.";

		dcxdoc_print_description("$pagelabel Events", $str);

		foreach ($EVENTS as $event => $data) {
	        dcxdoc_format_event($event, $data, $count);
	        $count++;
  		}
	}

	if ($page == "index") {
		$SECTION = 0;
		credits_index();
	}

	// close center info cell
	echo "</td>";
	
	
	// right menu
    if (!in_array($page, array('changes')))
		dcxdoc_menu_right($pagelabel);

	// footer
	dcxdoc_footer();
	
	// get buffered output
	$str = ob_get_clean();
	
	// write page content to file
	fwrite($hfile, $str);
	fclose($hfile);
	
	// show statistics
	$end = explode(" ", microtime());
	$end = $end[0] + $end[1];
	
	error_log("Generated $page.htm (" . round(($end - $start) * 1000, 3) . "ms)");
}
// ----------------------------------------------------------------------------
// end page generation
?>
