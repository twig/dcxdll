alias dcx {
  if (%dcx_conflict) { echo 4 -s [DCX ERROR] Dll Conflict, Trying to load multiple copies of DCX: $+(06,$qt($dll(dcx.dll)),) and $+(03,$qt($scriptdirdcx\dcx.dll),) | halt }
  if ($isid) returnex $dll($scriptdirdcx\dcx.dll,$1,$2-)
  else dll " $+ $scriptdirdcx\dcx.dll" $1 $2-
}

alias udcx {
  if ($dcx(IsUnloadSafe)) $iif($menu, .timer -o 1 0 editbox -ns) dll -u dcx.dll
  else echo 4 -qmlbfti2 [DCX] Unable to Unload Dll.
}

alias xdid {
  if ( $isid ) returnex $dcx( _xdid, $1 $2 $prop $3- )
  dcx xdid $2 $3 $1 $4-
}

alias xdialog {
  if ( $isid ) returnex $dcx( _xdialog, $1 $prop $2- )
  dcx xdialog $2 $1 $3-
}

alias xpop {
  if ( $isid ) returnex $dcx( _xpop, $1 $prop $2- )
  dcx xpop $2 $1 $3-
}

alias xpopup {
  if ( $isid ) returnex $dcx( _xpopup, $1 $prop $2- )
  dcx xpopup $2 $1 $3-
}

alias xmenubar {
  if ($isid) returnex $dcx(_xmenubar, $prop $1-)
  dcx xmenubar $1-
}

alias mpopup {
  dcx mpopup $1 $2-
}

alias xdock {
  if ($isid) returnex $dcx( _xdock, $1 $prop $2- )
  dcx xdock $1-
}

alias xtray {
  if ($isid) returnex $dcx(TrayIcon, $1 $prop $2-)
  dcx TrayIcon $1-
}

alias xstatusbar {
  !if ($isid) returnex $dcx( _xstatusbar, mIRC $prop $1- )
  dcx xstatusbar $1-
}

alias xtreebar {
  !if ($isid) returnex $dcx( _xtreebar, mIRC $prop $1- )
  dcx xtreebar $1-
}

alias dcxml {
  !if ($isid) returnex $dcx( _dcxml, $prop $1- )
  dcx dcxml $1-
}

; $1 = hwnd needs .prop
alias xGetWindowProps {
  if (($0 != 1) || ($prop == $null)) { echo 4 -smlbfti2 [ERROR] xGetWindowProps: Invalid Args - $1- | halt }
  if ($isid) returnex $dcx(GetWindowProps,$1 $prop)
  !echo 4 -smlbfti2 [ERROR] xGetWindowProps: Must be used as an identifier.
  return
  :error
  !echo 4 -smlbfti2 [ERROR] xGetWindowProps: $error
}

