<!---
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
-->

<div class=note>

## Technology Preview

This is a technology preview of the new, documentation system for Simics. It can be used to generate HTML documentation for Simics from Markdown. Everything here is subject to change without notice.
</div>

# Writing Simics Documentation

The Simics documentation tool takes Markdown files and images in a directory and a table of contents file and produces documents in html.

To write a document you write a `toc.json` file to specify the structure of the document and a series of Markdown file containing the actual content of the document.

This Simics module build system uses a simple convention to find documents to build.
It looks for documents in the `doc-src` directory in your Simics project, and allows you to build any documents it finds there.
For example, if you have a document at `doc-src/my-document` in your Simics project, then the build system will find it and you can build it by running `make doc/my-document`.
You can configure your project to look in different directories.
We will describe this later in this document.

The tool accepts multiple *input directories* for a single document.
When the tool looks for a file it looks across all its input directories and takes the first file with the given name it finds.
For the build system integration this is used to share common input files, like the CSS style-sheet, across documents.
It adds both the document directory and the common directory as input directories.

## An Example
Let's look at building this very document you are reading as an example.
We assume you have set up a Simics project associated with a Simics installation that includes the documentation package.
This example also assumes you run things on a Linux host.
The tool works just as well on Windows, but you have to adapt to the `win64` host directory.
The examples also assumes a Unix shell, and you may need to use other commands on Windows.
All the examples here also assume that you have your simics-project as the current working directory in your shell.

First you need to copy the source of the document from the documentation package to your project:
```
mkdir -p doc-src
cp -a <doc-package>/src/docs/simics-manuals/doc-guide doc-src
```
Then you can build your local copy of the document:
```
make doc/doc-guide
```

This will place the built document in `linux64/doc/html/doc-guide`.
The main entry-point to the document is in `linux64/dod/html/doc-guide/index.html`.
You can open it in your browser to read the resulting document.

## Documentation Make Targets
You don't have to write any make files to use the documentation system.
The Simics build system will find the documents automatically and provide relevant make targets.
The build system integration also selects the Simics CSS style sheet for you.

Each document you can build will have a make target of the form `doc/<doc>`.
By convention, the build system will look in `doc-src` for documents.
Each sub-directory of `doc-src` containing a `toc.json` file is considered a document.
The build system defines one build rule for each such document.
It will only look for documents that are direct sub-directories of `doc-src`.
It won't try to find documents at arbitrary depths.

There is also a `doc/all` rule to build all documents in your project, and a `doc/list` rule to list the build targets for all individual documents in the project.

In your project you can change which directory to search for documents in by setting `DOC_SRC_DIRS` in your `config-user.mk`.
It should be set to a space-separated list of directories to look for documents in.
As above, a document is any directory containing a `toc.json` file.

## Running The Tool Directly
The tool can be run directly.
This could be used to integrate in other build systems.
When you run the tool directly you don't have to follow the conventions set out by the integration into the Simics build system, but you also have to specify more things manually.
Knowing how the tool works and what its inputs are is also useful to better understand `toc.json`.

The tool is available at `<doc-package>/linux64/bin/dodoc`.
It takes a list of directories as input, and an output directory to put the resulting document in.
There are also some additional flags to specify which css-file to use etc.
To get the full list of arguments run `<doc-package>/linux64/bin/dodoc --help`.

You specify input directories one after another.
To share chapters between documents or common resources like images you can specify more than one input directory.
The tool requires that all the directories exist.
When looking for files it will take the first instance of the file it finds.

## toc.json
This file defines your document and its structure.
It is a JSON file declaring the table of contents of the document with references to the source files that provide the text for each section of the document.
The document is structured as a tree of sections.
Each section is a JSON object with keys to specify the source file of the section, children of the section, etc.

The top-level section has some additional fields you can specify.

### Section Objects
Each section object in `toc.json` can have the following keys:

- **`file`:** A string value that points to the Markdown file which contains the source for the section's text.
    The tool will look for this Markdown (`.md`) in all input directories.
    The tool will convert this input file to HTML and place a corresponding file in the output directory with an `.html` extension.
