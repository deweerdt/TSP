 /* bb_tools.i */
 %module bb
 %{
 /* Includes the header in the wrapper code */
 #include <bb_tools.h>
 %}
 
 /* Parse the header file to generate wrappers */
 %include "bb_tools.h"
