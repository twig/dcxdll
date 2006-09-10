<?php
// defines
define('SECTION_INTRO', 1);
define('SECTION_GENERAL', 2);
define('SECTION_STYLES', 3);
define('SECTION_XDID', 4);
define('SECTION_EVENTS', 8);
define('SECTION_XDIDPROPS', 9);
define('SECTION_XDIALOG', 10);
define('SECTION_XDIALOGPROPS', 11);
define('SECTION_XPOPUP', 12);
define('SECTION_XPOPUPPROPS', 13);
define('SECTION_XPOP', 14);
define('SECTION_XPOPPROPS', 15);
define('SECTION_XDOCK', 16);
define('SECTION_XDOCKPROPS', 17);


// global variables
$VERSION = "1.3.6";

$DOCPATH = "./doc/";
$INCPATH = "./inc/";

$XDID = array();
$XDIDPROPS = array();
$XDIALOG = array();
$XDIALOGPROPS = array();
$XDOCK = array();
$XDOCKPROPS = array();
$EVENTS = array();
$GENERAL = array();
$STYLES = array();
$XPOPUP = array();
$XPOPUPPROPS = array();
$XPOP = array();
$XPOPPROPS = array();

$SECTION = 0;


// information to be displayed in the CLA details (for dialog, panel, box, etc)
$CLA = array(
	'__desc' => "This command lets you add Cell Layout Algorithm rules to your dialog controls for automatic resizing of the child controls.",
	'__cmd' => "[COMMAND] [PATH] [TAB] [+FLAGS] [CID] [WEIGHT] [W] [H]",
	'__eg' => array(
		"root \$chr(9) +pv 0 1 0 0",
		"root 1 2 \$chr(9) +fi 7 1 200 300",
		"space 1 \$chr(9) + 5 5 5 5",
	),
	'__params' => array(
		'COMMAND' => array(
			'__desc' => "Layout command.",
			'__values' => array(
				'root' => "Sets the root cell element.",
				'cell' => "Adds a cell element as a child of another cell element.",
				'space' => "Sets the space padding around a cell element.",
				'update' => "Updates the cell layout (must be used after adding rules or changing padding settings to refresh display).",
			),
		),
		'PATH' => "Cell Path from [v]root[/v] element (can be root to point to root element).",
		'+FLAGS' => array(
            '__desc' => "Flags to manipulate Cell Layout Algorithm.",
			'__values' => array(
				'f' => "Creates a fixed cell element.",
				'h' => "Creates a width fixed cell if used with [v]+f[/v] or a horizontal cell pane if used with [v]+p.[/v]",
				'i' => "Indicates that the supplied control [p]ID[/p] is valid and is the child control when used with [v]+f[/v] or [v]+l[/v].",
				'l' => "Creates a fill cell element.",
				'p' => "Creates a pane cell element.",
				'v' => "Creates a height fixed cell if used with [v]+f[/v] or a vertical cell pane if used with [v]+p[/v].",
				'w' => "When used, it means that the supplied [p]W[/p] and [p]H[/p] represent the fixed cell width in the applicable direction (if the fixed cell is fixed in width, height or both).",
			),
		),
		'CID' => "ID of the control (used with fixed or fill cells - use 0 if no control is to be linked to the cell).",
		'WEIGHT' => "Cell child weight. (used when adding a child cell to a pane cell)",
		'W' => "Fixed width of control (used with [v]fixed[/v] cell).",
		'H' => "Fixed height of control. (used with [v]fixed[/v] cell)",
	),
	'__notes' => array(
	    "See the <a href='layout.htm'>Cell Layout Algorithm</a> explanation for concrete examples.",
	    "If you use the [v]update[/v] command in the [e]init[/e] event of a dialog, you will have to use \".timer 1 0 xdialog -l dialog update\" as a glitch with XP and themes disabled prevents windows from appearing. The timer corrects this problem.",
	    "When using the [v]space[/v] command, the [p]ID[/p] [p]WEIGHT[/p] [p]W[/p] [p]H[/p] are in fact the [p]LEFT[/p] [p]TOP[/p] [p]RIGHT[/p] [p]BOTTOM[/p] spacings",
	    "Use the value 0 for the fields not used as the command expects them to be filled even though they aren't used.",
	),
);