- **`name`:** The name of the section.
    If this is not supplied it will be taken from the top-level heading in the source text instead.
    If it is supplied the source text should not contain any top-level heading and the tool will insert a heading in the output file for you.
- **`children`:** A list of child sections.
    Each child is its own section object.
- **`numbering`:** A boolean which tells the tool if this section should be numbered or not.
    If it is numbered then the tool will give it a section number.
- **`appendix`:** Tells the tool that this section is an appendix.
    This is only valid for the children of the top-level section or the chapters in a book.
    The first section with this key set to `true` and all following sections will be considered appendicies.
    This means they get a section letter instead of a section number.

### The Root Section
The top-level of the `toc.json` file is a JSON object and defines the root section of the table of contents.
The root section can have some extra keys to control the tool.

- **`anchor`:** A string which says where in the master table of contents this document should be placed.
    Defaults to "users_guides".
    The master table of contents recognizes the anchors 
    - "users_guides",
    - "target_guides",
    - "programming_guides",
    - "technology_guides",
    - "application_notes",
    - "reference_manuals",
    - "release_notes",
    - "getting_started",
    - "installation_guides", and
    - "master-toc".
- **`is_book`:** A boolean which says if this is a book or not.
    A book has parts as the top-level children, with chapters as the level below that.
    Parts have roman numerals.
    The chapters are enumerated in one big sequence across the entire book.
    The chapter count is not reset for each part.
- **`resource_directories`:** An array of strings.
    Extra input directories for the tool.
    Each element is the path to a directory relative to the directory where the `toc.json` file is found.
    The resource directories are searched after the input directories specified on the command line.

The purposes of the various anchors are as follows:

|Anchor              | Purpose
|:-------------------|:--------
|users_guides        | User's Guides guides users through how to use a part of the product.
|target_guides       | Target Guides describe a target platform, its target scripts, etc.
|programming_guides  | For specialized APIs such as the Link Library or the C++ Device API.
|technology_guides   | Cover the simulation of specific technologies.
|application_notes   | Tend to deal with very specific subjects, such as Understanding Simics Timing.
|reference_manuals   | Reference documentation for Simics and its packages.
|release_notes       | What has changed between releases of a particular package.
|getting_started     | Helps a user get started with Simics itself.
|installation_guides | Describes how to install Simics itself.
|master-toc          | Don't use. A description of the Simics documentation for users.

You can read a fuller description for the categories of documents in the *Documentation Contents* document.
## Markdown files
You write the actual text of the document in Markdown in the [CommonMark] flavour.
There are a couple of simple extensions and conventions used to write figures and references to figures and sections.
The tool also expects a single top-level heading in the document, or no top-level heading if you provide a section title in `toc.json`.

[CommonMark]: https://commonmark.org/

The tool can number sections, subheadings, and figures for you, and allows you to link to them without having to write the number.
This is mostly intended for back-wards compatibility with our old Simics documentation conventions.

