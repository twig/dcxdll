alias demo {
  dialog -m dcx_demo dcx_demo
}

dialog dcx_demo {
  title "DCX - Dialog Control Xtension Demo"
  size -1 -1 520 500
  option pixel
}

ON *:DIALOG:dcx_demo:*:*: {

  if ( $devent == init ) {
    dcx Mark $dname dcx_event

    ; change dialog border and styles settings
    xdialog -b $dname +tyzsmn

    ; rebar acting as top toolbar
    xdialog -c $dname 1 rebar 10 10 400 400 varheight nodivider dblclktoggle
    xdid -w $dname 1 + 113 $shell
    xdid -w $dname 1 + 112 $shell
    ;xdid -q $dname 1 1

    ; Toolbar in the rebar
    xdid -a $dname 1 -1 +c 0 23 220 0 $rgb(255,0,0) Toolbar $chr(9) 2 toolbar 0 0 0 0 list flat tooltips nodivider
    xdid -l $dname 2 16
    xdid -w $dname 2 +n 112 $shell
    xdid -w $dname 2 +d 113 $shell
    xdid -w $dname 2 +h 114 $shell
    xdid -a $dname 2 0 +ac 0 1 $rgb(255,0,0) Button 1 $chr(9) Tooltip
    xdid -a $dname 2 0 + 0 0 0 -
    xdid -a $dname 2 0 +ac 0 0 $rgb(0,0,255) Button 2 $chr(9) Tooltip
    xdid -a $dname 2 0 + 0 0 0 -
    xdid -a $dname 2 0 +ac 0 1 -1 $chr(9) Tooltip
    xdid -m $dname 2 1

    ; Comboex in the rebar
    xdid -a $dname 1 -1 +c 0 23 100 2 $rgb(0,255,0) ComboEx $chr(9) 3 comboex 10 10 100 100 dropedit
    xdid -w $dname 3 + 113 $shell
    xdid -w $dname 3 + 112 $shell
    xdid -a $dname 3 0 0 1 1 0 Item 1
    xdid -a $dname 3 0 1 2 2 0 Item 2

    ; trackbar
    xdid -a $dname 1 -1 +wc 0 23 200 1 $rgb(0,0,255) Trackbar $chr(9) 4 trackbar 0 0 0 0 tooltips

    ; pbar
    xdid -a $dname 1 -1 +c 0 23 200 1 $rgb(128,0,255) Pbar $chr(9) 5 pbar 0 0 0 0
    xdid -v $dname 5 40


    xdialog -c $dname 50 tab 10 10 400 100 notheme
    xdid -w $dname 50 + 113 $shell
    xdid -w $dname 50 + 112 $shell

    xdid -a $dname 50 0 1 Tab 1 $chr(9) 51 divider 10 10 420 250 $chr(9) arf
    xdid -l $dname 51 100 100 $chr(9) 52 listview 10 10 420 250 report showsel fullrow checkbox editlabel noheader
    xdid -r $dname 51 100 100 $chr(9) 53 richedit 10 10 420 250 multi vsbar hsbar

    xdid -a $dname 50 0 2 Tab 2 $chr(9) 55 divider 10 10 420 250 vertical $chr(9) arf
    xdid -l $dname 55 100 100 $chr(9) 56 listview 10 10 420 250 report showsel fullrow checkbox editlabel noheader
    xdid -r $dname 55 100 100 $chr(9) 57 tab 10 10 420 250 buttons flat flatseps

    xdid -w $dname 57 + 113 $shell
    xdid -w $dname 57 + 112 $shell

    xdid -a $dname 57 0 1 Tab 1 $chr(9) 58 richedit 10 10 420 250 multi vsbar hsbar

    xdid -ra $dname 58 1,15 1,15 5:53p   (2KaLLoK) s'toi la truite moulue $+ $crlf $+ 01,15  5:53p   02* 01Kenshiro|Away 03[away] Ailleurs $+ $crlf $+ 01,15  5:57p   02»03»01» 02N01ick 2Enki|jVtfRlaPo ---> 02Enki|_other_pseudo $+ $crlf $+ 01,15  5:59p   02»03»01» 02N01ick 2NaNaU_|[Deco] ---> 02NaNaU_|[_kelly_] $+ $crlf $+ 01,15  6:04p   02»03»01» 02J01oin Zsol (03guest@619.729.642.61201)
    xdid -a $dname 57 0 2 Tab 2 $chr(9) 59 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel fullrow


    ; bottom statusbar
    xdialog -c $dname 100 statusbar 10 10 120 250 tooltips grip
    xdid -w $dname 100 + 113 $shell
    xdid -w $dname 100 + 112 $shell
    xdid -l $dname 100 150 300 -1

    xdid -t $dname 100 1 +p 1 DCX $chr(9) Cell Text
    xdid -t $dname 100 2 +p 2 Demo $chr(9) Cell Text
    xdid -t $dname 100 3 +p 1 $chr(9) ClickHeRe

    var %r = $xdid($dname,1).pos
    var %s = $xdid($dname,100).pos

    xdialog -l $dname root $chr(9) +li 50 0 0 0
    xdialog -l $dname space root $chr(9) + 0 $calc( $gettok( %r, 4, 32 ) + 2 ) 0 $calc( $gettok( %s, 4, 32 ) - 2 )
    ; needed for the dialog to be opened and have it's full width before updating the display which is after init event
    .timer 1 0 xdialog -l $dname update
  }
}

; [DIALOG] [EVENT] [ID] [PARMS]
alias dcx_event {
  if (($2 == sizing) || ($2 == moving)) return
  if ($2 == max) return
  if ($2 == mouse) return

  echo -s ($1): $1 > ($2): $2 > ($3): $3 > ($4): $4-
/*
  ; capture rebar changing size
  if ( $2 == change && $3 == 1 ) {
    if ( $xdialog( $1, 100 ).isid ) {
      var %s = $xdid( $1, 100 ).pos
      xdialog -l $1 space root $chr(9) + 0 $calc($5 + 2) 0 $calc( $gettok( %s, 4, 32 ) - 2 )
    }
    else {
      xdialog -l $1 space root $chr(9) + 0 $calc($5 + 2) 0 0
    }
    
    xdialog -l $1 update
  }
*/
}



alias -l shell {
  return $scriptdirshell.dll
}