function loadSection(&$arr, $fn) {
	if (function_exists($fn)) {
		$fn($arr);
		array_walk_recursive($arr, "wikiData");
		ksort($arr, SORT_STRING);
	}
	else {
		$arr = array();
	}
}

// php5 function, slightly tweaked
if (!function_exists('array_walk_recursive')) {
	function array_walk_recursive(&$input, $funcname, $userdata = "") {
		if (!is_callable($funcname))
			return false;

		if (!is_array($input))
			return false;

		foreach ($input AS $key => $value) {
			if (is_array($input[$key]))
				array_walk_recursive($input[$key], $funcname, $userdata);
			else {
				$saved_value = $value;

				if (!empty($userdata))
					$funcname($value, $userdata);
				else
					$funcname($value);

				if ($value != $saved_value)
					$input[$key] = $value;
			}
		}

		return true;
	}
}

// do formatting of wiki code here
// http://www.phpit.net/article/create-bbcode-php/
function wikiData(&$value, $userdata = "") {
/*
	// old code
	if (preg_match_all('/\\[([vesproi]+)\\](.+?)\\[\/([vesproi]+)\\]/', $value, $regs)) {
		foreach ($regs[0] as $id => $matchtext) {
			$style = '';
			$format = '';

			// $regs[2][$id] = text in the tags
			switch ($regs[1][$id]) {
				case 'v': $style = 'value'; break;
				case 'e': $style = 'event'; break;
				case 'r': $style = 'eventreturn'; break;
				case 's': $style = 'style'; break;
				case 'p': $style = 'param'; break;
				case 'i': $style = 'property'; break;
				case 'o': $style = 'os'; $format = "({$regs[2][$id]})"; break;
			}

		    // perform replacement
			if ($style) {
				if (!$format)
				    $format = $regs[2][$id];

				$value = str_replace($matchtext, "<a class=\"$style\">$format</a>", $value);
			}
		}
	}
*/

	// new code
	$str = $value;

	// TODO: links for tags
	$simple_search = array(
		'/\[v\](.*?)\[\/v\]/is', // value
		'/\[e\](.*?)\[\/e\]/is', // event
		'/\[r\](.*?)\[\/r\]/is', // returnevent
		'/\[s\](.*?)\[\/s\]/is', // style
		'/\[p\](.*?)\[\/p\]/is', // param
//		'/\[i\](.*?)\[\/i\]/is', // property - unused
		'/\[o\](.*?)\[\/o\]/is', // operating system
		'/\[n\](.*?)\[\/n\]/is', // note
		'/\[f\](.*?)\[\/f\]/is', // flag
	);

	$simple_replace = array(
		'<a class="value">$1</a>',
		'<a class="event">$1</a>',
		'<a class="returnevent">$1</a>',
		'<a class="style">$1</a>',
		'<a class="param">$1</a>',
//		'<a class="property">$1</a>',
		'<a class="os">($1)</a>',
		'<a class="note">Note:</a> $1',
		'<a class="value">$1</a>', // flag
	);

	// Do simple BBCode's
	$value = preg_replace($simple_search, $simple_replace, $str);
}


// Generates styles for the specified page
function gen_styles($page, $pagelabel) {
	global $STYLES, $SECTION;
	
	loadSection($STYLES, "get_styles_$page");
	
	// control styles
	if ($STYLES) {
		$SECTION = SECTION_STYLES;

		dcxdoc_print_description("Control Styles", "These control styles are available when creating a $pagelabel control. Remember that the general styles [s]disabled[/s], [s]group[/s], [s]notheme[/s], and [s]tabstop[/s] apply to all DCX controls.");

   		dcxdoc_format_styles($STYLES);
	}
}


// Generates /xdid commands for the specified page
function gen_xdid($page, $pagelabel) {
	global $XDID, $SECTION;
	
	loadSection($XDID, "get_xdid_$page");
	
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
}


// Generates xdid properties for the specified page
function gen_xdidprops($page, $pagelabel) {
	global $XDIDPROPS, $SECTION;
	
	loadSection($XDIDPROPS, "get_xdidprops_$page");
	
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
}


// Generates events for the specified page
function gen_events($page, $pagelabel) {
	global $EVENTS, $SECTION;
	
	loadSection($EVENTS, "get_events_$page");
	
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
}


