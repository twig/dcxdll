<?php

function index_layout($page, $pagelabel) {
	global $SECTION, $GENERAL;

	$SECTION = SECTION_INTRO;	
	dcxdoc_print_intro($page);
    
        $SECTION = SECTION_GENERAL;
        gdiplus();
	callback_description();
	latest_changes();
	credits();
}

function get_intro_index() {
?>This DLL lets you add new controls to mIRC dialogs as well as change other features like the visual style, position etc...<br />

<div class="divpink">DCX does not support mIRC controls (NOT EVEN mIRC TABS), nor anything related to mIRC normal dialogs except it's menus. DCX does not work like MDX so please read the manual and see dcx_demo.mrc and dcx_tools.mrc for examples.</div>

<div class="divpink" style="color: #0000ff; padding-top: 0px;">DCX v3.1+ only works on mIRC v7+, Try & use the latest mIRC if possible (v7.48 at the time of writing)</div>

The DLL creates the controls directly on the mIRC dialog and you use clone functions defined by /xdid and $xdid which behave like the regular /did
and $did to interact with the controls.<br />
<br />
The event handling in the DLL is supported by a callback alias supplied by the user when the dialog is marked before adding DCX controls. The callback alias is called from within the DLL to ask for info (if you want to modify the default behavior in the DLL thus the "callback" type) and will supply the user with the events and parameters of these events.<br />
<br />
Every function in this DLL returns a value if used with the $dll identifier. [v]D_OK [function]: [message][/v] if successful, [v]D_ERROR [function]: [message][/v] otherwise.
<?php

$cb_notes = array(
    "Mandatory parameters are expressed in [p][SQUARE][/p] brackets and optional parameters in [p](ROUNDED)[/p] brackets.",
    "We strongly suggest you load the <b>dcx_tools.mrc</b> file included in the zip as it contains the necessary <b>/xdialog</b>, <b>/xdid</b>, <b>/xdock</b>, <b>/xtray</b>, <b>/xstatusbar</b>, <b>/xtreebar</b> and <b>/xmenubar</b> aliases and identifiers needed to use the DLL.",
    "Function calls, styles, and references to anything DCX related are all <strong>case sensitive</strong>!"
);

format_notes($cb_notes);

//'
}

function gdiplus() {
    ob_start();
?>
For advanced support for images (such as extra file formats and drawing routines), Microsoft has released a library called GDI+. This is included in Windows XP, however older versions of Windows may not have this.<br />
<br />
Use [link page="dcx" section="general" flag="IsUsingGDI"]$dcx(IsUsingGDI)[/link] to check whether or not you have GDI+ support on your computer. If you do not, you can download it <a href="http://dcx.scriptsdb.org/gdiplus.zip">here</a>.<br />
<?php
    $str = ob_get_clean();
    
    dcxdoc_print_description("GDI+", $str);
}


function latest_changes() {
	ob_start();
?>
If you notice any inconsistancies or errors with the documentation, please put a post on the forum on this <a href="http://dcx.scriptsdb.org/forum/showthread.php?tid=394">thread</a>!<br />
<br />
<?php
	$str = ob_get_clean() . format_changes_latest();
	
    dcxdoc_print_description("Latest Changes", $str);
}

function callback_description() {
	ob_start();
?><b>Overview</b><br />
Every DCX Control fires events upon user action on the control. These events are sent to an <a>alias</a> specified when the dialog is <a>Marked</a>.<br />
<br />
<b>Why are the events not sent like regular on *:DIALOG: events?</b><br />
Well for one thing, mIRC events are linear and static in the way that you cannot return any value to the event manager, to modify the default behavior of the event. Using callback support with an <a>alias</a> enables this type of interaction.<br />
<br />
<b>Callback Event Data</b><br />
<b>Format:</b> <a class="syntax">[NAME] [EVENT] [ID] [PARAM]</a>
<table>
<?php
	$cb_param = array(
		'NAME' => '$1 = Name of the dialog that the event came from.',
		'EVENT' => '$2 = Name of the event (eg. sclick, dclick, ...)',
		'ID' => '$3 = ID of the control that fired the event.',
		'PARAM' => '$4- = Additional information, event specific (usually $null, except specified otherwise in documentation)',
	);
	$cb_notes = array(
	    "Some control events support return values to change their default behaviour. When returning special values during certain events, such as [r]nosize[/r] for [e]sizing[/e], interact with the DLL and modify the default event behavior. The events that support a return value will have a description of what the return value does when returned."
	);
	
	array_walk_recursive($cb_param, "wikiData");
	array_walk_recursive($cb_notes, "wikiData");
	
	format_parameters($cb_param);
	format_notes($cb_notes);
?>
</table>
<?php
	//'
	$str = ob_get_clean();
	dcxdoc_print_description("Callback Alias Events", $str);
}


