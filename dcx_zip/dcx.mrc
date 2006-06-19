alias test {
  clearall
  dialog -m dcx dcx
}

ON *:DIALOG:dcx:*:*: {

  if ( $devent == init ) {

    echo -s Marking Dialog ..
    echo -s $dcx( Mark, $dname dcx_call )

    ;xdialog -c $dname 1 divider 0 50 150 200
    ;/xdid -[l|r] dialog ID [MIN] [IDEAL][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
    ;xdid -l $dname 1 60 0 $chr(9) 2 divider 0 0 0 0
    ;Grey
    ;xdid -r $dname 1 100 0 $chr(9) 3 panel 0 0 0 0
    ;Red
    ;xdid -l $dname 2 20 0 $chr(9) 4 panel 0 0 0 0
    ;xdid -r $dname 2 40 0 $chr(9) 5 divider 0 0 0 0
    ;Green
    ;xdid -l $dname 5 20 0 $chr(9) 6 panel 0 0 0 0
    ;Blue
    ;xdid -r $dname 5 20 0 $chr(9) 7 panel 0 0 0 0
    ;xdid -v $dname 1 300
    ;xdid -C $dname 4 +b $rgb(128,10,10)
    ;xdid -C $dname 6 +b $rgb(10,255,10)
    ;xdid -C $dname 7 +b $rgb(0,10,255)
    ;xdialog -l $dname root $chr(9) +p 0 0 0 0
    ;xdialog -l $dname cell root $chr(9) +li 1 1 0 0
    ;xdialog -l $dname space root $chr(9) + 0 0 0 0
    ;xdid -l $dname 6 root $chr(9) +p 0 0 0 0
    ;xdid -l $dname 6 cell root $chr(9) +li 7 1 0 0
    ;xdid -l $dname 6 space root $chr(9) + 2 2 2 2
    ;.timer 1 0 xdialog -l $dname update

    xdialog -b dcx +tyzsmn
    ;xdialog -q dcx +r wait

    ;dcx_listview
    ;dcx_listview2
    ;dcx_treeview
    ;dcx_pbar
    ;dcx_rebar
    ;dcx_rebar2
    ;dcx_combo
    ;dcx_trackbar
    ;dcx_win
    ;dcx_dialog
    ;dcx_statusbar
    ;dcx_button
    ;dcx_richedit
    dcx_colorcombo
    ;dcx_updown
    ;dcx_toolbar
    ;dcx_layout
    ;dcx_tab
    ;dcx_divider
    ;dcx_panel
    ;dcx_panel2
    ;dcx_list
    ;dcx_link
    ;dcx_image
    ;dcx_mix
    ;dcx_updown
    ;dcx_ipaddress
    ;dcx_webctrl
    ;dcx_calendar
    ;dcx_box2
    ;dcx_box3

    ;xdialog -a dcx +ab 1000

    ;did -o $dname 201 My Menu
    ;did -o $dname 202 1 $chr(11) blah2
    ;did -c $dname 202
    ;did -o $dname 203 1 $chr(11) blah3 $chr(9) CTRL-H
    ;did -b $dname 203
    ;did -o $dname 204 1 $chr(11) blah4
    ;did -cb $dname 204
    ;did -r $dname 206 1 $chr(11) submenu $chr(9) CTRL-S
    ;did -o $dname 207 1 $chr(11) blah2
    ;did -c $dname 207
    ;did -o $dname 208 1 $chr(11) blah3 $chr(9) CTRL-H
    ;did -b $dname 208
    ;did -o $dname 209 1 $chr(11) blah4
    ;did -cb $dname 209

  }
  else if ( $devent == menu ) {
    echo -s $devent $did
  }
}

alias lvsort {
  echo -s $1 : $2 : $3
  return 0
}

dialog dcx {
  title "DCX - Dialog Control Xtension"
  size -1 -1 520 500
  option pixel

  menu "Menu", 201
  item "a", 202
  item "a", 203
  item "a", 204
  item break, 205
  menu "a", 206, 201
  item "a", 207, 206
  item "a", 208
  item "a", 209
}

;##############################

; [DIALOG] [EVENT] [ID] [PARMS]
alias dcx_call {

  if ( $2 == sizing || $2 == moving || $2 == mouse || $2 == mouseenter || $2 == mouseleave ) { return }

  ;if ( $2 == beginsize ) return nosize

  if ( $2 == max ) return stop

  echo -s 1: $1 - 2: $2 - 3: $3 - 4: $4-

  ;if ( $2 == close ) return noclose
  ;if ( $2 == escclose ) return noclose


  ;if ( $2 == sclick && $3 == 3 ) {
  ;  echo -s $xdid(dcx,3).seltext
  ;}
  if ( $2 == dclick && $3 == 5 ) {
    echo -s $xdid(dcx,5, 2).seltext
  }


  if ( $2 == labelend ) {

    if ( $3 == 3 ) {
      return noedit
    }
  }

  if ( $2 == stateclick && $3 == 3 ) {
    ;echo -s $xdid($1,$3,$5).state
  }

  if ( $2 == close ) {
    ;window -c @arf
  }

}

;##############################

alias dcx_listview2 { 

  xdialog -c dcx 5 listview 10 10 420 250 report showsel fullrow editlabel subitemimage

  xdid -w dcx 5 +n 113 $shell
  xdid -w dcx 5 +n 112 $shell

  ;xdid -g dcx 5 +n 50 50 $mircdirdcx/images/dcx.png
  ;xdid -i dcx 5 +k none
  ;xdid -i dcx 5 +b $rgb(255,0,0)

  xdid -t dcx 5 +l 2 120 column 1 $chr(9) +c 1 120 column 2 $chr(9) +rb 2 120 column 3
  xdid -q dcx 5 1 +l 1 Group Text 1
  xdid -q dcx 5 2 +l 2 Group Text 2
  xdid -q dcx 5 3 +l 3 Group Text 3
  xdid -m dcx 5 1

  xdid -a dcx 5 0 0 +u 2 0 0 1 -1 -1 text $chr(9) + 0 0 $chr(9) + 0 text0
  xdid -a dcx 5 0 0 +u 2 0 0 2 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text1
  xdid -a dcx 5 0 0 +u 2 0 0 3 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text2
  xdid -a dcx 5 0 0 +u 2 0 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text3
  xdid -a dcx 5 0 0 +u 2 0 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text4
  xdid -a dcx 5 0 0 +u 2 0 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text5
  xdid -a dcx 5 0 0 +u 2 0 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text6
  xdid -a dcx 5 0 0 +u 2 0 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text7
  xdid -a dcx 5 0 0 +u 2 0 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text8
  xdid -a dcx 5 0 0 +u 2 0 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text9
}


alias dcx_panel2 {

  xdialog -c dcx 40 panel 10 10 500 280
  xdid -c dcx 40 38 listview 10 10 420 250 report showsel fullrow checkbox editlabel

  xdid -w dcx 38 +n 113 $shell
  xdid -w dcx 38 +n 112 $shell
  xdid -w dcx 38 +n 0 canada.ico

  xdid -t dcx 38 +l 2 120 column 1 $chr(9) +c 1 120 column 2

  xdid -a dcx 38 1 0 +cbku 1 2 0 0 -1 $rgb(170,213,255) ClickHeRe
  xdid -a dcx 38 2 0 +u 2 0 0 0 -1 -1 Attila


  ;xdid -c dcx 40 36 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel
  ;dialog -m dcx2 dcx2
  ;xdid -c dcx 40 2 dialog 220 30 200 200 dcx2
  ;window -h @arf
  ;xdid -c dcx 40 2 window 220 30 200 200 @arf
}


alias dcx_link {

  xdialog -c dcx 91 link 10 10 170 20
  xdid -t dcx 91 TEST HERE
  xdid -w dcx 91 0 canada.ico
}


alias dcx_box3 {

  xdialog -g dcx +b $rgb(0,0,255)
  xdialog -c $dname 109 box 10 18 120 100
  xdid -t $dname 109 Informations
  ;xdid -c $dname 109 110 comboex 13 25 90 70 dropdown
  xdid -C dcx 109 +b $rgb(0,0,255)

}

alias dcx_box2 { 

  ;xdialog -g dcx +b $rgb(0,0,255)

  xdialog -c dcx 40 box 10 10 500 380 center bottom
  xdid -t dcx 40 Group Box

  ;xdid -C dcx 40 +bk $rgb(0,0,255)
  ;xdid -C dcx 40 +t $rgb(255,255,255)

  xdid -c dcx 40 203 button 20 22 82 22
  xdid -t dcx 203 Lookup
  ;xdid -f dcx 203 + default 10 Tahoma
  ;xdid -k dcx 203 +dhns 6052956

  xdid -c dcx 40 33  button 20 52 82 22 bitmap

  xdid -k dcx 33 +n $rgb(255,0,255) $scriptdirstart.bmp
  xdid -k dcx 33 +h $rgb(255,0,255) $scriptdirstart_hover.bmp
  xdid -k dcx 33 +s $rgb(255,0,255) $scriptdirstart_pressed.bmp
  xdid -k dcx 33 +d $rgb(255,0,255) $scriptdirstart.bmp

  ;xdid -c dcx 40 36 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel
  ;xdid -c dcx 40 38 listview 10 10 420 250 report showsel fullrow checkbox editlabel noheader
  ;xdid -c dcx 40 39 richedit 10 10 420 250 multi vsbar hsbar
  ;xdid -l dcx 40 root $chr(9) +li 36 1 0 0
  ;xdid -l dcx 40 space root $chr(9) + 5 20 5 5

  ;xdid -l dcx 40 root $chr(9) +ph 0 0 0 0
  ;xdid -l dcx 40 space root $chr(9) + 5 20 5 5
  ;xdid -l dcx 40 cell root $chr(9) +fwi 36 0 120 0
  ;xdid -l dcx 40 space 1 $chr(9) + 0 0 10 0
  ;xdid -l dcx 40 cell root $chr(9) +pv 0 1 0 0
  ;xdid -l dcx 40 cell 2 $chr(9) +li 38 1 0 0
  ;xdid -l dcx 40 space 2 1 $chr(9) + 0 0 0 5
  ;xdid -l dcx 40 cell 2 $chr(9) +li 39 2 0 0
  ;xdid -l dcx 40 space 2 2 $chr(9) + 0 5 0 0

  ;xdialog -l dcx root $chr(9) +li 40 1 0 0
  ;.timer 1 0 xdialog -l dcx update

}

alias dcx_calendar {

  xdialog -c dcx 101 calendar 10 10 200 200 multi
}

alias dcx_webctrl {

  xdialog -c dcx 100 webctrl 10 10 500 480
  xdid -n dcx 100 http://icyserve.webtechnet.net:8080/fweather/request/wsnag.php?request=lookup&zone=
  xdialog -l dcx root $chr(9) +li 100 1 0 0
  .timer 1 0 xdialog -l dcx update
}

alias dcx_win {

  window -h @arf 20 20 150 100

  xdialog -c dcx 40 panel 10 10 500 280

  xdid -c dcx 40 1 window 10 30 200 200 @arf

  ;xdialog -c dcx 1 window 10 30 200 200 @arf
}


alias dcx_ipaddress {

  xdialog -c dcx 94 ipaddress 10 10 150 25
  xdid -a dcx 94 255.255.0.0
  echo -s $xdid(dcx,94).ip

}


alias dcx_updown { 

  xdialog -c dcx 95 edit 10 10 70 25
  xdialog -c dcx 93 updown 70 10 10 25 wrap buddyint
  xdid -c dcx 93 95
  xdid -r dcx 93 0 25
  xdid -v dcx 93 12
}

alias dcx_listview { 

  xdialog -c dcx 5 listview 10 10 420 150 report showsel fullrow editlabel subitemimage tooltip checkbox 

  xdid -w dcx 5 +n 113 $shell
  xdid -w dcx 5 +n 112 $shell

  ;xdid -g dcx 5 +n 50 50 $mircdirdcx/images/dcx.png
  ;xdid -i dcx 5 +k none
  ;xdid -i dcx 5 +b $rgb(255,0,0)

  xdid -t dcx 5 +l 2 120 column 1 $chr(9) +c 1 120 column 2 $chr(9) +rb 2 120 column 3
  xdid -a dcx 5 1 0 +cbku 1 0 0 0 -1 $rgb(170,213,255) text $chr(9) + 0 0 $chr(9) + 0 text0
  xdid -a dcx 5 2 0 +u 2 1 0 0 -1 -1 text $chr(9) + 0 1 $chr(9) + 0 text1
  xdid -a dcx 5 3 0 +cbku 1 2 0 0 -1 $rgb(170,213,255) text $chr(9) + 0 2 $chr(9) + 0 text2
  xdid -a dcx 5 4 0 + 2 0 0 0 -1  -1 text $chr(9) + 0 3 $chr(9) + 0 text3
  xdid -a dcx 5 5 0 +cbku 1 0 0 0 -1 $rgb(170,213,255) text $chr(9) + 0 4 $chr(9) + 0 text4
  xdid -a dcx 5 6 0 + 2 0 0 0 -1 -1 text $chr(9) + 0 5 $chr(9) + 0 text5
  xdid -a dcx 5 7 0 +cbku 1 0 0 0 -1 $rgb(170,213,255) text $chr(9) + 0 6 $chr(9) + 0 text6
  xdid -a dcx 5 8 0 + 2 0 0 0 -1 -1 text $chr(9) + 0 7 $chr(9) + 0 text7
  xdid -a dcx 5 9 0 +cbku 1 0 0 0 -1 $rgb(170,213,255) text $chr(9) + 0 8 $chr(9) + 0 text8
  xdid -a dcx 5 10 0 + 2 0 0 0 -1 -1 text $chr(9) + 0 9 $chr(9) + 0 text9
  xdid -a dcx 5 11 0 +cbku 1 0 0 0 -1 $rgb(170,213,255) text $chr(9) + 0 10 $chr(9) + 0 text10

}

alias dcx_treeview {

  xdialog -c dcx 3 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel fullrow
  xdid -l dcx 3 16
  xdid -w dcx 3 +n 113 $shell
  xdid -w dcx 3 +n 112 $shell

  xdid -a dcx 3 $tab(-1,+ebcu 1 2 1 1 0 255 2, tooltip)
  xdid -a dcx 3 $tab(-1,+sbcu 1 2 1 2 0 $rgb(0,0,255) 1, tooltip)
  xdid -a dcx 3 $tab(1 -1,+ 1 0 0 0 0 0 Item 3, tooltip)
  xdid -a dcx 3 $tab(1 1,+ 1 0 0 0 0 0 Item 4, tooltip)
}

alias dcx_pbar {

  xdialog -c dcx 2 pbar 140 30 200 25 smooth notheme
  xdid -r dcx 2 0 100
  xdid -v dcx 2 75
}

alias dcx_rebar {

  xdialog -c dcx 20 rebar 10 10 120 400 varheight vertical dblclktoggle notheme

  xdid -w dcx 20 113 $shell
  xdid -w dcx 20 112 $shell
  xdid -w dcx 20 0 canada.ico

  xdid -a dcx 20 -1 +c 0 120 250 1 $rgb(0,0,255) Treeview $chr(9) 21 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel
  xdid -a dcx 20 -1 +c 0 120 100 3 $rgb(255,0,0) Listview $chr(9) 22 listview 10 10 420 250 report showsel fullrow checkbox editlabel noheader

  xdid -w dcx 22 +n 113 $shell
  xdid -w dcx 22 +n 112 $shell
  xdid -w dcx 22 +n 0 canada.ico

  xdid -t dcx 22 +l 0 110
  xdid -a dcx 22 1 0 +cbku 1 2 0 0 -1 $rgb(170,213,255) ClickHeRe
  xdid -a dcx 22 2 0 +u 2 0 0 0 -1 -1 Attila

  xdid -w dcx 21 +n 113 $shell
  xdid -w dcx 21 +n 112 $shell

  xdid -a dcx 21 $tab(-1,+bcu 1 2 1 1 0 255 Item 2, tooltip)
  xdid -a dcx 21 $tab(-1,+bcu 1 2 1 2 0 $rgb(0,0,255) Item 1, tooltip)
  xdid -a dcx 21 $tab(1 -1,+ 1 0 0 0 0 0 Item 3, tooltip)
  xdid -a dcx 21 $tab(1 1,+ 1 0 0 0 0 0 Item 4, tooltip)
}

alias dcx_rebar2 {

  xdialog -c dcx 10 rebar 10 10 400 400 varheight notheme

  xdid -w dcx 10 113 $shell
  xdid -w dcx 10 112 $shell
  xdid -a dcx 10 -1 + 0 27 200 1 $rgb(0,0,255) Button $chr(9) 33 button 0 0 0 0
  xdid -a dcx 10 -1 + 0 27 200 1 $rgb(0,0,255) ColorCombo $chr(9) 11 colorcombo 0 0 0 0
  xdid -a dcx 10 -1 +c 0 23 200 1 $rgb(0,0,255) Trackbar $chr(9) 14 trackbar 0 0 0 0 tooltips
  xdid -a dcx 10 -1 +c 0 23 100 2 $rgb(0,255,0) ComboEx $chr(9) 12 comboex 10 10 100 100 dropedit
  xdid -a dcx 10 -1 +c 0 23 50 0 $rgb(255,0,0) Toolbar $chr(9) 13 toolbar 0 0 0 0 list flat tooltips nodivider

  xdid -l dcx 13 16
  xdid -w dcx 13 +n 113 $shell
  xdid -w dcx 13 +d 112 $shell
  xdid -w dcx 13 +h 114 $shell

  xdid -a dcx 13 1 +lc 100 1 $rgb(255,0,0) a a $chr(9) Tooltip
  xdid -a dcx 13 2 + 0 0 0 -
  xdid -a dcx 13 3 +lc 100 1 $rgb(0,0,255) b b $chr(9) Tooltip

  xdid -w dcx 12 113 $shell
  xdid -w dcx 12 112 $shell
  xdid -a dcx 12 1 0 1 2 0 Blah
  xdid -a dcx 12 2 0 1 2 0 Blah2

  xdid -m dcx 11
}

alias dcx_trackbar {

  xdialog -c dcx 8 trackbar 100 10 300 40 tooltips select notheme noticks
  xdid -r dcx 8 0 100
  ;xdid -r dcx 8 250 600000000
  xdid -v dcx 8 50
  xdid -j dcx 8 0 75
}

alias dcx_combo {

  xdialog -c dcx 6 comboex 10 10 420 250 dropedit
  xdid -w dcx 6 113 $shell
  xdid -w dcx 6 112 $shell

  xdid -a dcx 6 1 0 1 2 0 Blah
  xdid -a dcx 6 2 0 1 2 0 Blah3
  xdid -a dcx 6 3 0 1 2 0 Blah2

}

alias dcx_mix {

  xdialog -g dcx +b $rgb(0,0,255)

  xdialog -c dcx 80 line 240 90 200 38
  xdialog -c dcx 81 line 240 100 10 100 vertical

  xdialog -c dcx 82 box 20 20 200 200 center bottom
  xdid -t dcx 82 Group Box

  xdialog -c dcx 83 radio 25 100 100 20 tabstop group
  xdid -t dcx 83 Radio
  xdid -c dcx 83

  xdialog -c dcx 84 radio 25 120 100 20 rjustify
  xdid -t dcx 84 Radio

  xdialog -c dcx 85 check 25 140 100 20 3state
  xdid -t dcx 85 Check

  xdialog -c dcx 86 text 25 240 150 40 center
  xdid -t dcx 86 This is a long long long text

  xdialog -c dcx 87 text 25 280 150 40 endellipsis
  xdid -t dcx 87 This is a long long long text

  xdialog -c dcx 88 edit 25 320 170 70 autohs multi return vsbar
  xdid -a dcx 88 This is

  xdialog -c dcx 89 scroll 25 400 150 20 tabstop
  xdid -r dcx 89 0 100
  xdid -v dcx 89 50

  xdialog -c dcx 40 panel 210 300 270 180
  xdid -c dcx 40 36 scroll 10 30 250 20 tabstop
  xdid -r dcx 36 0 100
  xdid -v dcx 36 50

  xdid -c dcx 40 37 text 10 60 250 20 tabstop
  xdid -t dcx 37 This is a long long long text

  xdid -C dcx 82,83,84,85,86,87,88,89,40,36,37 +bk $rgb(0,0,255)
  xdid -C dcx 82,83,84,85,86,87,88,89,40,36,37 +t $rgb(255,255,255)
}

alias dcx_dialog {

  dialog -m dcx2 dcx2
  xdialog -c dcx 2 dialog 220 30 200 200 dcx2
}

alias dcx_statusbar {

  xdialog -c dcx 4 statusbar 10 10 120 250 tooltips
  xdid -w dcx 4 113 $shell
  xdid -w dcx 4 112 $shell
  xdid -l dcx 4 150 300 -1

  xdid -t dcx 4 1 +p 1 Cell Text $chr(9) Cell Text
  xdid -t dcx 4 2 +p 2 Cell Text $chr(9) Cell Text
  xdid -t dcx 4 3 +p 1 $chr(9) Cell Text
}

alias dcx_button {

  xdialog -c dcx 32 button 140 10 100 30
  xdialog -c dcx 33 button 140 50 98 28 bitmap
  xdialog -c dcx 34 button 140 90 38 38

  xdid -l dcx 32 16

  xdid -w dcx 32 113 $shell
  xdid -w dcx 32 112 $shell
  xdid -w dcx 32 114 $shell
  xdid -w dcx 32 115 $shell

  xdid -k dcx 32 +n $rgb(0,0,0)
  xdid -k dcx 32 +h $rgb(255,0,0)
  xdid -k dcx 32 +s $rgb(0,0,255)
  xdid -k dcx 32 +d $rgb(200,200,200)
  xdid -t dcx 32 arf

  xdid -k dcx 33 +n $rgb(255,0,255) $scriptdirstart.bmp
  xdid -k dcx 33 +h $rgb(255,0,255) $scriptdirstart_hover.bmp
  xdid -k dcx 33 +s $rgb(255,0,255) $scriptdirstart_pressed.bmp
  xdid -k dcx 33 +d $rgb(255,0,255) $scriptdirstart.bmp

  xdid -l dcx 34 32

  xdid -w dcx 34 113 $shell
  xdid -w dcx 34 112 $shell
  xdid -w dcx 34 114 $shell
  did -w dcx 34 115 $shell
}

alias dcx_richedit {
  xdialog -c dcx 9 richedit 100 10 400 120 multi hsbar
  xdid -k dcx 9 $rgb(255,0,0)
  xdid -a dcx 9 arf arf
  ;xdid -ra dcx 9 1,15 1,15 5:53p   (2KaLLoK) s'toi la truite moulue $+ $crlf $+ 01,15  5:53p   02* 01Kenshiro|Away 03[away] Ailleurs $+ $crlf $+ 01,15  5:57p   02»03»01» 02N01ick 2Enki|jVtfRlaPo ---> 02Enki|_other_pseudo $+ $crlf $+ 01,15  5:59p   02»03»01» 02N01ick 2NaNaU_|[Deco] ---> 02NaNaU_|[_kelly_] $+ $crlf $+ 01,15  6:04p   02»03»01» 02J01oin Zsol (03guest@619.729.642.61201)
}

alias dcx_colorcombo {
  xdialog -c dcx 30 colorcombo 10 10 150 100
  xdialog -c dcx 6 comboex 160 10 120 250 dropedit

  xdid -m dcx 30
  ;xdid -c dcx 30 1
}

alias dcx_updown {
  xdialog -c dcx 31 updown 140 10 100 100
}

alias dcx_toolbar {

  xdialog -c dcx 7 toolbar 10 10 420 22 list transparent tooltips flat arrows

  xdid -l dcx 7 16
  xdid -j dcx 7 100 100
  xdid -w dcx 7 +n 113 $shell
  xdid -w dcx 7 +d 112 $shell
  xdid -w dcx 7 +h 114 $shell

  xdid -a dcx 7 1 +cv 100 1 $rgb(255,0,0) a a $chr(9) Tooltip
  xdid -a dcx 7 2 + 30 30 30 -
  xdid -a dcx 7 3 +c 200 1 $rgb(0,0,255) b b $chr(9) Tooltip
  xdid -a dcx 7 4 +c 200 1 $rgb(0,0,255) c c $chr(9) Tooltip
  xdid -a dcx 7 5 +c 200 1 $rgb(0,0,255) d d $chr(9) Tooltip
}

alias dcx_divider {

  xdialog -c dcx 35 divider 10 10 500 400 notheme vertical
  xdid -l dcx 35 100 100 $chr(9) 36 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel
  xdid -r dcx 35 100 100 $chr(9) 37 divider 10 10 420 250
  xdid -v dcx 35 200
  xdid -l dcx 37 100 100 $chr(9) 38 listview 10 10 420 250 report showsel fullrow checkbox editlabel noheader
  xdid -r dcx 37 100 100 $chr(9) 39 richedit 10 10 420 250 multi vsbar hsbar
  xdid -v dcx 37 200


  xdialog -l dcx root $chr(9) +li 35 1 0 0
  xdialog -l dcx update

}

alias dcx_tab {

  xdialog -c dcx 50 tab 10 10 400 100
  xdid -w dcx 50 113 $shell
  xdid -w dcx 50 112 $shell

  xdid -a dcx 50 0 1 blah1 $chr(9) 36 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel $chr(9) arf
  xdid -a dcx 50 0 2 blah2 $chr(9) 37 divider 10 10 420 250 $chr(9) arf
  xdid -l dcx 37 100 100 $chr(9) 38 listview 10 10 420 250 report showsel fullrow checkbox editlabel noheader
  xdid -r dcx 37 100 100 $chr(9) 39 richedit 10 10 420 250 multi vsbar hsbar

  xdialog -l dcx root $chr(9) +li 50 0 0 0
  xdialog -l dcx space root $chr(9) + 2 2 2 2
  xdialog -l dcx update
}

alias dcx_panel {

  xdialog -c dcx 40 panel 10 10 500 280

  xdid -c dcx 40 36 treeview 10 30 120 250 haslines linesatroot hasbuttons checkbox editlabel showsel
  xdid -c dcx 40 38 listview 10 10 420 250 report showsel fullrow checkbox editlabel
  xdid -c dcx 40 39 richedit 10 10 420 250 multi vsbar hsbar
  xdid -l dcx 40 root $chr(9) +ph 0 0 0 0
  xdid -l dcx 40 space root $chr(9) + 5 5 5 5
  xdid -l dcx 40 cell root $chr(9) +fwi 36 0 120 0
  xdid -l dcx 40 space 1 $chr(9) + 0 0 10 0
  xdid -l dcx 40 cell root $chr(9) +pv 0 1 0 0
  xdid -l dcx 40 cell 2 $chr(9) +li 38 1 0 0
  xdid -l dcx 40 space 2 1 $chr(9) + 0 0 0 5
  xdid -l dcx 40 cell 2 $chr(9) +li 39 2 0 0
  xdid -l dcx 40 space 2 2 $chr(9) + 0 5 0 0

  xdid -w dcx 36 +n 113 $shell
  xdid -w dcx 36 +n 112 $shell

  xdid -a dcx 36 $tab(-1,+bcu 1 2 1 1 0 255 Item 2, tooltip)
  xdid -a dcx 36 $tab(-1,+bcu 1 2 1 2 0 $rgb(0,0,255) Item 1, tooltip)
  xdid -a dcx 36 $tab(1 -1,+ 1 0 0 0 0 0 Item 3, tooltip)
  xdid -a dcx 36 $tab(1 1,+ 1 0 0 0 0 0 Item 4, tooltip)

  xdid -w dcx 38 +n 113 $shell
  xdid -w dcx 38 +n 112 $shell
  xdid -w dcx 38 +n 0 canada.ico

  xdid -t dcx 38 +l 2 120 column 1 $chr(9) +c 1 120 column 2

  xdid -a dcx 38 1 0 +cbku 1 2 0 0 -1 $rgb(170,213,255) ClickHeRe
  xdid -a dcx 38 2 0 +u 2 0 0 0 -1 -1 Attila

  .timer 1 0 xdid -l dcx 40 update
}

alias dcx_layout {

  ;xdialog -l dcx root $chr(9) +ph 0 0 0 0
  ;xdialog -l dcx cell root $chr(9) +l 0 1 0 0
  ;xdialog -l dcx cell root $chr(9) +pv 0 8 0 0
  ;xdialog -l dcx cell root $chr(9) +l 0 1 0 0
  ;xdialog -l dcx cell 2 $chr(9) +l 0 1 0 0
  ;xdialog -l dcx cell 2 $chr(9) +li 3 8 0 0
  ;xdialog -l dcx cell 2 $chr(9) +l 0 1 0 0

  ;xdialog -l dcx root $chr(9) +ph 0 0 0 0
  ;xdialog -l dcx space root $chr(9) + 5 5 5 5
  ;xdialog -l dcx cell root $chr(9) +fwi 3 0 120 0
  ;xdialog -l dcx space 1 $chr(9) + 0 0 10 0
  ;xdialog -l dcx cell root $chr(9) +pv 0 1 0 0
  ;xdialog -l dcx cell 2 $chr(9) +li 5 1 0 0
  ;xdialog -l dcx space 2 1 $chr(9) + 0 0 0 5
  ;xdialog -l dcx cell 2 $chr(9) +li 9 2 0 0
  ;xdialog -l dcx space 2 2 $chr(9) + 0 5 0 0
  ;xdialog -l dcx update
}

alias dcx_list {

  xdialog -c dcx 90 list 10 10 100 80 multi
  xdid -a dcx 90 0 Item 1
  xdid -a dcx 90 0 Item 2
  xdid -a dcx 90 0 Item 4
  xdid -a dcx 90 0 Item 3
}

alias dcx_image {

  xdialog -c dcx 92 image 10 10 128 128
  xdid -C dcx 92 +b $rgb(face)
  xdid -k dcx 92 $rgb(255,0,255)
  xdid -i dcx 92 $scriptdirpngtr.bmp

  xdialog -c dcx 93 image 200 10 128 128
  xdid -w dcx 93 0 32 $scriptdirCanada.ico
}


;##############################

dialog dcx2 {
  title "DCX - Dialog Control Xtension - docked"
  size -1 -1 520 500
  option pixel
}
