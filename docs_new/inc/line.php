<?php
function get_intro_line() {
	echo "The line control is a graphical line, much like the side of a box control.";
}

function get_styles_line(&$STYLES) {
	$STYLES = array(
		"vertical" => "The line is vertical.",
	);
}


function get_events_line(&$EVENTS) {
	$EVENTS = array(
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
