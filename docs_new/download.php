<?
header("Cache-Control: no-cache, must-revalidate");	// HTTP/1.1
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");	// Date in the past

// latest stable version
$stable = '1.4.0';

// archive directory
$archive = 'archive';

// version => date released
$versions = array(
    '1.4.0' => 1198548748,
    '1.3.7' => 1168739772,
    '1.3.6' => 1159028955,
    '1.3.5' => 1155404032,
	'1.3.4' => 1150614000,
	'1.3.3' => 1149420240,
	'1.3.2' => 1149180480,
	'1.3.1' => 1147423920,
	'1.3.0' => 1145517420,
	'1.2.9' => 0,
	'1.2.8' => 0,
	'1.2.6' => 0,
	'1.2.5' => 0,
	'1.2.3' => 0,
	'1.2.1' => 0,
	'1.1.5' => 0,
);

$version = isset($_GET['v']) ? $_GET['v'] : '';

// if a version is specified
if ($version) {
	if ($version == 'stable')
	    $version = $stable;

	header("Location: $archive/dcx_v$version.zip");
	exit();
}

function format_filesize($size) {
	if     ($size /1000000000 >= 1) { return round($size /1073741824, 2) . 'gb'; }
	elseif ($size /1000000    >= 1) { return round($size /1048576   , 2) . 'mb'; }
	elseif ($size /1000       >= 1) { return round($size /1024      , 2) . 'kb'; }
	else                            { return $size . 'b'; }
}


echo "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?".">";
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

<title>DCX Download Archive - Dialog Control Xtension DLL by ClickHeRe &amp; twig*</title>

<link href="dcx.css" rel="stylesheet" type="text/css" />
<base target="_top" />
</head>

<body>

<table>
<tr><th>Latest Stable</th><th>Release Date</th><th>Link</th><th>Size</th><th>Change Log</th></tr>
<?
// show link for latest stable release
echo "<tr>
<td><strong>v$stable</strong></td>
<td>" . ($versions[$stable] ? date("jS F, Y", $versions[$stable]) : 'Unknown') . "</td>";

	$file = "$archive/dcx_v$stable.zip";

	if (file_exists($file)) {
		echo "<td><a href=\"{$_SERVER['PHP_SELF']}?v=$stable\">Download</a></td>";
		echo "<td>" . format_filesize(filesize($file)) . "</td>";
	}
	else {
	    echo "<td>(Missing)</td><td>&nbsp;</td>";
	}

echo "<td><a href=\"changes.htm#v$stable\">Changes</a></td>
</tr>";

echo "<tr><td colspan='4'>&nbsp;</td></tr>";

// list all download versions
echo "<tr><th>Version</th><th>Release Date</th><th>Link</th><th>Size</th><th>Change Log</th></tr>";

foreach ($versions as $version => $datereleased) {
	// skip stable version
	if ($version == $stable)
		continue;
	
	echo "<tr>
	<td><strong>v$version</strong></td>
	<td>" . ($datereleased ? date("jS F, Y", $datereleased) : 'Unknown') . "</td>";

	$file = "$archive/dcx_v$version.zip";

	if (file_exists($file)) {
		echo "<td><a href=\"{$_SERVER['PHP_SELF']}?v=$version\">Download</a></td>";
		echo "<td>" . format_filesize(filesize($file)) . "</td>";
	}
	else {
	    echo "<td>(Missing)</td><td>&nbsp;</td>";
	}

	echo "<td><a href=\"changes.htm#v$version\">Changes</a></td>
	</tr>";
}

echo "</table>";
?>

</body>
</html>