function dcxdoc_header($page, $pagelabel) {
	global $VERSION;

	echo "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?".">";
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

<title><?php
// Prevent DCX DCX vs MDX
if ($page == "dcxvsmdx")
	echo $pagelabel;
// eg. DCX Box
else if ($page != 'index')
	echo "DCX $pagelabel";
// Prevent DCX DCX
else
	echo "DCX";
?> - Dialog Control Xtension DLL v<?php echo $VERSION; ?> by ClickHeRe, twig* &amp; Ook</title>

<link href="dcx.css" rel="stylesheet" type="text/css" />
</head>

<body>

<table>
	<tr>
		<td>
			<a href="index.htm"><img src="images/dcx.png" alt="DCX - Dialog Control Xtension" /></a>
		</td>
		<td style="vertical-align: bottom; text-align: right;">
<?php
/*
			<a href="translations/nl/index.htm">Dutch</a> |
			<a href="index.htm">English</a> |
			<a href="translations/fi/index.htm">Finnish</a> |
			<a href="translations/fr/index.htm">French</a> |
			<a href="translations/ge/index.htm">German</a> |
			<a href="translations/he/index.htm">Hebrew</a> |
			<a href="translations/it/index.htm">Italian</a> |
			<a href="translations/my/index.htm">Malaysian</a> |
			<a href="translations/br/index.htm">Portuguese (Brazil)</a> |
			<a href="translations/pt/index.htm">Portuguese</a> |
			<a href="translations/sp/index.htm">Spanish</a> |
			<a href="translations/sw/index.htm">Swedish</a>
*/
?>
			<a href='index.htm'>Home</a> |
			<a href='http://dcx.scriptsdb.org/translations'>Translations</a> |
			<a href="archive.htm">Download</a> |
			<a href="http://dcx.scriptsdb.org/forum/">Forum</a> |
			<a href="http://dcx.scriptsdb.org/bug/">Bug Tracker</a>
		</td>
	</tr>
</table>

<table class="bar">
	<tr>
		<td>
<?
/*
			<a href='index.htm'>Home</a> |
			<a href="archive.htm">Download</a> |
			<a href="http://dcx.scriptsdb.org/forum/">Forum</a> |
			<a href="http://dcx.scriptsdb.org/bug/">Bug Tracker</a> |
*/
?>
			&nbsp;
		</td>
		<td style="text-align: center;">Dialog Control Xtension<?php
	if ($page == 'xpopup')
	    echo " - $pagelabel Menus";
	else if (!in_array($page, array('index')))
	    echo " - $pagelabel";
?></td>
		<td style="text-align: right;">v<?php echo $VERSION; ?></td>
	</tr>
</table>

<table>
	<tr>
<?php
}


function dcxdoc_menu_left() {
	global $PAGES;
?><td class="menuleft">
	<br />
	<a class="section" href="#<?php echo SECTION_INTRO; ?>">Introduction</a><br />
	<br />
	<a class="section">DCX Controls</a><br />
	<ul>
<?php
	$pages = $PAGES;
	asort($pages);

	foreach ($pages as $page => $pagelabel) {
		if (in_array($page, array('index', 'changes', 'xpopup', 'cla', 'dcxvsmdx', 'archive', 'xdock')))
			continue;

	    echo "<li><a href=\"$page.htm\">$pagelabel</a></li>";
	}
?>
	</ul>
	<a class="section">DCX Features</a><br />
	<ul>
		<li><a href="layout.htm">Cell Layout Algorithm</a></li>
		<li><a href="xdock.htm">XDock</a></li>
		<li><a href="xpopup.htm">XPopup</a></li>
	</ul>
	<a class="section">About DCX</a><br />
	<ul>
		<li><a href="archive.htm">Download Archive</a></li>
		<li><a href="changes.htm">Version History</a></li>
		<li><a href="dcxvsmdx.htm">DCX vs MDX</a></li>
    </ul>
</td>
<?php
}


