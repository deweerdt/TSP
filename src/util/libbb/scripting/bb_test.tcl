
lappend auto_path "$env(DEVBASE)/exec/current/linux/debug/lib"
package require Tcl 8.3
package require bb

#namespace import bb::*

puts "Ok"
puts "Trying bbtools..."
set a [bb::bbtools_cmd]
puts "Trying bb::bb_dump..."
set b [bb::bbtools_dump]
