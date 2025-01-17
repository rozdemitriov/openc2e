" Vim syntax file
" Language:     CAOS
" Maintainer:   Francis Irving (francis.irving@creaturelabs.com) 

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

if !exists("main_syntax")
  let main_syntax = 'caos'
endif

set iskeyword=@,48-57,_,192-255,:,<,=,>,#,+

" Comments
syn keyword caosTodo contained TODO
syn region caosComment start="*" end="$" contains=caosTodo

" TODO: Make this understand exactly the characters CAOS knows about
syn match caosCharacter "'[^\\]'"
syn match caosCharacter "'\\.'"
syn match caosCharacterInString "\\." contained
syn region caosString start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=caosCharInString

" syn match caosByteArray "\[[0-9, ].\]"
syn match caosByteArray "\[[0-9 ]*\]"

" Parentheses
" TODO: brace matching?
syn match caosParen '('
syn match caosParen ')'

" Integer and floating point numbers, with dot in various places
syn match caosNumber "\<\d\+\>"
syn match caosNumber "\<\d\+\.\d*\>"
syn match caosNumber "\.\d\+\>"
syn match caosNumber "-\<\d\+\>"
syn match caosNumber "-\<\d\+\.\d*\>"
syn match caosNumber "-\.\d\+\>"

" Conditions
syn keyword caosConditional eq ne gt lt ge le and or = <> <= >= < >

" Meta
syn keyword caosMeta scrp endm rscr iscr

" Read in autogenerated stuff
so <sfile>:h/caos.auto.vim

if !exists("did_caos_syntax_inits")
  let did_caos_syntax_inits = 1

  " The default methods for highlighting.  Can be overridden later
  hi link caosComment            Comment
  hi link caosString		 String
  hi link caosCharacter		 Character
  hi link caosCharacterInString	 SpecialChar
  hi link caosNumber		 Number
  hi link caosCommand		 Statement
  hi link caosStringRV		 Identifier
  hi link caosIntegerRV		 Identifier
  hi link caosFloatRV		 Identifier
  hi link caosAgent		 Identifier
  hi link caosVariable		 Type
  hi link caosTodo		 Todo
  hi link caosError		 Error
  hi link caosParen		 Conditional
  hi link caosConditional	 Conditional
  hi link caosMeta		 PreProc
  hi link caosByteArray		 Constant
  	
  " Note: This uses nc which is netcat - you'll need to install it
  " TODO: Add easy shortcuts for MANN and APRO, looking at current word
  " TODO: Use ~/.creaturesengine/port for the injection port, rather than 20001
  function DoInjectTempfile(tempfile)
 	let a=system("nc localhost 20001 <\"" . a:tempfile . "\"")
	execute ":bdelete! " . a:tempfile
	if v:shell_error
		return "Failed to connect to game"
	endif
	return a
  endfunction
  function Inject()
  	let tempfile = tempname()
	execute ":w " . tempfile
	let a=system("echo -e \"\\nrscr\\n\" \>\>" . tempfile)
	return DoInjectTempfile(tempfile)
  endfunction
  function UnInject()
	let tempfile = tempname()
	execute "\:1/rscr/+1,$w " . tempfile
	let a=system("echo -e \"\\nrscr\\n\" \>\>" . tempfile)
	return DoInjectTempfile(tempfile)
  endfunction
  function ChooseInject()
	let tempfile = tempname()
	let b = input("CAOS command: ")
	echo "\n"
	" TODO: Work out how to do this better in vim
	" (make a new buffer, and append() to that?)
	let b = substitute(b, "\\", "\\\\\\\\", "g")
	let b = substitute(b, "\"", "\\\\\\\"", "g")
	let a=system("echo " . b . " \>\>" . tempfile)
	let a=system("echo -e \"\\nrscr\\n\" \>\>" . tempfile)
	execute ":badd " . tempfile
	return DoInjectTempfile(tempfile)
  endfunction
  map <F11> : echo Inject()<CR>
  map <F12> : echo UnInject()<CR>
  map <F10> : echo ChooseInject()<CR>
endif

let b:current_syntax = "caos"

if main_syntax == 'caos'
  unlet main_syntax
endif

" vim: ts=8