function dcxdoc_menu_right($page) {
	global $SECTION, $XDID, $XDIALOG, $XDIDPROPS, $XDIALOGPROPS, $EVENTS, $GENERAL,
	$STYLES, $XPOPUP, $XPOPUPPROPS, $XPOP, $XPOPPROPS, $XDOCK, $XDOCKPROPS;

?><td class="menuright">
	<br />
<?php
    print_menu_items($GENERAL, SECTION_GENERAL, "General Commands");
    print_menu_items($STYLES, SECTION_STYLES, "Control Styles");
    print_menu_items($XDIALOG, SECTION_XDIALOG, "/xdialog Flags");
    print_menu_items($XDIALOGPROPS, SECTION_XDIALOGPROPS, "\$xdialog() Properties");
    print_menu_items($XDID, SECTION_XDID, "/xdid Flags");
    print_menu_items($XDIDPROPS, SECTION_XDIDPROPS, "\$xdid() Properties");
    
    print_menu_items($XPOPUP, SECTION_XPOPUP, "/xpopup Flags");
    print_menu_items($XPOPUPPROPS, SECTION_XPOPUPPROPS, "\$xpopup() Properties");
    print_menu_items($XPOP, SECTION_XPOP, "/xpop Flags");
    print_menu_items($XPOPPROPS, SECTION_XPOPPROPS, "\$xpop() Properties");
    
    print_menu_items($XDOCK, SECTION_XDOCK, "/xdock Commands");
    print_menu_items($XDOCKPROPS, SECTION_XDOCKPROPS, "\$xdock() Properties");
    
    print_menu_items($EVENTS, SECTION_EVENTS, "Events");
	//echo "<a href=\"#\">$page Notes</a><br />";
?>
</td>
<?php
}

function print_menu_items(&$arr, $sec, $sectext) {
	global $SECTION;

	if (count($arr)) {
		$color = get_section_color($sec);
		$count = 1;
		$prefix = in_array($sec, array(SECTION_XDID, SECTION_XDIALOG)) ? '-' : '';

		echo "<a href=\"#$sec\" style=\"color: $color;\">$sectext</a><br /><ul>";

		foreach (array_keys($arr) as $flag) {
			if (preg_match("/__.+/i", $flag))
			    continue;
			
			echo "<li><a href=\"#$sec.$count\" style=\"color: $color;\">$prefix$flag</a></li>";
			$count++;
		}

		echo "</ul>";
	}
}


function dcxdoc_footer() {
?>  </tr>
</table>

<table class="bar">
	<tr>
	    <td><a href="mailto:clickhere@scriptsdb.org">Contact</a></td>
		<td style="text-align: center;">© 2005-2006</td>
		<td style="text-align: right;">Last Updated: <?php echo date("jS F, y"); ?></td>
	</tr>
</table>
<div style="text-align: center;">
	<br />
	<a href="http://validator.w3.org/check?uri=referer">
		<img src="images/valid_xhtml.png" alt="Valid XHTML 1.0 Transitional" />
	</a>
	<a href="http://jigsaw.w3.org/css-validator/check/referer">
		<img src="images/valid_css.png" alt="Valid CSS!" />
	</a>
</div>
</body>
</html>
<?php
}


// call generic function
function dcxdoc_format_general($cmd, &$data, $id) {
    format_xcmd(SECTION_GENERAL, $cmd, $data, $id);
}

function dcxdoc_format_xdialog($flag, &$data, $id) {
    format_xcmd(SECTION_XDIALOG, $flag, $data, $id);
}
function dcxdoc_format_xdialogprop($prop, $data, $id) {
    format_xcmd(SECTION_XDIALOGPROPS, $prop, $data, $id);
}

function dcxdoc_format_xdid($flag, &$data, $id) {
    format_xcmd(SECTION_XDID, $flag, $data, $id);
}
function dcxdoc_format_xdidprop($prop, $data, $id) {
    format_xcmd(SECTION_XDIDPROPS, $prop, $data, $id);
}

function dcxdoc_format_event($event, $data, $count) {
    format_xcmd(SECTION_EVENTS, $event, $data, $count);
}

function dcxdoc_format_xpopup($event, $data, $count) {
    format_xcmd(SECTION_XPOPUP, $event, $data, $count);
}
function dcxdoc_format_xpopupprop($event, $data, $count) {
    format_xcmd(SECTION_XPOPUPPROPS, $event, $data, $count);
}

function dcxdoc_format_xpop($event, $data, $count) {
    format_xcmd(SECTION_XPOP, $event, $data, $count);
}
function dcxdoc_format_xpopprops($event, $data, $count) {
    format_xcmd(SECTION_XPOPPROPS, $event, $data, $count);
}

