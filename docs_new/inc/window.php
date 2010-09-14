<?php
function get_intro_window() {
?>This control enables you to dock a @window as a control on a dialog. The @window must be opened before any attempt to add it to the dialog.<br />
<br />
You can dock any number of windows as long as each is a separate control.<br />
<br />
Please take note that the windows are <b>ejected</b> when the dialog is closed.
<?php
}


function get_styles_window(&$STYLES) {
	$STYLES = array(
		"@windowName" => 'There are no specific styles for this control. The style is used to determine which window is embedded. You may also specify a HWND to embed instead of a @windowName',
	);
}

function get_xdidprops_window(&$PROPS) {
	$PROPS = array(
		'wname' => 'Returns the name of the embedded window.',
	);
}

function get_events_window(&$EVENTS) {
	$EVENTS = array(
		"help" => "Launched when you click on a control using the [s]?[/s] contexthelp button.",
	);
}
?>
