<?php

function xstatusbar_load($page) {
    global $XSTATUSBAR, $XSTATUSBARPROPS;

    loadSection($XSTATUSBAR, "get_xstatusbar");
    loadSection($XSTATUSBARPROPS, "get_xstatusbarprops");
}

function xstatusbar_unload() {
    global $XSTATUSBAR, $XSTATUSBARPROPS;
    
    $XSTATUSBAR = array();
    $XSTATUSBARPROPS = array();
}

function get_intro_xstatusbar() {
	echo 'The /xstatusbar command is used to create and control a statusbar docked directly within the main mIRC window, without the need of creating a dialog to host it.';
}

// TODO: fix this up after
function xstatusbar_layout($page, $pagelabel) {
    global $SECTION, $XSTATUSBAR, $XSTATUSBARPROPS;
    
    dcxdoc_print_intro($page);
    
    // /xstatusbar commands
    if ($XSTATUSBAR) {
        $SECTION = SECTION_XSTATUSBAR;
        $count = 1;

        dcxdoc_print_description('/xstatusbar Commands', 'The /xstatusbar command is used to control the docked StatusBar.');

        foreach ($XSTATUSBAR as $cmd => $data) {
       	    dcxdoc_format_xstatusbar($cmd, $data, $count);
      	    $count++;
	}
    }
	
    // $xstatusbar commands
    if ($XSTATUSBARPROPS) {
        $SECTION = SECTION_XSTATUSBARPROPS;
        $count = 1;

        dcxdoc_print_description('$xstatusbar Properties', 'The $xstatusbar identifier is used to retrieve the information from the docked statusbar.');

        foreach ($XSTATUSBARPROPS as $cmd => $data) {
            dcxdoc_format_xstatusbarprops($cmd, $data, $count);
      	    $count++;
	}
    }
    
    // xstatusbar signals
    $SECTION = SECTION_EVENTS;
    dcxdoc_print_description('XStatusbar Signals', xstatusbar_events());
}

function get_xstatusbar(&$XSTATUSBAR) {
    $XSTATUSBAR = array(
        'A' => array(
            '__desc' => 'This command enables/disables a statusbar in the mIRC window and applies the styles specified.',
            '__cmd' => '[1|0] (STYLE)',
            '__eg' => '1 notheme',
            '__params' => array(
                'STYLE' => 'Styles supported by the <a href="statusbar.htm">StatusBar</a> control.',
            ),
            '__notes' => array(
                'Syntax for controlling the mIRC StatusBar using /xstatusbar is identical to the syntax for the DCX <a href="statusbar.htm">StatusBar</a> control.<br />',
                'eg. /xstatusbar -l 200 200 100 -1',
            ),
        ),
        "f" => array(
            '__desc' => "This command lets you change the XStatusBar font.",
            '__cmd' => "[+FLAGS] [CHARSET] [SIZE] [FONTNAME]",
            '__eg' => "+abs ansi 10 Tahoma",
            '__params' => array(
                '+FLAGS' => array(
                    '__desc' => "Font style flags.",
                    '__values' => array(
                        'a' => "Font is anti-aliased.",
                        'b' => "Font is bold.",
                        'd' => "Default font.",
                        'i' => "Font is italic.",
                        's' => "Font is strikedout.",
                        'u' => "Font is underlined.",
                    ),
                ),
                'CHARSET' => "Can be any of the following: [v]ansi[/v], [v]baltic[/v], [v]chinesebig[/v], [v]default[/v], [v]easteurope[/v], [v]gb2312[/v], [v]greek[/v], [v]hangul[/v], [v]mac[/v], [v]oem[/v], [v]russian[/v], [v]shiftjis[/v], [v]symbol[/v], [v]turkish[/v], or [v]vietnamese[/v].",
                'SIZE' => "Font size in points.",
                'FONTNAME' => "Font name. Ex: Tahoma, Courier New, Arial, ...",
            ),
            '__notes' => array(
                "Font names are limited to 31 chars (system definition).",
                'When using [f]+d[/f], [p]CHARSET[/p] [p]SIZE[/p] and [p]FONTNAME[/p] can be omitted.'
            ),
        ),
        'k' => array(
            '__desc' => 'This command lets you set the status bar background color.',
            '__cmd' => '[RGB]',
            '__eg' => '$rgb(255,0,0)',
            '__notes' => array(
                "You can use [v]-1[/v] for [p]RGB[/p] to revert to default color.",
                'This command will only work if the computer has no visual styles applied (XP themes). To use this on machines with visual styles enabled, apply the [s]notheme[/s] style.',
            ),
        ),
        'l' => array(
            '__desc' => 'This command lets you set the width of status bar cells. A value of [v]-1[/v] will cause the cell to automatically stretch, filling the remainding space. Values can also be a percentage, such as [v]20%[/v].',
            '__cmd' => '[POS] (POS POS ...)',
            '__eg' => '10% 200 400 -1'
        ),
        't' => array(
            '__desc' => 'This command lets you set the icon, text and tooltip of a statusbar cell.',
            '__cmd' => '[N] [+FLAGS] [#ICON] [BKGCOLOR] [TXTCOLOR] (Cell Text) $chr(9) (Tooltip)',
            '__eg' => '2 +p 0 -1 -1 Cell Text $chr(9) Tooltip Text',
            '__params' => array(
                'N' => 'Status bar cell number to modify.',
                '+FLAGS' => array(
                    '__desc' => 'Status bar cell flags.',
                    '__values' => array(
                        'p' => 'Popup style border around cell.',
                        'n' => 'No borders around cell.',
						'f' => 'Enables Owner Draw, allowing control codes in the text & custom colours'
                    ),
                ),
                '#ICON' => 'Icon number for the statusbar cell (0 for no icon)',
                'BKGCOLOR' => 'The background colour (requires [f]+f[/f])',
                'TXTCOLOR' => 'The text colour (requires [f]+f[/f])',
            ),
            '__notes' => 'Tooltip is only displayed if cell text is absent or cell text is too long to fit in cell.',
        ),
		'v' => array(
		    '__desc' => 'This command lets you change the text of a statusbar cell.',
			'__cmd' => '[N] (TEXT)',
			'__eg' => "2 New Text",
		),
        'w' => array(
            '__desc' => 'This command lets you add an icon to the statusbar image list.',
            '__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
            '__eg' => '+ 113 shell32.dll',
            '__params' => array(
                    // +FLAGS
	            'INDEX' => 'Icon index in icon archive',
		    'FILENAME' => 'Icon archive filename',
		),
	    '__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
	),
	'y' => 'This command lets you clear the statusbar image list.',
    );
}