function dcxdoc_format_xdock($event, $data, $count) {
	format_xcmd(SECTION_XDOCK, $event, $data, $count);
}
function dcxdoc_format_xdockprops($event, $data, $count) {
	format_xcmd(SECTION_XDOCKPROPS, $event, $data, $count);
}

function format_xcmd($section, $flag, $data, $id) {
	if (!is_array($data)) {
		$data = array('__desc' => $data);
	}

	if (!isset($data['__desc'])) {
		error_log("ERROR: __desc not set for $flag");
		return;
	}

	// error_log("ERROR: __cmd not set for $flag");
	if (!isset($data['__cmd']))
		$data['__cmd'] = '';

	// TODO: make this support multiple examples
	// error_log("ERROR: __eg not set for $flag");
	if (!isset($data['__eg']))
		$data['__eg'] = array('');
	// if single value, format it into an array to make it easier to manipulate later
	else if (!is_array($data['__eg'])) {
		$data['__eg'] = array($data['__eg']);
	}

    // error_log("ERROR: __return not set for $flag");
	if (!isset($data['__return']))
		$data['__return'] = '';

	// generate syntax
	$color = get_section_color($section);
	$heading = "HEADING GENERATION ERROR";
	$syntax = "SYNTAX GENERATION ERROR";
	$example = "EXAMPLE GENERATION ERROR";
	
	// set up the information header for the xcmd
	if (!format_xcmd_header($section, $heading, $syntax, $example, $flag, $data)) {
		error_log("format_xcmd: Unknown type $section");
	    exit();
	}
?>
<table class="<?php echo get_section_name($section); ?>">
	<tr><td colspan="2" class="flag" style="<?php echo "color: $color; border-color: $color;";?>">
			<a name="<?php echo "$section.$id"; ?>"></a><?php echo $heading; ?>
	</td></tr>
	<tr><td colspan="2"><?php echo $data['__desc']; ?></td></tr>
	<tr>
		<td class="syntax">Syntax:</td>
		<td><div class="syntax"><?php echo $syntax; ?></div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example"><?php echo $example; ?></div></td>
	</tr>
<?php

	// parameters
	if (isset($data['__params']) && count($data['__params']))
	  format_parameters($data['__params']);

	// event return calls
	if ($data['__return'])
	  format_return($data['__return']);


	// notes
	if (isset($data['__notes']))
		format_notes($data['__notes']);

?>
</table>
<br />
<?php
}

