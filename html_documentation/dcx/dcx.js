function version() {
	document.write(v());
}

function v() {
	return "1.3.0";
}

function header() {
	var txt;

	txt  = '<table border="0" width="100%" cellspacing="0" cellpadding="0">';
	txt += '<tr><td align="left" valign="middle"><a href="index.htm" title="DCX - Dialog Control Xtension"><img src="../images/dcx.png"></a></td>';
	txt += '<td valign="bottom" align="right">';
	txt += '<a href="nl/index.htm">Dutch</a> |';
	txt += '<a href="index.htm">English</a> |';
	txt += '<a href="fi/index.htm">Finnish</a> |';
	txt += '<a href="fr/index.htm">French</a> |';
	txt += '<a href="ge/index.htm">German</a> |';
	txt += '<a href="he/index.htm">Hebrew</a> |';
	txt += '<a href="it/index.htm">Italian</a> |';
	txt += '<a href="my/index.htm">Malaysian</a> |';
	txt += '<a href="br/index.htm">Portuguese (Brazil)</a> |';
	txt += '<a href="pt/index.htm">Portuguese</a> |';
	txt += '<a href="sp/index.htm">Spanish</a> |';
	txt += '<a href="sw/index.htm">Swedish</a>';
	txt += '</td></tr></table>';

	txt += '<table class="bar"><tr>';
	txt += '<td align=left width=33%><a href="index.htm">Home</a> | <a href="http://scriptsdb.org/download.php?id=1155">Download</a> | <a href="http://dcx.scriptsdb.org/forum/">Forum</a> | <a href="http://dcx.scriptsdb.org/bug/">Bug Tracker</a></td>';
	txt += '<td align=center width=33%><b>Dialog Control Xtension</b></td>';
	txt += '<td align=right width=33%><b>Version ' + v() + '</b></td></tr></table>';

	document.write(txt);
}

function menu_left() {
	var txt;

	txt  = '<a class=i href="#1">Introduction</a><br /><br />';

	txt += '<a class=i href="dcxvsmdx.htm">DCX vs MDX</a><br /><br />';

	txt += '<a class=i href="#2">DCX Supported Controls</a><ul>';
	txt += '<li><a href="pbar.htm">Progressbar</a></li>';
	txt += '<li><a href="treeview.htm">TreeView</a></li>';
	txt += '<li><a href="listview.htm">ListView</a></li>';
	txt += '<li><a href="toolbar.htm">Toolbar</a></li>';
	txt += '<li><a href="statusbar.htm">Statusbar</a></li>';
	txt += '<li><a href="comboex.htm">ComboEx</a></li>';
	txt += '<li><a href="trackbar.htm">Trackbar</a></li>';
	txt += '<li><a href="rebar.htm">Rebar</a></li>';
	txt += '<li><a href="richedit.htm">RichEdit</a></li>';
	txt += '<li><a href="colorcombo.htm">ColorCombo</a></li>';
	txt += '<li><a href="button.htm">Button</a></li>';
	txt += '<li><a href="ipaddress.htm">IP Address</a></li>';
	txt += '<li><a href="updown.htm">UpDown</a></li>';
	txt += '<li><a href="webctrl.htm">WebControl</a></li>';
	txt += '<li><a href="calendar.htm">Calendar</a></li>';
	txt += '<br />';
	txt += '<li><a href="line.htm">Line</a></li>';
	txt += '<li><a href="box.htm">Box</a></li>';
	txt += '<li><a href="radio.htm">Radio</a></li>';
	txt += '<li><a href="check.htm">Check</a></li>';
	txt += '<li><a href="edit.htm">Edit</a></li>';
	txt += '<li><a href="scroll.htm">Scroll</a></li>';
	txt += '<li><a href="image.htm">Image</a></li>';
	txt += '<li><a href="list.htm">List</a></li>';
	txt += '<li><a href="link.htm">Link</a></li>';
	txt += '<li><a href="text.htm">Text</a></li>';
	txt += '<br />';
	txt += '<li><a href="divider.htm">Divider</a></li>';
	txt += '<li><a href="panel.htm">Panel</a></li>';
	txt += '<li><a href="tab.htm">Tab</a></li>';
	txt += '<br />';
	txt += '<li><a href="window.htm">@Window</a></li>';
	txt += '<li><a href="dialog.htm">Dialog</a></li></ul>';
	txt += '<br />';
	txt += '<a class=i href="#15">DCX Supported Features</a><ul>';
	txt += '<li><a href="layout.htm">Cell Layout Algorithm</a></li>';
	txt += '<li><a href="xpopup.htm">XPopup Menus</a></li></ul>';
	txt += '<br />';
	txt += '<a class=i href="#14">Latest Features</a> [<a class=i2 href="changes.htm">all</a>]<br />';
	txt += '<a class=i href="changes.htm#15">Coming Soon</a><br />';

	document.write(txt);
}

// changes title for each document
document.title = "DCX - Dialog Control Xtension DLL v" + v() + " by ClickHeRe";