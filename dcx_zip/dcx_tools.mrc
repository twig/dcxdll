alias dcx {
  if ($isid) return $dll($scriptdirdcx\dcx.dll,$1,$2-)
  else dll " $+ $scriptdirdcx\dcx.dll" $1 $2-
}

alias udcx {
  $iif($menu, .timer 1 0) dll -u dcx.dll
}

alias xdid {
  if ($isid) {
    return $dcx(_xdid, $1 $2 $prop $3-)
  }
  else {
    dcx xdid $2 $3 $1 $4-
  }
}

alias xdialog {
  if ($isid) {
    return $dcx(_xdialog, $1 $prop $2-)
  }
  else {
    dcx xdialog $2 $1 $3-
  }
}

alias xpop {
  if ($isid) {
    return $dcx(_xpop, $1 $prop $2-)
  }
  else {
    dcx xpop $2 $1 $3-
  }
}

alias xpopup {
  if ($isid) {
    return $dcx(_xpopup, $1 $prop $2-)
  }
  else {
    dcx xpopup $2 $1 $3-
  }
}

alias xdock {
  if ($isid) {
    return $dcx(_xdock, $1 $prop $2-)
  }
  else {
    dcx xdock $1-
  }
}

alias mpopup {
  dcx mpopup $1 $2
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

alias shell {
  return $scriptdirshell.dll
}