function format_xcmd_header($section, &$heading, &$syntax, &$example, $flag, &$data) {
	$examplefmt = array("", "");
	$ARGS = 0;
	$NOARGS = 1;
	
	switch ($section) {
		case SECTION_GENERAL:
		    if (isset($data['__isid']) && $data['__isid']) {
                $heading = "\$dcx($flag)";
				$syntax = "\$dcx($flag" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ")";
//				$example = "\$dcx($flag" . ($data['__eg'] ? ", {$data['__eg']}" : '') . ")";
				$examplefmt[$ARGS]   = "\$dcx($flag, [-EXAMPLE])";
				$examplefmt[$NOARGS] = "\$dcx($flag)";
			}
			else {
                $heading = "/dcx $flag";
				$syntax = "/dcx $flag " . ($data['__cmd'] ? $data['__cmd'] : '');
//				$example = "/dcx $flag " . ($data['__eg'] ? $data['__eg'] : '');
				$examplefmt[$ARGS]   = "/dcx $flag [-EXAMPLE]";
				$examplefmt[$NOARGS] = "/dcx $flag";
			}
		    break;
		    
		case SECTION_XDID:
			$heading = "/xdid -$flag";
			$syntax = "/xdid -$flag [DNAME] [ID] {$data['__cmd']}";
//			$example = "/xdid -$flag dcx 4 {$data['__eg']}";
			$examplefmt[$ARGS]   = "/xdid -$flag dcx 4 [-EXAMPLE]";
			$examplefmt[$NOARGS] = "/xdid -$flag dcx 4";
			break;

		case SECTION_XDIDPROPS:
			$heading = "\$xdid().$flag";
			$syntax = "\$xdid(dialog, ID" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ").$flag";
//			$example = "\$xdid(dcx, 4" . ($data['__cmd'] ? ", {$data['__eg']}" : '') . ").$flag";
			$examplefmt[$ARGS]   = "\$xdid(dcx, 4, [-EXAMPLE]).$flag";
			$examplefmt[$NOARGS] = "\$xdid(dcx, 4).$flag";
			break;
			
		case SECTION_XDIALOG:
			$heading = "/xdialog -$flag";
			$syntax = "/xdialog -$flag [DNAME] {$data['__cmd']}";
//			$example = "/xdialog -$flag dcx {$data['__eg']}";
			$examplefmt[$ARGS]   = "/xdialog -$flag dcx [-EXAMPLE]";
			$examplefmt[$NOARGS] = "/xdialog -$flag dcx";
			break;
			
		case SECTION_XDIALOGPROPS:
			$heading = "\$xdialog().$flag";
			$syntax = "\$xdialog(dialog" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ").$flag";
//			$example = "\$xdialog(dcx" . ($data['__cmd'] ? ", {$data['__eg']}" : '') . ").$flag";
			$examplefmt[$ARGS]   = "\$xdialog(dcx, [-EXAMPLE]).$flag";
			$examplefmt[$NOARGS] = "\$xdialog(dcx).$flag";
		    break;
		    
        case SECTION_EVENTS:
            $dialogspecific = array('mouseleave', 'mouseenter', 'mouse', 'focusout', 'focus');
            
        	$heading = "$flag";
			$syntax = "/cb_alias DNAME $flag ID {$data['__cmd']}";
			// TODO: fix this since index page has 0 for ID when its a dialog
//			$example = "/cb_alias dcx $flag " . (!in_array($flag, $dialogspecific) ? ' 0 ' : ' 4 ') . $data['__eg'];
//			$example = "/cb_alias dcx $flag 4 {$data['__eg']}";
			$examplefmt[$ARGS]   = "/cb_alias dcx $flag 4 [-EXAMPLE]";
			$examplefmt[$NOARGS] = "/cb_alias dcx $flag 4";
			
            break;
            
        case SECTION_XPOPUP:
			$heading = "/xpopup -$flag";
			$syntax = "/xpopup -$flag [MENU] {$data['__cmd']}";
//			$example = "/xpopup -$flag mymenu {$data['__eg']}";
			$examplefmt[$ARGS]   = "/xpopup -$flag mymenu [-EXAMPLE]";
			$examplefmt[$NOARGS] = "/xpopup -$flag mymenu";
			break;
			
        case SECTION_XPOPUPPROPS:
			$heading = "\$xpopup().$flag";
			$syntax = "\$xpopup(MENU" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ").$flag";
//			$example = "\$xpopup(mymenu" . ($data['__cmd'] ? ", {$data['__eg']}" : '') . ").$flag";
			$examplefmt[$ARGS]   = "\$xpopup(mymenu, [-EXAMPLE]).$flag";
			$examplefmt[$NOARGS] = "\$xpopup(mymenu).$flag";
		    break;
		    
		case SECTION_XPOP:
			$heading = "/xpop -$flag";
			$syntax = "/xpop -$flag [MENU] [PATH] {$data['__cmd']}";
//			$example = "/xpop -$flag mymenu 2 1 {$data['__eg']}";
			$examplefmt[$ARGS]   = "/xpop -$flag mymenu 2 1 [-EXAMPLE]";
			$examplefmt[$NOARGS] = "/xpop -$flag mymenu 2 1";
			break;
			
        case SECTION_XPOPPROPS:
			$heading = "\$xpop().$flag";
			$syntax = "\$xpop(MENU, PATH" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ").$flag";
//			$example = "\$xpop(mymenu, 2 5" . ($data['__cmd'] ? ", {$data['__eg']}" : '') . ").$flag";
			$examplefmt[$ARGS]   = "\$xpop(mymenu, 2 5, [-EXAMPLE]).$flag";
			$examplefmt[$NOARGS] = "\$xpop(mymenu, 2 5).$flag";
		    break;
		    
		case SECTION_XDOCK:
			$heading = "FIXME /xdock $flag";
			$syntax = "/xdock $flag [UNKNOWN] {$data['__cmd']}";
			$examplefmt[$ARGS]   = "/xdock $flag [STILL_WIP] [-EXAMPLE]";
			$examplefmt[$NOARGS] = "/xdock $flag [STILL_WIP]";
			break;
			
        case SECTION_XDOCKPROPS:
			$heading = "DONT KNOW \$xdock().$flag";
			$syntax = "\$xdock(MENU" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ").$flag";
			$examplefmt[$ARGS]   = "\$xdock(mymenu, [-EXAMPLE]).$flag";
			$examplefmt[$NOARGS] = "\$xdock(mymenu).$flag";
		    break;

		// unknown section type, failed
		default:
		    return false;
	}
	
	// handle multiple examples
	$egcount = count($data['__eg']);
	$egcurrent = 1;
	$egtemp = "";
	
	foreach ($data['__eg'] as $eg) {
		$tmp = "";
		
		// if example has a value
		if ($eg)
			$tmp = str_replace("[-EXAMPLE]", $eg, $examplefmt[$ARGS]);
		// otherwise just use a fixed example
		else
			$tmp = $examplefmt[$NOARGS];
		
		// append it to the string buffer
		$egtemp = ($egtemp ? $egtemp : "") . $tmp . ($egcurrent != $egcount ? "<br />" : "");
	}
	
	$example = $egtemp;
	return true;
}


