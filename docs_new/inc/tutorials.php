<?php

function tutorials_layout($page, $pagelabel) {
	global $SECTION;
	
	// intro
	dcxdoc_print_intro($page);
	
	$SECTION = SECTION_GENERAL;
	$EXAMPLES = array();
	get_examples($EXAMPLES);

	foreach ($EXAMPLES as $title => $d) {
		display_example($title, $d);
	}
}


function get_intro_tutorials() {
	echo "DCX offers a diverse number of additional controls to mIRC dialogs. It also offers many exciting new features to existing mIRC controls which allow you more flexibility and creativity within your dialog designs.<br />" .
			"<br />" .
			"We strive to keep DCX as easy to use as possible, but we also recommend you have a <strong>strong</strong> understanding of mIRC dialogs before venturing into the world of DCX. Once you feel you are ready, its time to say hello world :)";
}


function display_example($title, $data) {
	global $EGPATH;
	
	ob_start();
	
	// syntax
	echo "<table>";
	echo "<tr><td><strong>Command:</strong></td><td>/{$data['__cmd']}</td></tr>";
	
	// teachings
	echo "<tr><td><strong>Teaches:</strong></td><td>";
	$count = count($data['__teaches']);
	
	foreach ($data['__teaches'] as $k => $teaching) {
		echo "- $teaching" . ($k < $count ? '<br />' : '');
	}

	// display code
	echo "</td></tr>";
	echo "<tr><td colspan='2'><strong>Script by {$data['__author']}</strong></td></tr>";
	echo "<tr><td colspan='2'>[code]";
	
	$filename = $EGPATH . $data['__cmd'] . '.txt';
    $hfile = fopen($filename, "r");
    
    // couldnt open file for writing
    if (!$hfile) {
		error_log("Could not open example file for reading. Terminating batch.");
		exit();
	}
	// NB: This replace is needed to stop tags etc.. within the scripts being rendered as html.
	$needles = array(
		"&","<", ">"
	);
	$reps = array(
		"&amp;","&lt;","&gt;"
	);
	echo str_replace($needles,$reps,fread($hfile, filesize($filename)));
	//echo fread($hfile, filesize($filename));
	fclose($hfile);
	echo "[/code]</td></tr>";
	
	// end table
	echo "</table>";
	$str = ob_get_clean();
	
	dcxdoc_print_description($title, $str);
}



function get_examples(&$EXAMPLES) {
	$EXAMPLES = array(
		'Very Basic' => array(
			'__author' => 'twig*',
			'__cmd' => 'very_basic',
			'__teaches' => array(
				'Basic structure of DCX initialisation procedure',
				'How to recognise events for specific controls'
			),
		),
		'Still Basic' => array(
			'__author' => 'twig*',
			'__cmd' => 'box_styles',
			'__teaches' => array(
				'How to use DCX styles on controls',
				'Use of $xdid().properties',
				'Demonstrating use of /xdialog -b (border command)'
			),
		),
		'Somewhat useful' => array(
			'__author' => 'Duplex',
			'__cmd' => 'tb_ex',
			'__teaches' => array(
				'How to use the DCX toolbar',
				'Demonstrates that $dialog() commands still work on DCX\'d dialogs',
				'How to implement toolbar groups',
				'How to add icons to a toolbar',
				'Usage of control/event specific callback parameters',
			),
		),
		'Practical' => array(
			'__author' => 'sprion',
			'__cmd' => 'tags_eg',
			'__teaches' => array(
				'Use of listview with headers',
				'Use of listview $xdid().properties and /xdid commands',
			),
		),
		'Almost Complete' => array(
			'__author' => 'sprion',
			'__cmd' => 'proxy_eg',
			'__teaches' => array(
				'Use of child controls on the DCX Box control',
				'Use of radio boxes to switch between options',
				'Use of global functions to enable/disable controls'
			),
		),
		'xTreebar' => array(
			'__author' => 'Ook',
			'__cmd' => 'dcxtreebar',
			'__teaches' => array(
				'Use of xTreebar',
			),
		),
		'Menus' => array(
			'__author' => 'Ook',
			'__cmd' => 'dcxmenus',
			'__teaches' => array(
				'Use of DCX Menus',
			),
		),
		'xStatusbar' => array(
			'__author' => 'Ook',
			'__cmd' => 'dcxstatusbar',
			'__teaches' => array(
				'Use of xStatusbar',
			),
		),
	);
}
?>
