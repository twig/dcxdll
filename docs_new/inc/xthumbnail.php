<?php

function xthumbnail_load($page) {
	global $XTHUMBNAIL;

	loadSection($XTHUMBNAIL, "get_xthumbnail");
}

function xthumbnail_unload() {
	global $XTHUMBNAIL;
	
	$XTHUMBNAIL = array();
}

function xthumbnail_layout($page, $pagelabel) {
	global $SECTION, $XTHUMBNAIL;
	
	dcxdoc_print_intro($page);
	
	// /xthumbnail commands
	if ($XTHUMBNAIL) {
        $SECTION = SECTION_XTHUMBNAIL;
        $count = 1;

        dcxdoc_print_description('/xthumbnail Command', "The /xthumbnail command is used to manipulate buttons in the taskbar preview window.");

        foreach ($XTHUMBNAIL as $cmd => $data) {
       		dcxdoc_format_xthumbnail($cmd, $data, $count);
      		$count++;
		}
	}
	
	// xthumbnail signals
	$SECTION = SECTION_EVENTS;
	dcxdoc_print_description('xThumbnail Signals', xthumbnail_events());
}

function get_intro_xthumbnail() {
	echo "XThumbnail allows you to create and modify buttons in the taskbar preview window. These can be used to execute scripted commands without activating the mIRC window.";
}

function get_xthumbnail(&$XTHUMBNAIL) {
	$XTHUMBNAIL = array(
		'w' => array(
			'__desc' => 'This command adds an icon to the internal list.',
			'__cmd' => '[+FLAGS] [INDEX] [FILENAME]',
			'__eg' => array(
				'+ 4 mirc.exe',
				'+g 2 mirc.exe',
				'+P 0 image.png',
			),
			'__params' => array(
			    // +FLAGS
				'INDEX' => 'Icon index in icon archive',
				'FILENAME' => 'Icon archive filename',
			),
			'__notes' => array(
				"Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
				"Use [v]-1[/v] for [p]INDEX[/p] to load all icons in a file.",
				"[p]INDEX[/p] supports ranges eg: 1-7,12",
			),
		),
		'r' => array(
			'__desc' => 'This command removes a button.',
			'__params' => array(
			    'ID' => 'The unique ID that this trayicon will be recognised by.',
			),
			'__notes' => "Use [v]0[/v] for [p]INDEX[/p] if the file is a single icon file.",
		),
		'e' => array(
			'__desc' => 'This command applies all changes made and enables or disables buttons.',
			'__cmd' => '[1|0]',
			'__eg' => array(
				'1',
			),
			'__params' => array(
			    'STATE' => '1 == Enable or 0 == disable.',
			),
		),
	    'W' => array(
			'__desc' => 'This command adds a bitmap file.',
			'__cmd' => '[FILENAME]',
			'__eg' => 'image.bmp',
			'__params' => array(
				'FILENAME' => 'Image filename',
			),
		),
		'b' => array(
			'__desc' => 'This command adds a button.',
			'__cmd' => '[ID] [IMAGE] [TOOLTIP]',
			'__eg' => '1 4 This is a new button',
			'__params' => array(
			    'ID' => 'The unique ID that this button will be recognised by. (1-7)',
				'IMAGE' => 'The image index to use.',
				'TOOLTIP' => 'The tooltip for the trayicon.',
			),
		),
		'y' => 'This command clears the image list.',
	);
	
	writeDcxLoadIcon($XTHUMBNAIL, 'w', '+FLAGS', 2);
}

function xthumbnail_events() {
	global $SECTION;
	
	$color = get_section_color($SECTION);

	$data = array(
		'__params' => array(
		    'thumb' => 'The event signal identifier.',
			'ID' => 'The unique ID which the button was created with. (1-7)',
		),
	);
	
	ob_start();
?>
<table class="<?php echo get_section_name($SECTION); ?>">
	<tr><td colspan="2">XThumbnail signals are always enabled and sent to mIRC when a button is pressed.</td></tr>
	<tr>
		<td class="syntax">Signal:</td>
		<td><div class="syntax">DCX thumb mIRC ID</div></td>
	</tr>
	<tr>
		<td class="example">Example:</td>
		<td><div class="example">DCX thumb mIRC 1</div></td>
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