/*
prints the parameter array out into formatted table rows
*/
function format_parameters(&$data) {
?>
	<tr><td colspan="2"><b>Parameters:</b></td></tr>
<?php
	foreach ($data as $param => $val) {
		// if theres only a description for the parameter
		if (!is_array($val)) {
?>
	<tr>
		<td class="param"><?php echo $param; ?></td>
		<td><?php echo $val; ?></td>
	</tr>
<?php
		}
		// show parameter desc, then list each value
		else {
?>
	<tr>
		<td class="param"><?php echo $param; ?></td>
		<td><?php echo $val['__desc']; ?></td>
	</tr>
<?php
			if (isset($val['__values'])) {
				foreach ($val['__values'] as $p => $info) {
?>
	<tr>
		<td class="value"><?php echo $p; ?></td>
		<td><?php echo $info; ?></td>
	</tr>
<?php
				}
			}
		}
	}
}


function format_notes(&$data) {
	echo "<tr><td colspan='2'>&nbsp;</td></tr>";

	if (!is_array($data))
	    $data = array($data);

	$count = 0;
	$total = count($data);
	$prefix = ($total > 1 ? '<ul>' : ' ');

	echo "<tr><td colspan='2'><a class=\"note\">Note.</a>$prefix";

	foreach ($data as $note) {
		if ($total > 1)
			echo "<li>$note</li>";
		else
		    echo "$note";
		
		if (!$count)
		    $prefix = '';
		    
		$count++;
	}
	
	if ($total > 1)
	    echo "</ul>";
	
	echo "</td></tr>";
}


function format_return(&$data) {
	$txt = "";
	
	if (!is_array($data))
    	$txt = $data;
	else {
		foreach ($data as $k => $ret) {
			$txt .= ($txt ? "<br />" : "") . '[r]' . $k . '[/r] ' . $ret; 
		}
		
		wikiData($txt);
	}
	
	echo "<tr><td class=\"syntax\">Return</td><td>$txt</td></tr>";
}

// returns the color associated with a particular section
function get_section_color($section = 0) {
	if (!$section) {
		global $SECTION;
		$section = $SECTION;
	}

	switch ($section) {
        case SECTION_GENERAL		: return '#888888'; // grey
        case SECTION_XDIALOG		: return '#800080'; // purple
	    case SECTION_XDIALOGPROPS	: return '#AC59AC'; // light purple
        
		case SECTION_STYLES			: return '#B52929'; // brown
		case SECTION_XDID			: return '#0000FF'; // blue
		case SECTION_XDIDPROPS		: return '#6666FF'; // light blue
		case SECTION_EVENTS			: return '#009900'; // green

		case SECTION_XPOPUP			: return '#800080'; // purple
		case SECTION_XPOPUPPROPS	: return '#AC59AC'; // light purple
        case SECTION_XPOP			: return '#0000FF'; // blue
		case SECTION_XPOPPROPS		: return '#6666FF'; // light blue

		case SECTION_XDOCK			: return '#0000FF'; // blue
		case SECTION_XDOCKPROPS		: return '#6666FF'; // light blue
		
		case SECTION_INTRO:
		default:
			return '#000000';
	}
}

