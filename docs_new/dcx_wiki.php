<?php
// Do formatting of wiki code here
// http://www.phpit.net/article/create-bbcode-php/
function wikiData(&$value, $userdata = "") {
	// new code
	$str = $value;

	// TODO: links for tags
	$simple_search = array(
		'/\[v\](.*?)\[\/v\]/is', // value
		'/\[e\](.*?)\[\/e\]/is', // event
		'/\[r\](.*?)\[\/r\]/is', // returnevent
		'/\[s\](.*?)\[\/s\]/is', // style
		'/\[p\](.*?)\[\/p\]/is', // param
//		'/\[i\](.*?)\[\/i\]/is', // property - unused
		'/\[o\](.*?)\[\/o\]/is', // operating system
                '/\[os\](.*?)\[\/os\]/is', // operating system
		'/\[n\](.*?)\[\/n\]/is', // note
		'/\[f\](.*?)\[\/f\]/is', // flag
		'/\[code\](.*?)\[\/code\]/is', // code
	);

	$simple_replace = array(
		'<a class="value">$1</a>',
		'<a class="event">$1</a>',
		'<a class="returnevent">$1</a>',
		'<a class="style">$1</a>',
		'<a class="param">$1</a>',
//		'<a class="property">$1</a>',
		'<a class="os">($1)</a>',
                '<a class="os">($1)</a>',
		'<a class="note">Note:</a> $1',
		'<a class="value">$1</a>', // flag
		'<pre class="code">$1</pre>', // code
	);

	// Do simple BBCode's
	$value = preg_replace($simple_search, $simple_replace, $str);
        
        // Perform replacement of advanced tags
        // [link page='control' section='SECTION' flag='f|mouse']link text[/link]
        getTagInfo($value, 'link', 'parseAdvancedWikiLink');
}

/*
 * Used to extract attributes from tags.
 *
 * (in) $text = text to search
 * (in) $tag = the tag to search for
 * (in) $handler = function to process the result
 *
 * $result = Array(
 *  '__name' => 'tag',
 *  '__tag' => '[tag attr1="value1" attr2="value2" ...]tagValue[/tag]',
 *  '__value' => 'tagValue',
 *  '__attributes' => Array(
 *      'attr1' => 'value1',
 *      'attr2' => 'value2',
 *      'attr3' => 'value3',
 *  ),
 * );
 */
function getTagInfo(&$text, $tag, $handler) {
    // Test if the text contains the tag at all.
    $rx = '/\[' . $tag . '([^\]]*)\](.*?)\[\/' . $tag . '\]/';
    
    preg_match_all($rx, $text, $matches, PREG_SET_ORDER);
    
    if (count($matches) == 0)
        return null;

    foreach ($matches as $match) {
        // Fill in array info.
        $result = array();
        $result['__name'] = $tag;
        $result['__tag'] = $match[0];
        $result['__value'] = $match[2];
        $result['__attributes'] = array();
        
        // Search for attributes.
        $attributes = trim($match[1]);
        
        if (strlen($attributes) > 0) {
            preg_match_all('/(\w+)="(.*?)"/', $attributes, $attributeMatches, PREG_SET_ORDER);
    
            // Fill the result with key/value pairs.
            foreach ($attributeMatches as $key => $value) {
                $result['__attributes'][$value[1]] = $value[2];
            }
        }
        
        $handler($text, $result);
    }
}



// This parses [link] tags
function parseAdvancedWikiLink(&$value, &$tagInfo) {
    $page    = isset($tagInfo['__attributes']['page'])    ? $tagInfo['__attributes']['page']    : '';
    $section = isset($tagInfo['__attributes']['section']) ? $tagInfo['__attributes']['section'] : '';
    $flag    = isset($tagInfo['__attributes']['flag'])    ? $tagInfo['__attributes']['flag']    : '';

    // Check if appropriate attributes have been supplied.
    if ((strlen($section) > 0) && (strlen($flag) == 0)) {
        global $CURRENTPAGE;
            
        error_log("$CURRENTPAGE > Malformed LINK tag: missing flag tag.\n{$tagInfo['__tag']}");
        return;
    }
    else if ((strlen($flag) > 0) && (strlen($section) == 0)) {
        global $CURRENTPAGE;
            
        error_log("$CURRENTPAGE > Malformed LINK tag: missing section tag.\n{$tagInfo['__tag']}");
        return;
    }

    if (strlen($page) > 0) {
        global $PAGES;

        if (!isset($PAGES[$page])) {
            global $CURRENTPAGE;
            
            error_log("$CURRENTPAGE > Malformed LINK tag: page '$page' not found.\n{$tagInfo['__tag']}");
            return;
        }
        
        if ($page == 'cla')
            $page = 'layout';
        
        $page .= '.htm';
    }

    // Create the URL.
    $url = $page;

    // Fix flags for capitals
    if (strlen($flag) > 0) {
        if (ctype_upper($flag)) {
            $flag = 'big.' . $flag;
        }
    }

    if ((strlen($section) > 0) && (strlen($flag) > 0))
        $url .= "#$section.$flag";

    if (strlen($url) == 0) {
        global $CURRENTPAGE;
        
        error_log("$CURRENTPAGE > Malformed LINK tag.\n{$tagInfo['__tag']}");
        return;
    }
    
    $insert = "<a class='value' href='$url'>{$tagInfo['__value']}</a>";

    $value = str_replace($tagInfo['__tag'], $insert, $value);
    return;
}
?>