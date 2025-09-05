# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# html, body {
#   font-family: Verdana,sans-serif;
#   font-size: 10px;
# }

html_start = """
<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Performance Report</title>
    <script type="text/javascript" src="https://cdn.plot.ly/plotly-2.25.2.min.js" charset="utf-8" ></script>
  </head>

  <STYLE>
    .myTable {
      width: 100%;
      text-align: left;
      background-color: #EDEDED;
      border-collapse: collapse;
    }
    .myTable th {
      background-color: Silver;
      color: black;
      text-align: center;
    }
    .myTable td,
    .myTable th {
      padding: 5px;
      border: 1px solid #EDEDED;
    }

    .lighttheme {
      background-color : White;
    }
    .darktheme {
      background-color : #EDEDED;
    }

    /* Styling for the accordion */
    .accordion {
        color: #444;
        cursor: pointer;
        padding: 18px;
        width: 100%;
        border: none;
        text-align: left;
        outline: none;
        font-size: 16px;
        transition: 0.4s;
      }
      .active {
        background-color: Turquoise;
      }
      .inactive {
        background-color: Aquamarine;
      }
      .accordion:hover {
        background-color: Cyan;
      }
      .panel {
        padding: 0 18px;
        display: block;
        background-color: white;
        overflow: hidden;
      }

      /* Styling for the tool-tip hover-over */
      .tooltip-icon {
          position: absolute;
          right: 20px; /* Position the icon to the far right */
          cursor: pointer;
      }
      .tooltip-content {
          display: none; /* Initially hide the tooltip */
          position: absolute;
          right: 0; /* Align the tooltip to the right edge of the button */
          top: 100%; /* Position the tooltip below the icon */
          background-color: #333;
          color: #fff;
          padding: 10px;
          border-radius: 5px;
          min-width: 500px; /* Minimum width for the tooltip */
          max-width: 800px; /* Maximum width for the tooltip */
          box-shadow: 0 0 5px rgba(0, 0, 0, 0.3);
          z-index: 10;
          white-space: normal; /* Allow text to wrap */
          text-align: left; /* Align text to the left */
      }
      .tooltip-content a {
         color: #1E90FF; /* Light blue color for links */
         text-decoration: underline; /* Underline for better visibility */
         transition: color 0.3s; /* Smooth transition for color change */
      }
      .tooltip-content a:hover {
         color: #FFD700; /* Gold color on hover for high contrast */
      }
      .tooltip-icon:hover .tooltip-content {
          display: block; /* Show the tooltip on hover */
      }
  </STYLE>

  <body>
"""

html_end = """
  </body>
</html>
"""

script_end = """
      // Function to toggle the accordion on click
      function toggleAccordion() {
          var panel = this.nextElementSibling;
          if (panel.style.display == "none") { // Open it
              panel.style.display = "block";
              this.classList.remove("inactive");
              this.classList.add("active");
          } else { // Close it
              panel.style.display = "none";
              this.classList.remove("active");
              this.classList.add("inactive");
          }
      }

      // Add toggleAccordion for all accordion buttons
      const accordions = document.querySelectorAll(".accordion");
      for (let i = 0; i < accordions.length; i++) {
          var panel = accordions[i].nextElementSibling;
          if (i == 0) {
              accordions[i].classList.add("active");
              panel.style.display = "block";
          } else {
              accordions[i].classList.add("inactive");
              panel.style.display = "none";
          }
          accordions[i].addEventListener("click", toggleAccordion);
      }
"""

class HtmlPage:
    def __init__(self):
        self.html = html_start
        self.script = ""
        self.resize_divs = []
        self.div_id_ctr = 0

    def get_unique_div_id(self):
        self.div_id_ctr += 1
        return f"div{self.div_id_ctr}"

    def add_section(self, html, js, resize_divs):
        self.html += html
        self.script += js
        self.resize_divs += resize_divs

    def add_html(self, html):
        self.html += html

    def add_js(self, js):
        self.script += js

    def start_section(self, button_name, tooltip=""):
        self.add_accordion(button_name, tooltip)
        self.html += '<DIV class="panel">\n'

    def end_section(self):
        self.html += '</DIV>\n'

    def add_accordion(self, button_name, tooltip=""):
        tt = ""
        if tooltip:
            tt = ('<span class="tooltip-icon">ℹ️'
                  f'<div class="tooltip-content">{tooltip}</div></span>')
        self.html += f'<button class="accordion">{button_name}{tt}</button>'

    def add_resize_divs(self, divname):
        self.resize_divs.append(divname)

    def add_svg(self, svg_filename, height):
        self.html += f"""
        <object class="panel" data="{svg_filename}" type="image/svg+xml" width=1200 height={height}>
        <img src="fan.png" width=1200 height={height} />
        </object>"""

    def finalize(self):
        resize_code = "\n".join([f"Plotly.relayout({d},"
                                 "{width: window.innerWidth * 0.95});"
                                 for d in self.resize_divs])
        self.script += f"""
        window.onresize = function() {{
           {resize_code}
        }};"""

        self.script += script_end

        self.html += f"<script>\n{self.script}\n</script>"
        self.html += html_end
        return self.html