// returns the textual name, given the section id
function get_section_name($section = 0) {
	if (!$section) {
		global $SECTION;
		$section = $SECTION;
	}

	switch ($section) {
        case SECTION_GENERAL		: return 'general'; // grey
        case SECTION_XDIALOG		: return 'xdialog'; // purple
	    case SECTION_XDIALOGPROPS	: return 'xdialogprop'; // light purple
        
		case SECTION_STYLES			: return 'styles'; // brown
		case SECTION_XDID			: return 'xdid'; // blue
		case SECTION_XDIDPROPS		: return 'xdidprop'; // light blue
		case SECTION_EVENTS			: return 'event'; // green

		case SECTION_XPOPUP			: return 'xpopup'; // purple
		case SECTION_XPOPUPPROPS	: return 'xpopupprop'; // light purple
        case SECTION_XPOP			: return 'xpop'; // blue
		case SECTION_XPOPPROPS		: return 'xpopprops'; // light blue

		case SECTION_XDOCK			: return 'xdock'; // blue
		case SECTION_XDOCKPROPS		: return 'xdockprops'; // light blue
		
		case SECTION_INTRO:
		default:
			error_log("get_section_name(): unknown section $section");
			return '';
	}
}

function dcxdoc_print_description($caption, $content = "") {
	global $SECTION;

	$color = get_section_color();

    wikiData($caption);
    wikiData($content);

	if ($caption != "Introduction")
	    echo "<br />";
?>
<table class="description">
	<tr><td class="heading" style="border-color: <?php echo $color; ?>"><a name="<?php echo $SECTION; ?>"></a><?php echo "<div style='color: $color;'>$caption</div>"; ?></td></tr>
<?php
	if ($content) {
?>
	<tr><td><?php echo $content; ?></td></tr>
<?php
	}
?>
</table>
<?php
}


function dcxdoc_print_intro($page) {
	global $SECTION;

	$SECTION = SECTION_INTRO;
	$fninfo = "get_intro_$page";

	if (!function_exists($fninfo))
	    return;

	ob_start();
	$fninfo();

	// image at the end of intro
	if (!in_array($page, array('index', 'cla', 'archive'))) {
?><br /><br />
<div style="text-align: center;">
	<img src="images/<?php echo $page; ?>.png" alt="" />
</div>
<?php
	}
	
	$str = ob_get_clean();

	dcxdoc_print_description("<a name=\"" . SECTION_INTRO . "\"></a>Introduction", $str);
}


function dcxdoc_format_styles($data) {
	global $SECTION;
	$count = 1;
	
?><table class="styles">
<?php
	foreach ($data as $style => $info) {
		// allow for notes in styles section
		if ($style == '__notes')
			continue;
?>
	<tr>
		<td class="style"><a name="<?php echo "$SECTION.$count"; ?>" class="style"><?php echo $style; ?></a></td>
		<td><?php echo $info; ?></td>
	</tr>
<?php
		$count++;
	}

	if (isset($data['__notes']) && $data['__notes']) {
		format_notes($data['__notes']);
	}
?></table>
<br />
<?php
}

// get the latest changes only
function format_changes_latest() {
	global $CHANGES;
	
	if (!$CHANGES)
		return;

	// just return the first one
	foreach ($CHANGES as $version => $changes)
		return print_changes($version, $changes);
}

// get all changes
function format_changes() {
	global $CHANGES;

	if (!$CHANGES)
		return;

	ob_start();

	foreach ($CHANGES as $version => $changes) {
	    echo print_changes($version, $changes);
	}

	return ob_get_clean();
}

function print_changes($version, $changes) {
	ob_start();
	
	echo "<a name=\"$version\"></a><b>$version</b>\n<ul>";

	foreach ($changes as $key => $change) {
		// organised/detailed list
		if (is_array($change)) {
			echo "<li><strong>$key</strong><ul>";
			
			foreach ($change as $item)
			    echo "<li>" . htmlentities($item) . "</li>";
			    
			echo "</ul></li>";
		}
		// normal big list
		else
		    echo "<li>" . htmlentities($change) . "</li>";
	}

	echo "</ul>\n";
	return ob_get_clean();
}
?>
