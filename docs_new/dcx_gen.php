<?php
require('dcx_inc.php');
require('dcx_changes.php');

$PAGES = array(
    "index" => "DCX",
    'changes' => 'Version History',
    'xpopup' => 'XPopup',
    'xdock' => 'XDock',
    'cla' => 'Cell Layout Algorithm',
    'dcxvsmdx' => 'DCX vs MDX',
    'archive' => 'Download Archive',
    'tutorials' => 'Tutorials',

	'dcx' => 'DCX Commands',
	'xdialog' => 'Marked Dialog',
	'xdid' => 'Controls',

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
    'dialog' => "Dialog (embedded)",
    'window' => "Window (embedded)",
    'toolbar' => "ToolBar",
    'list' => 'List',
    'scroll' => 'Scroll',
    'edit' => 'Edit',
    'treeview' => 'Treeview',
    'listview' => 'Listview',
    'box' => 'Box',
    'divider' => 'Divider',
    'panel' => 'Panel',
    'tab' => 'Tab',
    'rebar' => 'Rebar',
    'pager' => 'Pager',
);

array_walk_recursive($CHANGES, "wikiData");

// ----------------------------------------------------------------------------
// load up all the files first
foreach ($PAGES as $page => $pagelabel) {
	// include the data files required for generation
	// $STYLES, $X?, $X?PROPS, $EVENTS, etc
	// dont bother loading changes, thats a massive array
	if (!in_array($page, array('changes'))) {
		require_once($INCPATH . $page . ".php");
	}
}

// page generation begins here
// function generate() {}
foreach ($PAGES as $page => $pagelabel) {
	// set variables for timing
	$start = explode(" ", microtime());
	$start = $start[0] + $start[1];

	// create/open file
	if ($page == 'cla')
	    $hfile = fopen($DOCPATH . "layout.htm", "w");
	else
	    $hfile = fopen($DOCPATH . "$page.htm", "w");
    
    // couldnt open file for writing
    if (!$hfile) {
		error_log("Could not open file for writing. Terminating batch.");
		exit();
	}

	// start output buffer
	ob_start();

	// initialise function pointers
	$loadfn = $page . "_load";
	$layoutfn = $page . "_layout";
	$unloadfn = $page . "_unload";

	// if we need to load data for a specialised page
	if (function_exists($loadfn))
		$loadfn($page);

	// write the page ehader and menu for all pages
	dcxdoc_header($page, $pagelabel);
	dcxdoc_menu_left();

	// center info cell
	echo "<td>";

	// since 'changes' isnt a normal dcxdoc include file, just print changes
	if ($page == "changes") {
		$SECTION = SECTION_INTRO;
		echo dcxdoc_print_description("Version History", format_changes());
	}
	// specialised page layout - page must also handle intro, /xdid, $xdid, events, etc
	else if (function_exists($layoutfn)) {
		$layoutfn($page, $pagelabel);
	}
	// otherwise it is a regular control page
	else {
		// write intro
		dcxdoc_print_intro($page);
		// generate styles
		gen_styles($page, $pagelabel);
		// generate /xdid commands
		gen_xdid($page, $pagelabel);
		// generate $xdid props
		gen_xdidprops($page, $pagelabel);
		// generate events
		gen_events($page, $pagelabel);
	}

	// close center info cell
	echo "</td>";
	
	
	// right menu - dont bother for these pages listed below
    if (!in_array($page, array('changes', 'cla', 'dcxvsmdx', 'index', 'tutorials')))
		dcxdoc_menu_right($pagelabel);

	// unload data if it is a specialised page
	if (function_exists($unloadfn)) {
		$unloadfn();
	}

	// unload data, otherwise it will carry onto the next page
	// (usually the right menu will show stuff thats not meant to be there)
	$STYLES = array();
	$XDID = array();
	$XDIDPROPS = array();
	$EVENTS = array();

	// generate footer and date updated
	dcxdoc_footer();
	
	// prepare buffered output for writing and close buffer
	$str = ob_get_clean();
	
	// write page content to file, close file
	fwrite($hfile, $str);
	fclose($hfile);
	
	// show statistics in console
	$end = explode(" ", microtime());
	$end = $end[0] + $end[1];
	
	error_log("Generated $page.htm (" . round(($end - $start) * 1000, 3) . "ms)");
}
// ----------------------------------------------------------------------------
// end page generation
?>