function get_xstatusbarprops(&$XDIDPROPS) {
    $XDIDPROPS = array(
	    'visible' => 'Returns whether or not the mIRC StatusBar is visible.',
        'parts' => 'Returns the number of parts in the mIRC StatusBar.',
	    'text' => array(
	        '__desc' => 'Returns the text on the mIRC StatusBar part.',
	        '__cmd' => 'N',
            '__eg' => '5',
        ),
        'tooltip' => array(
	        '__desc' => 'Returns the tooltip for the mIRC StatusBar part.',
	        '__cmd' => 'N',
            '__eg' => '2',
        ),
    );
}

function xstatusbar_events() {
	global $SECTION;
	
	$color = get_section_color($SECTION);

	$data = array(
		'__params' => array(
		    'xstatusbar' => 'The event signal identifier.',
            'DCXStatusbar' => 'This is to let the user know which event was triggered.',
			'EVENT' => array(
				'__desc' => 'Information about event occurred.',
				'__values' => array(
					'dclick'  => 'A part of the statusbar was double clicked.',
					'rdclick' => 'A part of the statusbar was double right-clicked.',
					'sclick'  => 'A part of the statusbar was single left-clicked.',
					'rclick'  => 'A part of the statusbar was single right-clicked.',
				),
			),
            'HWND' => 'The hwnd of the XStatusBar.',
            'PART' => 'The ID of the part which the event occurred on.',
		),
	);
	
	ob_start();
?>
<table class="<?php echo get_section_name($SECTION); ?>">
	<tr><td colspan="2">XStatusBar signals are enabled by default and sent to mIRC containing information about mouse activity over the XStatusBar. To enable or disable signal messages for XStatusBar, refer to [f]<a href="dcx.htm#general.xSignal">/dcx xSignal</a>[/f]</td></tr>
	<tr>
		<td class="syntax">Signal:</td>
		<td><div class="syntax">DCXStatusbar EVENT HWND PART</div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example">DCXStatusbar sclick 133826 3</div></td>
	</tr>
<?php

	// parameters
	 format_parameters($data['__params']);
?>
</table>
<?php
	return ob_get_clean();
}
?>