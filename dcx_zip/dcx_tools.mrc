alias dcx {
  if (%dcx_conflict) { echo 4 -s [DCX ERROR] Dll Conflict, Trying to load multiple copies of DCX: $+(06,$qt($dll(dcx.dll)),) and $+(03,$qt($scriptdirdcx\dcx.dll),) | halt }
  if ($isid) returnex $dll($scriptdirdcx\dcx.dll,$1,$2-)
  else dll " $+ $scriptdirdcx\dcx.dll" $1 $2-
}

alias udcx {
  if ($dcx(IsUnloadSafe)) $iif($menu, .timer 1 0) dll -u dcx.dll
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
  dcx mpopup $1 $2
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

alias dcxml dcx dcxml $1-

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
on *:start: {
  unset %dcx_conflict
  if ($dll(dcx.dll) != $null) {
    if ($v1 != $scriptdirdcx\dcx.dll) {
      set %dcx_conflict 1
      echo 4 -s [DCX ERROR] Dll Conflict: Trying to load multiple copies of DCX: $+(04,$qt($v1),) and $+(03,$qt($scriptdirdcx\dcx.dll),)
    }
  }
}
