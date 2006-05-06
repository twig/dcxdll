<?php
$CLA = array(
	'__desc' => "This command lets you add Cell Layout Algorithm rules to your dialog controls for automatic resizing of the child controls.",
	'__cmd' => "[COMMAND] [PATH] [TAB] [+FLAGS] [ID] [WEIGHT] [W] [H]",
	'__eg' => //array(
// EXAMPLE
		"root \$chr(9) +pv 0 1 0 0",
//		"root 1 2 \$chr(9) +fi 7 1 200 300",
//		"space 1 \$chr(9) + 5 5 5 5",
//	),
	'__param' => array(
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
		'ID' => "ID of the control (used with fixed or fill cells - use 0 if no control is to be linked to the cell).",
		'WEIGHT' => "Cell child weight. (used when adding a child cell to a pane cell)",
		'W' => "Fixed width of control (used with [v]fixed[/v] cell).",
		'H' => "Fixed height of control. (used with [v]fixed[/v] cell)",
	),
	'__notes' => array(
	    "See the Cell Layout Algorithm explanation for concrete examples.",
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
				//$saved_value = $value;

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

function wikiData(&$value, $userdata = "") {
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
}




function dcxdoc_header($page, $pagelabel) {
	global $VERSION;

	echo "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?".">";
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

<title>DCX <?php if ($page != 'index') echo "$pagelabel "; ?>- Dialog Control Xtension DLL v<?php echo $VERSION; ?> by ClickHeRe</title>

<link href="dcx.css" rel="stylesheet" type="text/css" />
</head>

<body>

<table>
	<tr>
		<td>
			<a href="index.htm"><img src="images/dcx.png" alt="DCX - Dialog Control Xtension" /></a>
		</td>
		<td style="vertical-align: bottom; text-align: right;">
			<a href="nl/index.htm">Dutch</a> |
			<a href="index.htm">English</a> |
			<a href="fi/index.htm">Finnish</a> |
			<a href="fr/index.htm">French</a> |
			<a href="ge/index.htm">German</a> |
			<a href="he/index.htm">Hebrew</a> |
			<a href="it/index.htm">Italian</a> |
			<a href="my/index.htm">Malaysian</a> |
			<a href="br/index.htm">Portuguese (Brazil)</a> |
			<a href="pt/index.htm">Portuguese</a> |
			<a href="sp/index.htm">Spanish</a> |
			<a href="sw/index.htm">Swedish</a>
		</td>
	</tr>
</table>

<table class="bar">
	<tr>
		<td>
			<a href='index.htm'>Home</a> |
			<a href="http://scriptsdb.org/download.php?id=1155">Download</a> |
			<a href="http://dcx.scriptsdb.org/forum/">Forum</a> |
			<a href="http://dcx.scriptsdb.org/bug/">Bug Tracker</a>
		</td>
		<td style="text-align: center;">Dialog Control Xtension<?php
	if (!in_array($page, array('index')))
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

	<a class="section" href="#">Introduction</a><br />
	<br />
	<a class="section" href="index.htm">DCX Controls</a><br />
	<ul>
<?php
	$pages = $PAGES;
	asort($pages);

	foreach ($pages as $page => $pagelabel) {
		if (in_array($page, array('index', 'changes')))
			continue;

	    echo "<li><a href=\"$page.htm\">$pagelabel</a></li>";
	}
?>
	</ul>
	<a class="section">DCX Features</a><br />
	<ul>
		<li><a href="layout.htm">Cell Layout Algorithm</a></li>
		<li><a href="xpopup.htm">XPopup Menus</a></li>
		<li><a href="changes.htm">Version History</a></li>
		<li><a href="dcxvsmdx.htm">DCX vs MDX</a></li>
    </ul>
</td>
<?php
}


function dcxdoc_menu_right($page) {
	global $XDID, $XDIALOG, $XDIDPROPS, $XDIALOGPROPS, $EVENTS, $GENERAL, $STYLES, $SECTION;

?><td class="menuright">
<?php
    print_menu_items($GENERAL, SECTION_GENERAL, "General Commands");
    print_menu_items($STYLES, SECTION_STYLES, "Control Styles");
    print_menu_items($XDIALOG, SECTION_XDIALOG, "/xdialog Flags");
    print_menu_items($XDIALOGPROPS, SECTION_XDIALOGPROPS, "\$xdialog() Properties");
    print_menu_items($XDID, SECTION_XDID, "/xdid Flags");
    print_menu_items($XDIDPROPS, SECTION_XDIDPROPS, "\$xdid() Properties");
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
</div>
</body>
</html>
<?php
}


// call generic function
function dcxdoc_format_general($cmd, &$data, $id) {
    format_xcmd("general", $cmd, $data, $id);
}
function dcxdoc_format_xdialog($flag, &$data, $id) {
    format_xcmd("xdialog", $flag, $data, $id);
}
function dcxdoc_format_xdid($flag, &$data, $id) {
    format_xcmd("xdid", $flag, $data, $id);
}
function dcxdoc_format_xdidprop($prop, $data, $id) {
    format_xcmd("xdidprop", $prop, $data, $id);
}
function dcxdoc_format_xdialogprop($prop, $data, $id) {
    format_xcmd("xdialogprop", $prop, $data, $id);
}
function dcxdoc_format_event($event, $data, $count) {
    format_xcmd("event", $event, $data, $count);
}

function format_xcmd($type, $flag, $data, $id) {
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

	// error_log("ERROR: __eg not set for $flag");
	if (!isset($data['__eg']))
		$data['__eg'] = '';

    // error_log("ERROR: __return not set for $flag");
	if (!isset($data['__return']))
		$data['__return'] = '';

	global $SECTION;

	// generate syntax
	$color = get_section_color();
	$syntax = "SYNTAX GENERATION ERROR";
	$example = "EXAMPLE GENERATION ERROR";

	switch ($type) {
		case "general":
		    if (isset($data['__isid']) && $data['__isid']) {
                $heading = "\$dcx($flag)";
				$syntax = "\$dcx($flag" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ")";
				$example = "\$dcx($flag" . ($data['__eg'] ? ", {$data['__eg']}" : '') . ")";
			}
			else {
                $heading = "/dcx $flag";
				$syntax = "/dcx $flag " . ($data['__cmd'] ? $data['__cmd'] : '');
				$example = "/dcx $flag " . ($data['__eg'] ? $data['__eg'] : '');
			}
		    break;
		case "xdid":
			$heading = "/$type -$flag";
			$syntax = "/$type -$flag [DNAME] [ID] {$data['__cmd']}";
			$example = "/$type -$flag dcx 4 {$data['__eg']}";
			break;
		case "xdialog":
			$heading = "/$type -$flag";
			$syntax = "/$type -$flag [DNAME] {$data['__cmd']}";
			$example = "/$type -$flag dcx {$data['__eg']}";
			break;
		case "xdidprop":
			$heading = "\$xdid().$flag";
			$syntax = "\$xdid(dialog, ID" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ").$flag";
			$example = "\$xdid(dcx, 4" . ($data['__cmd'] ? ", {$data['__eg']}" : '') . ").$flag";
			break;
		case "xdialogprop":
			$heading = "\$xdialog().$flag";
			$syntax = "\$xdialog(dialog, ID" . ($data['__cmd'] ? ", {$data['__cmd']}" : '') . ").$flag";
			$example = "\$xdialog(dcx" . ($data['__cmd'] ? ", {$data['__eg']}" : '') . ").$flag";
		    break;
        case "event":
        	$heading = "$flag";
			$syntax = "/cb_alias DNAME $flag ID {$data['__cmd']}";
			$example = "/cb_alias dcx $flag 4 {$data['__eg']} ";
            break;
		default:
		    break;
	}
?>
<table class="<?php echo $type; ?>">
	<tr><td colspan="2" class="flag" style="border-color: <?php echo $color; ?>;"><a name="<?php echo "$SECTION.$id"; ?>"><?php echo "<div style='color: $color;'>$heading</div>"; ?></a></td></tr>
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
	echo "<tr><td>&nbsp;</td></tr>";

	if (!is_array($data))
	    $data = array($data);

	foreach ($data as $note) {
		echo "<tr><td colspan=\"2\"><a class=\"note\">Note.</a> $note</td></tr>";
	}
}


function format_return(&$data) {
	echo "<tr><td>&nbsp;</td></tr>";

	if (!is_array($data))
	    $data = array($data);

	foreach ($data as $ret) {
		echo "<tr><td class=\"syntax\">Return</td><td>$ret</td></tr>";
	}
}


function get_section_color($col = 0) {
	if (!$col) {
		global $SECTION;
		$col = $SECTION;
	}

	switch ($col) {
        case SECTION_GENERAL:
			$color = 'brown';
		    break;
		case SECTION_STYLES:
			$color = '#bb6600';
		    break;
        case SECTION_XDIALOG:
			$color = 'orange';
		    break;
	    case SECTION_XDIALOGPROPS:
			$color = 'red';
		    break;
		case SECTION_XDID:
			$color = 'blue';
		    break;
		case SECTION_XDIDPROPS:
			$color = 'purple';
		    break;
		case SECTION_EVENTS:
		    $color = 'green';
			break;
		case SECTION_INTRO:
		default:
			$color = 'black';
	}

	return $color;
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
	if (!in_array($page, array('index'))) {
?><br /><br />
<div style="text-align: center;">
	<img src="images/<?php echo $page; ?>.png" alt="" />
</div>
<?php
	}
	
	$str = ob_get_clean();

	dcxdoc_print_description("Introduction", $str);
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
	
	echo "<b>$version</b>\n<ul>";

	foreach ($changes as $change)
	    echo "<li>$change</li>";

	echo "</ul>";
	return ob_get_clean();
}
?>