; check the version of the loaded dcx.dll
; $1 = required version (3.1), $2 = required build (1067)
alias dcx_check_version {
  if ($dll(dcx.dll) == $null) return 0

  var %v = $file($dll(dcx.dll)).version
  if ($gettok(%v,1-2,46) < $1) return 0
  if ($gettok(%v,3,46) < $2) return 0

  ;var %v = $gettok($dcx(Version),4,32)
  ;if ($gettok(%v,1,45) < $1) return 0
  ;if ($gettok(%v,2,45) < $2) return 0
  return 1
}
; for use in dcx menus to embed styles & commands.
; 1 = set a tooltip for an item. (must be last style & after the item text)
; 11 = set an icon and/or style override for the item. (must be before the item text)
; 12 = set as radio item (can be anywhere in text)
; 14 = cause menu to stay open when item is selected, & callback alias is called
;   If callback doesnt exist or gives an invalid result then menu item is selected as normal & menu closes.
;   (can be anywhere)
; 16 = set a starting value for a progressbar or trackbar item.
; $1 = style id code, $2- any style args
alias xstyle {
  ; set tooltip (tooltip could be blank)
  if ($1 == 1) return $+($chr(1),$2-)
  ; set icon, and/or style override
  if ($1 == 11) {
    ; return if only the code is given
    if ($0 == 1) return
    ; $2 is an icon number
    ; $3 could be a style override
    var %t = $2-
    if ($2 isnum 0-) return $regsubex($str($chr(11),$calc($0 -1)),/(?=.)/g,$gettok(%t,\n,32))
    ; no icon number supplied. set icon to disabled.
    ; $2 could be a style override
    return $+(0,$chr(11),$regsubex($str($chr(11),$calc($0 -1)),/(?=.)/g,$gettok(%t,\n,32)))
  }
  ; set item as a radio style
  if ($1 == 12) return $chr(12)
  ; force menu to stay open & possibly set special command id (needed for mIRC's command menu as the id's change all the time)
  if (($1 == 14) && ($dcx_check_version(3.1,1132))) return $+($chr(14),$2)
  if (($1 == 16) && ($dcx_check_version(3.1,1257))) return $+($chr(16),$2)
}
;xdidtok dialog ID N C Item Text[[C]Item Text[C]Item Text]...
;xdidtok $dname 1 0 44 SomeText1,SomeText2
;$xdidtok($dname,1,0,44)
alias xdidtok {
  if (!$isid) {
    if ($0 < 5) { echo 4 -smlbfti2 [ERROR] /xdidtok Invalid args | halt }
  }
  elseif ($0 < 4) { echo 4 -smlbfti2 [ERROR] /xdidtok Invalid args | halt }
  if (!$dialog($1)) { echo 4 -smlbfti2 [ERROR] /xdidtok Invalid args | halt }
  if ($2 !isnum 1-) { echo 4 -smlbfti2 [ERROR] /xdidtok Invalid args | halt }
  if ($3 !isnum 0-) { echo 4 -smlbfti2 [ERROR] /xdidtok Invalid args | halt }
  if ($4 !isnum 1-) { echo 4 -smlbfti2 [ERROR] /xdidtok Invalid args | halt }
  var %type = $xdid($1,$2).type
  if (%type == list) {
    if (!$isid) xdid -A $1 $2 $3 +T $4 $5-
    else {
      ; warning buffer overflow possible!
      var %res, %c = $max(1,$3), %t = $xdid($1,$2).num
      while (%c <= %t) {
        ; cant use addtok as the items could be duplicates
        if (%res != $null) var %res = $+(%res,$chr($4),$xdid($1,$2,%c).text)
        else var %res = $xdid($1,$2,%c).text
        inc %c
      }
      return %res
    }
  }
  elseif (%type == listview) {
    if (!$isid) {
      ; no custom cmd atm
      ;xdid -a [NAME] [ID] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
      var %item = $3, %c = 1
      if (%item == 0) var %item = -1
      while ($gettok($5-,%c,$4) != $null) {
        xdid -a $1 $2 %item 0 + 0 0 0 0 -1 -1 $v1
        inc %c
        if (%item isnum 1-) inc %item
      }
    }
    else {
      ; returns first column only, no subitems
      ; warning buffer overflow possible!
      var %res, %c = $max(1,$3), %t = $xdid($1,$2).num
      while (%c <= %t) {
        ; cant use addtok as the items could be duplicates
        if (%res != $null) var %res = $+(%res,$chr($4),$xdid($1,$2,%c).text)
        else var %res = $xdid($1,$2,%c).text
        inc %c
      }
      return %res
    }
  }
  elseif (%type == comboex) {
    if (!$isid) xdid -a $1 $2 $3 +T 0 0 0 0 $4 $5-
    else {
      ; warning buffer overflow possible!
      var %res, %c = $max(1,$3), %t = $xdid($1,$2).num
      while (%c <= %t) {
        ; cant use addtok as the items could be duplicates
        if (%res != $null) var %res = $+(%res,$chr($4),$xdid($1,$2,%c).text)
        else var %res = $xdid($1,$2,%c).text
        inc %c
      }
      return %res
    }
  }
  elseif (%type == multicombo) {
  }
  elseif (%type == stacker) {
  }
  elseif (%type == treeview) {
  }
  else { echo 4 -smlbfti2 [ERROR] xdidtok: Invalid Control Type. | halt }
  !return
  :error
  !echo 4 -smlbfti2 [ERROR] xdidtok: $error
}
alias tab {
  var %i = 1, %tab
  while (%i <= $0) {
    if ($eval($+($,%i),2) != $null) {
      %tab = $instok(%tab,$eval($+($,%i),2),$calc($numtok(%tab,9) + 1),9)
    }
    inc %i
  }
  return %tab
}
; $1 = wid
; returns: pbstate pbpercent
; Used by custom xTreebar drawing.
alias _dcx_WIDtoPercent {
  if ($window(@ $+ $1).pbstate) return 1 $window(@ $+ $1).pbpercent
  if ($send(0)) {
    var %c = 1
    while ($send(%c).wid) {
      if ($v1 == $1) return 1 $send(%c).pc
      inc %c
    }
  }
  if ($get(0)) {
    var %c = 1
    while ($get(%c).wid) {
      if ($v1 == $1) return 1 $get(%c).pc
      inc %c
    }
  }
  return 0 0
}
on *:start: {
  unset %dcx_conflict
  if ($dll(dcx.dll) != $null) {
    if ($v1 != $scriptdirdcx\dcx.dll) {
      set %dcx_conflict 1
      echo 4 -s [DCX ERROR] Dll Conflict: Trying to load multiple copies of DCX: $+(04,$qt($v1),) and $+(03,$qt($v2),)
    }
  }
}
