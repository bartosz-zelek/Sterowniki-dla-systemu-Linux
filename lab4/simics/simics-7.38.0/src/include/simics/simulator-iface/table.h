/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_TABLE_H
#define SIMICS_SIMULATOR_IFACE_TABLE_H

#include <simics/pywrap.h>
#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="table_key_t def"></add-type> */       
typedef enum {
        /* Table property keys */
        Table_Key_Name = 1,
        Table_Key_Description,
        Table_Key_Default_Sort_Column,
        Table_Key_Columns,
        Table_Key_Extra_Headers,
        Table_Key_Stream_Header_Repeat,
        /* Additional properties might be added in the future.
           Thus, unknown values should be silently ignored. */
} table_key_t;

/* <add-type id="column_key_t def"></add-type> */
typedef enum {                
        /* Column property keys */
        Column_Key_Name = 1000,       /* Other number series than table-keys */
        Column_Key_Description,
        Column_Key_Alignment,
        Column_Key_Int_Radix,
        Column_Key_Float_Percent,
        Column_Key_Float_Decimals,
        Column_Key_Sort_Descending,
        Column_Key_Hide_Homogeneous,
        Column_Key_Generate_Percent_Column,
        Column_Key_Generate_Acc_Percent_Column,
        Column_Key_Footer_Sum,
        Column_Key_Footer_Mean,
        Column_Key_Int_Grouping,
        Column_Key_Int_Pad_Width,
        Column_Key_Metric_Prefix,
        Column_Key_Binary_Prefix,
        Column_Key_Time_Format,
        Column_Key_Unique_Id,
        Column_Key_Width,
        Column_Key_Word_Delimiters,
        /* Additional properties might be added in the future.
           Thus, unknown values should be silently ignored. */        
} column_key_t;

/* <add-type id="extra_header_key_t def"></add-type> */
typedef enum {
        /* Header property keys */
        Extra_Header_Key_Row = 2000,
        Extra_Header_Key_Name,
        Extra_Header_Key_Description,
        Extra_Header_Key_First_Column,
        Extra_Header_Key_Last_Column,
} extra_header_key_t;