function credits() {
	ob_start();
?>
<table style="table-layout: fixed;">
	<tr>
		<td style="width: 150px;"><a>Lead Developer</a></td>
		<td>ClickHeRe</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Supporting Developers<br /></a>(in chronological order)</td>
		<td>
			hixxy<br />
			twig*<br />
			Mpdreamz<br />
			HAPM<br />
			Ook
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Documentation</a></td>
		<td>
			twig*<br />
			Andy<br />
            Mpdreamz
		</td>
	</tr>
    <tr><td colspan="2">&nbsp;</td></tr>
    <tr>
        <td><a>Additional libraries</a></td>
        <td>&nbsp;</td>
    </tr>
    <tr>
        <td>CWndShadowx</td>
        <td>Copyright (c) 2006 Perry Zhu</td>
    </tr>
    <tr>
        <td>Treebar.dll</td>
        <td>NaquadaServ</td>
    </tr>
    <tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>MDX vs DCX</a></td>
		<td>Mpdreamz</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Demo Script</a></td>
		<td>elizio</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td>
			<a>General Testing</a><br />
			(and forum helpers!)
		</td>
		<td>
		    dJabba<br />
		    PYERO<br />
		    dunkelzahn<br />
		    TheShadowOne<br />
		    zipster<br />
			bl0x<br />
			darkrift<br />
			The_Mega_ZZTer<br />
			Duplex<br />
			evasion<br />
                        hkr<br />
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Tutorials</a></td>
		<td>
			<a href="http://perso.wanadoo.fr/jiveda/dcx/index.html">j0x's DCX Tutorials (French)</a><br />
			<a href="http://dcx.scriptsdb.org/forum/showthread.php?tid=120">DCX Forum: Help: CLA</a><br />
			<a href="http://dcx.scriptsdb.org/forum/showthread.php?tid=127">DCX Forum: CLA :(</a><br />
			<a href="http://www.mircscripts.org/comments.php?cid=2977">Mpdreamz's Cell Layout Algorithm Tutorial</a><br />
			<a href="http://dcx.scriptsdb.org/forum/showthread.php?tid=320">EinsteinXXL's CLA Tutorial (German)</a>
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Translations</a></td>
		<td>
			<table style="width: auto;">
				<tr>
					<td>Dutch</td>
					<td>Mpdreamz</td>
				</tr>
				<tr>
					<td>Finnish</td>
					<td>Mkz</td>
				</tr>
				<tr>
					<td>French</td>
					<td>dunkelzahn</td>
				</tr>
				<tr>
					<td>German</td>
					<td>Duplex</td>
				</tr>
				<tr>
					<td>Hebrew</td>
					<td>seec77</td>
				</tr>
				<tr>
					<td>Italian</td>
					<td><a href="mailto:clickhere@scriptsdb.org">Still need someone!</a></td>
				</tr>
				<tr>
					<td>Malaysian</td>
					<td>da^hype</td>
				</tr>
				<tr>
					<td>Portuguese (Brazil)</td>
					<td>Henrique</td>
				</tr>
				<tr>
					<td>Portuguese (Portugal)</td>
					<td>Theorem</td>
				</tr>
				<tr>
					<td>Spanish</td>
					<td>PaTaGoN</td>
				</tr>
				<tr>
					<td>Swedish</td>
					<td>Rixxon</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Website</a></td>
		<td>
			<a href="http://dcx.scriptsdb.org">http://dcx.scriptsdb.org</a><br />
			<a href="http://dcx.scriptsdb.org/forum">Forums</a>
		</td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
		<td><a>Support</a></td>
		<td>
			<a href="irc://irc.genscripts.net/genscripts">#genscripts @ Genscripts</a>
		</td>
	</tr>

</table>
<?php
	$str = ob_get_clean();
	dcxdoc_print_description("Credits", $str);
}



?>
