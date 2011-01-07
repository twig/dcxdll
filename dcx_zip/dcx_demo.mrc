/*
###### USAGE:
###### /dcx_demo


###### see dc.xml in the scriptsfolder for dialog's markup
######
*/

alias dcx_demo dialog -m mydialog mydialog
dialog mydialog {
  size 0 0 0 0
}
on *:dialog:mydialog:init:*:{
  dcx Mark $dname events
  japan
  dcxml -dx $dname $dname $qt($scriptdirdc.xml)
  if ($dcxml(Loaded)) {
      xdid -v mydialog 1 20
      xdid -l mydialog 2 16

    ; Populate Listview
      xdid -t mydialog 5 +l 2 100 column 1 $chr(9) +c 1 100 column 2 $chr(9) +rh 2 100 column 3
      var %i = 0 
      while (%i <= 20) { 
        xdid -a mydialog 5 0 0 +cbkm $rand(1,40) 0 0 0 $r(0,16777215) $r(0,16777215) $regsubex($str(.,$r(5,15)),/./g,$r(a,z))  $chr(9) +ckm $rand(1,40) $r(0,16777215) $r(0,16777215) $regsubex($str(.,$r(5,15)),/./g,$r(a,z)) $chr(9) +ckhm $rand(1,40) $r(0,16777215) $r(0,16777215) $regsubex($str(.,$r(5,15)),/./g,$r(a,z)) 
        inc %i
      }
      ; Hide and show controls
      xdid -h mydialog $gettok($regsubex($str(.,27),/./g,$+($xdid(mydialog,\n).pid,$chr(44))),1-,44)
      xdid -s mydialog $xdid(mydialog, 1).pid

      ;Fill colorcombo with mIRC's pallette
      xdid -m mydialog 8
      ;Set buddycontrol for updown
      xdid -c mydialog 12 122
      set %monitor 1
  }
}
alias events { 
  tokenize 32 $1-
  if ($2 == error) echo -s DCXError: $1 ID: $3 Type: $4 Prop: $5 Cmd: $6 Error: $7-
  if ($1 == mydialog) { 
    if ($2 == selchange) {
      if (($3 == $xdialog($1,tvMenu).id) && ($4)) { 
        xdid -h $1 $gettok($regsubex($str(.,27),/./g,$+($xdid(mydialog,\n).pid,$chr(44))),1-,44)
        xdid -s $1 $xdid(mydialog, $4).pid
        xdid -t $1 40 The $xdid(mydialog,$3,$4).text Control
        set %monitor $4
        xdid -r $1 51
        .xdid -l $1 $xdid(mydialog,$xdid(mydialog, $4).pid).pid update
      }
    }
    if (($3 == %monitor) && (($2 != mouse) && ($2 != zlayershow))) { 
      xdid -a $1 51 $crlf $1-
      xdid -Z $1 51 100
    }
    if ($2 == close) { 
      .timerq off | unset %japr %japy | window -c @m | window -c @japan
      unset %monitor
    }
    if ($2 == ready) {
      demo_postinit $1
    }
  }
}
alias -l demo_postinit {
  xdialog -l $1 update
  xdialog -j $1
  xdid -l $1 $xdid($1,$xdid($1, 1).pid).pid update
}
alias _scriptdir return $scriptdir

;###stupid demo for @window
alias japan { window -phCf @m | .timerq -m 0 100 mwi | window -pfh @japan 1 1 240 100 | %japy = 10 }
alias mwi { 
  if (!$window(@m)) return
  inc %japr 3 
  clear -n @m 
  if (%japy = 10) %d = inc
  if (%japy = 100) %d = dec 
  %d %japy 
  var %t = 360 , %or = 0 
  while (%t >= 0) { 
    %or = $xor(1,%or) 
    drawline -rn @m 255 1 201 200 $t(200,$calc(%t + %japr)) $iif(%or,$t(200,$calc((%t -20) + (%japr -3)))) 
    dec %t 20 
  }
  drawfill -nr @m 254 255 1 1 
  drawrect -drfn @m 255 1 185 185 30 30 30 30 
  var %t = 0 , %tx = DCX RULES DONKEYBALLS!!, %a123 = 23 
  while (%t <= 360) { 
    if ($mid(%tx,%a123,1)) && ($mid(%tx,%a123,1) != $chr(32)) { drawtext -rno @m 0 tahoma 9 $t(40,$calc(%t + %japr)) $ifmatch } 
    inc %t 16
    dec %a123
  }
  drawcopy -n @m %japy 170 240 100 @japan 0 0
  drawrect -rn @japan 0 3 0 0 240 100
  drawdot @m
  drawdot @japan
}
alias -l t { return $calc($1 * $cos($2).deg + 201) $calc($1 * $sin($2).deg + 200)  }
on *:CLOSE:@japan: .timerq off | unset %japr %japy | window -c @m