/* <add id="table_interface_t"> 

   The table interface can be implemented by objects holding data
   which can be presented in a table by the user interface (through
   Simics CLI command outputs, Eclipse etc.) 

   By properly implementing the table interface, less object specific code
   is needed to provide the data to the user. Generic Simics commands
   (associated with the table interface) allows the data to sorted on 
   the desired column and printed in a uniform way. 
   Similarly, the data can be exported to a comma separated value (.csv)
   file, making it possible to process the result in a spreadsheet program.

   The <fun>data()</fun> method returns the data as an
   <type>attr_value_t</type> array where the outer list contains each row and
   the inner list contains the columns for that row. The number of columns in
   each row must be exactly the same. The data only holds the actual data, not
   the headers for each column, which instead is specified in the
   <fun>properties()</fun> method. The data can be unsorted, it is up to the
   user of the interface to sort it in a suitable way.

   The data itself is sufficient for producing a table, but to customize
   the table layout the <fun>properties</fun> method can return hints on
   how the table should be presented more in detail. For example, specifying
   which column the table should be sorted on by default, or if a column 
   preferably should  be printed with hexadecimal numbers instead of decimals.
   In addition, the <fun>properties</fun> also contains the name of the columns
   as well as additional meta-data such as what each column represents, or 
   what the entire table represents.

   The <fun>properties</fun> also returns an <type>attr_value_t</type> type 
   as a list of key/value pairs. Thus, each list-entry should be exactly
   two elements long, but the value part can itself be a new list of key/value
   pairs for some keys.

   The table property keys are of <type>table_key_t</type> type:
   <insert id="table_key_t def"/>

   This is the definition for each table property:
   <dl>
   <dt><tt>Table_Key_Name</tt> <i>String</i></dt>
   <dd>A short description of what the table represents.</dd> 

   <dt><tt>Table_Key_Description</tt> <i>String</i></dt> 
   <dd>A longer description what the table represents.</dd>

   <dt><tt>Table_Key_Default_Sort_Column</tt> <i>String</i></dt> 
   <dd>References the Column_Key_Name (inside the Table_Key_Columns) to tell
   which column that should be used when sorting the table by
   default. If the Column_Key_Name contains new-line characters,
   replace those with spaces. If not specified, the table will
   be unsorted by default.</dd>

   <dt><tt>Table_Key_Extra_Headers</tt> <i>List of additional header
   rows</i></dt>

   <dd>In addition to the column-headers printed, this property allows
   additional header rows to be presented before the column headers. These
   additional header rows can span over multiple columns, providing additional
   explanation and grouping of columns.

   The format is: list of additional header rows, each identified as a
   <tt>Extra_Header_Key_Row</tt>.
   Each row is defined by a list of the header-elements where each element
   is a list of the header's key/value pair properties.

   The header property keys are of <type>extra_header_key_t</type> type,
   this is the definition for each extra header property:
   <insert id="extra_header_key_t def"/>

   <dl>
   <dt><tt>Extra_Header_Key_Row</tt> <i>List of header elements</i></dt>
   <dd>Identifies a new header row.</dd>

   <dt><tt>Extra_Header_Key_Name</tt> <i>String</i></dt>
   <dd>The name printed for the extra header element.</dd>

   <dt><tt>Extra_Header_Key_Description</tt> <i>String</i></dt>
   <dd>Optional additional text describing the header.</dd>

   <dt><tt>Extra_Header_Key_First_Column</tt> <i>String</i></dt>
   <dd>A reference to the column the additional header spans from.</dd>

   <dt><tt>Extra_Header_Key_Last_Column</tt> <i>String</i></dt>
   <dd>A reference to the column the additional header spans to.</dd>
   </dl>
   </dd>

   If there is only one header element on a row, and neither
   <tt>Extra_Header_Key_First_Column</tt> and
   <tt>Extra_Header_Key_Last_Column</tt> are set, the header will span the
   entire table, even if additional columns has been added by the system. In
   all other cases first/last column always needs to be specified.

   <dt><tt>Table_Key_Columns</tt> <i>List of columns, with key/value pairs
   </i></dt>
   <dd>A list of the columns, where each element consists of key/value pairs
   defining each column with various properties.
   The column properties are named with Column_Key_* which are listed below.
   The list size should match the number of columns in the data. List item 1,
   represents key/value definitions for column 1 etc.

   The column property keys are of <type>column_key_t</type> type:
   <insert id="column_key_t def"/>
   This is the definition for each column property:

   <dl>
   <dt><tt>Column_Key_Name</tt> <i>String</i></dt> 
   <dd>The name for the column, displayed on the first row of the table.
   Preferably these should be as short as possible while still being 
   descriptive. 
   It is possible to break longer strings with a newline character (\n)</dd>

   <dt><tt>Column_Key_Description</tt> <i>String</i></dt> 
   <dd>A longer descriptive text describing the column content.</dd>

   <dt><tt>Column_Key_Alignment</tt> <i>String: ["left", "right", "center"]</i>
   </dt> 
   <dd>Specifies if the column data should be aligned to the left, right
   or centered for the entire table data. If not specified strings are
   left-aligned and numbers right-aligned.</dd>

   <dt><tt>Column_Key_Word_Delimiters</tt> <i>String</i></dt>
   <dd>Overrides the default set of character that will be used for word
   wrapping of long lines. Default is <tt>" -_:,."</tt>.</dd>

   <dt><tt>Column_Key_Int_Radix</tt> <i>Integer: [2,10,16]]</i></dt> 
   <dd>Specifies the default radix which should be used for when displaying
   integers numbers, 2 means binary, 10 decimal and 16 hexadecimal.
   If not specified, the integers will be displayed in the default radix,
   selectable by the <cmd>output-radix</cmd> command.</dd>

   <dt><tt>Column_Key_Int_Grouping</tt> <i>Boolean</i></dt>
   <dd>If False, the current user preferences for integer grouping will be
   ignored. For example, instead of 12_345 the output will read 12345. If not
   specified, default grouping is respected. Grouping is set by the
   <cmd>output-radix</cmd> command.</dd>

   <dt><tt>Column_Key_Pad_Width</tt> <i>Integer</i></dt>
   <dd>Zero-extends values up to N characters. This allows similar width of
   the column data regardless of the value in each cell. For example,
   with hexadecimal output and 64-bits value printed, the column might
   be easier to read with a pad-width of 16.
   If not specified, the default size used 1 (no-padding).</dd>

   <dt><tt>Column_Key_Float_Percent</tt> <i>Boolean</i></dt> 
   <dd>If True, any float data in the column will be printed as a 
   percent value. For example, 0.1234 will be printed as 12.34%.</dd>

   <dt><tt>Column_Key_Float_Decimals</tt> <i>Integer</i></dt> 
   <dd>Specifies how many decimal digit that should be printed for
   any floating point values in the column. If not specified 2 decimals
   are printed by default.</dd>

   <dt><tt>Column_Key_Metric_Prefix</tt> <i>String</i></dt> <dd>A metric prefix
   (m, µ, n, p, f, a, z, y, k, M, G, T, P, E, Z, Y) will be added to the value
   and the value will be adjusted accordingly. If the string is non-empty the
   string will be interpreted as a unit that will be added after the prefix for
   each value.</dd>

   <dt><tt>Column_Key_Binary_Prefix</tt> <i>String</i></dt> <dd>A binary prefix
   (ki, Mi, Gi, Ti, Pi, Ei, Zi, Yi) will be added to the value
   and the value will be adjusted accordingly. If the string is non-empty the
   string will be interpreted as a unit that will be added after the prefix for
   each value.</dd>

   <dt><tt>Column_Key_Time_Format</tt> <i>Boolean</i></dt> <dd>If True
   the value (in seconds) will be formatted as <tt>HH:MM:SS.ss</tt>,
   where only the used parts are shown.
   The number of seconds decimals formatted is can be controlled by
   <tt>Column_Key_Float_Decimals</tt> property.
   To compact the width, decimals are dropped when hours are displayed.
   </dd>

   <dt><tt>Column_Key_Unique_Id</tt> <i>String</i></dt>
   <dd>If the same name is set for multiple columns in a table, this
   property sets a unique name for a specific column, needed when
   referencing the column from extra headers.</dd>

   <dt><tt>Column_Key_Sort_Descending</tt> <i>Boolean</i></dt> 
   <dd>If True, specifies if a column should be sorted with the 
   highest values first. If not specified, this is automatically
   set to True for any numbers and False for other types.</dd>

   <dt><tt>Column_Key_Hide_Homogeneous</tt> <i>Integer/Float/String</i></dt>
   <dd>If this property is specified and the column consists entirely of the
   given data, the column will not be shown. For example, if the table has a
   column which only consists of empty strings (""), the column can be
   discarded from the resulting table.</dd>

   <dt><tt>Column_Key_Footer_Sum</tt> <i>Boolean</i></dt>
   <dd>If True, all columns values are summed up and displayed in
   a footer row, below the actual table.</dd>

   <dt><tt>Column_Key_Footer_Mean</tt> <i>Boolean</i></dt>
   <dd>If True, an arithmetic mean of the column values are calculated
   and displayed in a footer row, below the actual table.</dd>

   <dt><tt>Column_Key_Generate_Percent_Column</tt>
   <i>List of key/value pairs for new column.</i></dt> 
   <dd>If this property is set, an additional column will be created 
   (to the right of this column) showing the percent values for each row.
   The list of key/value pairs allows to the created column to be
   customized, however just giving an empty list should give 
   understandable default values.
   </dd>

   <dt><tt>Column_Key_Generate_Acc_Percent_Column</tt>
   <i>List of key/value pairs for new column.</i></dt> 
   <dd>If this property is set, an additional column will be created 
   (to the right of this column) showing the accumulated percent values for
   each row.
   The list of key/value pairs allows to the created column to be
   customized, however just giving an empty list should give 
   understandable default values.
   </dd>
   </dl>

   </dd>
   </dl>

   
   <insert-until text="// ADD INTERFACE table"/>

   </add>
   <add id="table_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(table) {
        /* Returns all rows and columns in the following format:
           [[[i|f|s|o|n*]*]] where the outer list is the row
           and the inner list is the data for each column. */
        attr_value_t (*data)(conf_object_t *obj);
        
        /* Defines the table structure and meta-data for the table
           using a list of key/value pairs.
           [[[ia]*]*] where the integer is the key taken from the
           table_properties_t. The value is key-specific. */
        attr_value_t (*properties)(conf_object_t *obj);

};
#define TABLE_INTERFACE "table"
// ADD INTERFACE table

#if defined(__cplusplus)
}
#endif

#endif
