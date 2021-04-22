<?php

function xtray_load($page) {
	global $XTRAY;

	loadSection($XTRAY, "get_xtray");
}

function xtray_unload() {
	global $XTRAY;
	
	$XTRAY = array();
}

function xtray_layout($page, $pagelabel) {
	global $SECTION, $XTRAY;
	
	dcxdoc_print_intro($page);
	
	// /xtray commands
	if ($XTRAY) {
        $SECTION = SECTION_XTRAY;
        $count = 1;

        dcxdoc_print_description('/xtray Command', "The /xtray command is used to manipulate icons in the system tray.");

        foreach ($XTRAY as $cmd => $data) {
       		dcxdoc_format_xtray($cmd, $data, $count);
      		$count++;
		}
	}
	
	// xtray signals
	$SECTION = SECTION_EVENTS;
	dcxdoc_print_description('XTray Signals', xtray_events());
}

function get_intro_xtray() {
	echo "XTray allows you to create and modify icons in the system tray. They provide a useful indication of your status, tooltips, and can also receive basic mouse events.";
}

function get_xtray(&$XTRAY) {
	$XTRAY = array(
		'c' => array(
			'__desc' => 'This command creates a trayicon.',
			'__cmd' => '[+FLAGS] [INDEX] [FILENAME] (TAB) (TOOLTIP)',
			'__eg' => array(
				'+ 4 mirc.exe $chr(9) This is the trayicon tooltip',
				'+g 2 mirc.exe',
			),
			'__params' => array(
			    'ID' => 'The unique ID that this trayicon will be recognised by.',
			    // +FLAGS
				'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
				'TOOLTIP' => 'The tooltip for the trayicon.',
			),
			'__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
		'd' => array(
			'__desc' => 'This command deletes the trayicon.',
			'__params' => array(
			    'ID' => 'The unique ID that this trayicon will be recognised by.',
			),
			'__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
		'e' => array(
			'__desc' => 'This command edits a trayicon.',
			'__cmd' => '[+FLAGS] [INDEX] [FILENAME] ([TAB] TOOLTIP)',
			'__eg' => array(
				'+ 4 mirc.exe $chr(9) This is the new trayicon tooltip',
				'+g 2 mirc.exe',
			),
			'__params' => array(
			    'ID' => 'The unique ID that this trayicon will be recognised by.',
			    // +FLAGS
				'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
				'TOOLTIP' => 'The tooltip for the trayicon.',
			),
			'__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
	    'i' => array(
			'__desc' => 'This command edits the trayicon icon.',
			'__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
			'__eg' => '+ 4 mirc.exe',
			'__params' => array(
			    'ID' => 'The unique ID that this trayicon will be recognised by.',
			    // +FLAGS
				'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
			'__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
		'T' => array(
			'__desc' => 'This command edits the trayicon tooltip.',
			'__cmd' => '[TOOLTIP]',
			'__eg' => 'This is the new trayicon tooltip',
			'__params' => array(
			    'ID' => 'The unique ID that this trayicon will be recognised by.',
				'TOOLTIP' => 'The tooltip for the trayicon.',
			),
		),
	);
	
	writeDcxLoadIcon($XTRAY, 'c', '+FLAGS', 2);
	writeDcxLoadIcon($XTRAY, 'e', '+FLAGS', 2);
	writeDcxLoadIcon($XTRAY, 'i', '+FLAGS', 2);
}

function xtray_events() {
	global $SECTION;
	
	$color = get_section_color($SECTION);

	$data = array(
		'__params' => array(
		    'trayicon' => 'The event signal identifier.',
			'EVENT' => array(
				'__desc' => 'Information about event occurred.',
				'__values' => array(
					'dclick' => 'The trayicon was double clicked.',
					'rdclick' => 'The trayicon was double right-clicked.',
					'mdclick' => 'The trayicon was double middle-clicked.',
					'sclick' => 'The trayicon was single left-clicked.',
					'rclick' => 'The trayicon was single right-clicked.',
					'mclick' => 'The trayicon was single middle-clicked.',
				),
			),
			'ID' => 'The unique ID which the trayicon was created with.',
		),
	);
	
	ob_start();
?>
<table class="<?php echo get_section_name($SECTION); ?>">
	<tr><td colspan="2">XTray signals are enabled by default and sent to mIRC containing information about mouse activity over the trayicon. To enable or disable signal messages for trayicons, refer to [f]<a href="dcx.htm#general.xSignal">/dcx xSignal</a>[/f]</td></tr>
	<tr>
		<td class="syntax">Signal:</td>
		<td><div class="syntax">DCX trayicon EVENT ID</div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example">DCX trayicon dclick 1</div></td>
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