class HtmlKeyValueTable:
    def __init__(self, html_page):
        self.html_page = html_page
        self.cssclass = "lighttheme"
        self.html = """
        <DIV class="panel">
        <TABLE class="myTable">
          <TBODY>
        """

    def _switch_ccsclass(self):
        if self.cssclass == "darktheme":
            self.cssclass = "lighttheme"
        elif self.cssclass == "lighttheme":
            self.cssclass = "darktheme"
        else:
            assert 0

    @staticmethod
    def _inner_table(lst):
        html = "<TABLE><TBODY>"
        for row in lst:
            html += "<TR>"
            for c in row:
                html += f"<TD>{c}</TD>\n"
            html += "</TR>"
        html += "</TBODY></TABLE>\n"
        return html


    @staticmethod
    def _is_matrix(v):
        if not isinstance(v, list):
            return False
        if not all([isinstance(e, list) for e in v]):
            return False
        return True

    def add_row(self, key, value):
        self._switch_ccsclass()
        if self._is_matrix(value):
            v = self._inner_table(value)
        else:
            v = value
        self.html += f"""
            <tr class={self.cssclass}>
              <td>{key}</td>
              <td>{v}</td>
            </tr>
        """

    def finalize(self):
        self.html += "</TBODY></TABLE></DIV>\n"
        self.html_page.add_html(self.html)

class HtmlGlobalProbeTable:
    def __init__(self, html_page):
        self.html_page = html_page
        self.cssclass = "lighttheme"
        self.html = """
        <DIV class="panel">
        <TABLE class="myTable">
          <THEAD>
            <TR>
              <TH>Probe Name</TH>
              <TH>Display Name</TH>
              <TH>Value</th>
              <TH>Formatted Value</TH>
            </TR>
          </THEAD>
        <TBODY>
        """
    def _switch_ccsclass(self):
        if self.cssclass == "darktheme":
            self.cssclass = "lighttheme"
        elif self.cssclass == "lighttheme":
            self.cssclass = "darktheme"
        else:
            assert 0

    def add_global_probe_result(self, probe_name, display_name, value,
                                formatted_value):
        self._switch_ccsclass()
        self.html += f"""
            <tr class={self.cssclass}>
              <td>{probe_name}</td>
              <td>{display_name}</td>
              <td align="right">{value}</td>
              <td align="right">{formatted_value}</td>
            </tr>
        """

    def finalize(self):
        self.html += "</TBODY></TABLE></DIV>\n"
        self.html_page.add_html(self.html)

class HtmlObjectProbeTable:
    def __init__(self, html_page):
        self.html_page = html_page
        self.cssclass = "lighttheme"
        self.html = """
        <DIV class="panel">
        <TABLE class="myTable">
          <THEAD>
            <TR>
              <TH>Probe Name</TH>
              <TH>Display Name</TH>
              <TH>Object</TH>
              <TH>Value</th>
              <TH>Formatted Value</TH>
            </TR>
          </THEAD>
        <TBODY>
        """
    def _switch_ccsclass(self):
        if self.cssclass == "darktheme":
            self.cssclass = "lighttheme"
        elif self.cssclass == "lighttheme":
            self.cssclass = "darktheme"
        else:
            assert 0

    def add_object_probe_results(self, probe_name, display_name,
                                 object_value_list):
        self._switch_ccsclass()
        num = len(object_value_list)
        (o, v, f) = object_value_list[0]
        self.html += f"""
          <tr class={self.cssclass}>
            <td rowspan="{num}">{probe_name}</td>
            <td rowspan="{num}">{display_name}</td>
            <td>{o}</td>
            <td align="right">{v}</td>
            <td align="right">{f}</td>
          </tr>"""

        self.html += "".join([
            f"""<tr class={self.cssclass}>
            <td>{o}</td>
            <td align="right">{v}</td>
            <td align="right">{f}</td>
            </tr>""" for (o, v, f) in object_value_list[1:]])

    def finalize(self):
        self.html += "</TBODY></TABLE></DIV>\n"
        self.html_page.add_html(self.html)