To specify a figure the tool expects the convention that you wrap the figure in a `<figure>` tag and an id.
You can also add a figure caption by writing a `<figcaption>` inside the `<figure>` tag.
The caption will be decorated with the figure number by the tool.
See figure [x](#figure-example) for an example.
You don't have to use an actual image for a figure.

You don't have to use figures, and can include images directly instead.

<figure id="figure-example">

```
<figure id="example-figure">

Place your figure here. Possibly an image.
Markdown is picky, so you need one or more empty lines after the <figure>-tag.
<figcaption>An example</figcaption>
</figure>
```
<figcaption>How to write a figure</figcaption>
</figure>

To link to sections, pages and other documents use normal Markdown links.

Each heading gets an anchor you can link to based on its name.
The anchor is the lower cased text of the header, with non-alphanumeric character spans replaced with `-`.
If there are multiple sections with the same name on one page a sequence number is used to give each section an unique anchor.
For example, to link to [this section](#markdown-files) write `[this section](#markdown-files)`.

You can also link between documents by using relative URLs containing `..`.
For example, to link to [`SIM_create_object`](../reference-manual-api/simulator-api-functions.html#SIM_create_object) write ``[`SIM_create_object`](../reference-manual-api/simulator-api-functions.html#SIM_create_object)``.

To refer to a numbered section, subheading, or figure you use a normal Markdown link, with a single `x` as placeholder text.
The recommendation is to not use this for new documents.
It is better to write the link text explicitly.

The tool will replace the placeholder text of such links with the number of the section or figure they link to.
The replacement is only the number.
You have to write "Section", "Figure", etc yourself.
If the tool does not find the section or figure you try to link to, the placeholder will not be replaced.

<figure id="reference-example">

```
See figure [x](#example-figure).
```
<figcaption>Referring to the figure</figcaption>
</figure>

## Documentation Tests
The documentation tool supports extracting test sessions from documentation.
This allows you to write documentation that shows how you interact with Simics and test that what is in the documentation matches what the product does.
This is limited to interactions with the command line interface, and text consoles. 

The mechanism is based on fenced code blocks with the `simics` language.
In these code blocks you can use special annotations to drive the tests and highlight user input on the Simics prompt.
Write `simics test <session-name> [<optional-part>] [collect]` to make the block part of a test, or use just `simics` to only take advantage of the highlighting of commands.
If you end the session definition with the word `collect` then the session will be flagged to run in failure collection mode (which needs to be supported by whatever test framework ultimately consumes the test generated description). If a session consists of multiple parts, marking one part with `collect` will mark the entire session.

Here is a short example:
````
```simics test a-test-session
> run-command-file "%simics%/targets/qsp-x86/firststeps.simics"
|console board.serconsole.con
|run-to (# )
```
````

Which results in the following in the document
```simics test a-test-session
> run-command-file "%simics%/targets/qsp-x86/firststeps.simics"
|console board.serconsole.con
|run-to (# )
```

When you run `dodoc` with the `--extract-tests` optional argument the tool will extract a `.json` file with the test sessions found in the document.
```
<doc-package>/linux64/bin/dodoc <document-directories...> --extract-tests <test-sessions-json-file>
```

```json
[
  {
    "name": "a-test-session",
    "steps": [
      {
        "Command": "run-command-file %simics%/targets/qsp-x86/firststeps.simics"
      },
      {
        "Console": "board.serconsole.con"
      },
      {
        "RunTo": "# "
      }
    ],
    "collect": false
  }
]
```

This can then be used to drive tests, but `dodoc` itself does not include a framework to run such tests.

Here is the full list of commands you can use in `test-simics` blocks:

| Command    | Intended semantics                  | Include in documentation output |
|------------|-------------------------------------|---------------------------------|
| `>`        | run a simics command                | yes, prefixed with `simics>`    |
| `\|>`      | run a simics command                | no                              |
| `<`        | check output from simics            | yes                             |
| `\|<`      | check output from simics            | no                              |
| `input`    | input text on target console        | yes                             |
| `input-prompt` | input text on target console, without the printed prompt | yes    |
| `\|input`  | input text on target console        | no                              |
| `run-to`   | wait for output from target console | yes                             |
| `\|run-to` | wait for output from target console | no                              |
| `\|console`| declare which console to use for `input` and `run-to` | no            |
| `!`        | skipped                             | yes                             |
| `!>`       | skipped                             | yes, prefixed with `simics>`    |
| `prompt:`  | skipped                             | yes                             |

All other lines will be included in the manual as is, and skipped for testing.
All commands are followed by text.
This text is what is included in the manual, if the line is included.

`prompt` and `input-prompt` are used to format user input that includes a printed shell prompt.
For `input-prompt` the prompt part of the line is stripped by `dodoc` in the test extraction.
For example, the following code in a `simics` code block:

```shell
input-prompt: user@host:~$ ls -l
```

is output to the documentation as:

```simics
input-prompt: user@host:~$ ls -l
```

while the extracted text to be input on the target console is only `ls -l`.
The prompt expression matches several common CLI prompts on both Linux shells and Windows PowerShell.

The text after a command can be surrounded by parenthesis.
The parenthesis will not be included in the line, neither for testing or output.
To allow parenthesis in the text, the initial parenthesis you can include any number of `:` characters.
This will match with the same number of `:` after the final parenthesis.

A test session can span several blocks in the same markdown file.
This allows you to mix descriptions and interactions with Simics freely.

Blocks that are empty after filtering for lines to include in the manual will be removed entirely from the output.

Note that `<` (and `|<`) commands can only check output that is *printed* to the Simics command line output. They cannot check strings or other objects that are *returned* from function calls. A workaround is to use an `echo` statement like this:

```text
!> get-auto-mac-address
"00:17:a0:00:00:00"
|> echo (get-auto-mac-address)
|< 00:17:a0:00:00:00
```

This will only show the top two lines in the output, but will only use the bottom two lines in the test. Use this method sparingly, as it carries the risk of the printed commands diverging from the tested commands if the two versions are not updated in tandem.

### Multi-line text

You can also use multi-line text after a command. Use a `{` after the command.
Again, you can prefix it with `:`-characters.
The following lines, up to a line starting with `}` and the same number as `:`s a at the start will be used as the text for the command. This can simplify *check output* commands. However, for *run* commands (`>`), this is only useful to split a long line that makes up a single Simics command into several lines for readability. It is not possible to use this method to enter multiple lines of Python code. For that use a workaround such as placing the code in a separate file and *including* it (see the [next section](#including-text-from-source-files) for the *include* syntax):

````text
```simics test include-test
|> run-script "%simics%/src/docs/simics-manuals/doc-guide/example.py"
!> python-mode
```

{{include example.py#code}}

```simics test include-test
.........
simics>>> (Ctrl-D)
```
````

which will render like this:

```simics test include-test
|> run-script "%simics%/src/docs/simics-manuals/doc-guide/example.py"
!> python-mode
```

{{include example.py#code}}

```simics test include-test
.........
simics>>> (Ctrl-D)
```

Alternatively, separate the printed command from the run command and write the run command as one long line. Due to the risk of the two commands getting out of sync in future document changes, this method is discouraged for anything but very short code segments:

```text
simics> @def callback(user_data, clock, packet, crc_status):
.......     print(f'callback was called with packet: {packet}')
|> python "def callback(user_data, clock, packet, crc_status):\n  print(f'callback was called with packet: {packet}')"
```

## Including text from source files

You can use the special annotation `{{include file#id}}` to include text from source files.
This includes the snippet with the given *id* from the specified *file*.

In the source files you use special comments to create snippets.
There are two kinds of snippets:
- code snippets, which are inserted as code blocks, and
- markdown snippets, which are inserted as additional markdown text.

To specify a code snippet surround it with line comments specifying the start and end of the snippet.
Different languages use different syntax for line comments. 
Python uses the `#` character.
There you specify the start of the snippet with `#:: pre <id> {{` and the end with `# }}`.
This creates a snippet with the id *&lt;id>*.

To specify a markdown snippet you instead start the snippet with `#:: doc <id> {{`.
The markdown will be extracted from the line comments between the start and end of the snippet, with the single `#` and the first space after the `#` stripped.

For example:
```
{{include file.py#some-code}}
{{include file.py#some-text}}
```
and
```python
#:: pre some-code {{
def foo():
    print("hello")
# }}

#:: doc some-text {{
# ## Hello from Python
# Yes, that is all I have to say right now.
# }}
```

In C-like languages, C++ style line comments can be used, starting with `//::` and ending with `// }}`.
The starting `//` and space will be stripped from intermediate lines.

Block comments are also supported.
The snippet starts with `/*:: doc <id>` *without* `{{` and ends when the block comment ends.
Intermediate lines are used as is.
Leading asterisks in comments such as this are *not* stripped:

```c
/*:: doc long-comment
 * This is a long comment
 * that consists of multiple lines
 */
```
