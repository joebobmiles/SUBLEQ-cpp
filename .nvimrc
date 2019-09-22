" Allow us to build our application(s) with a keystroke.
set makeprg=build
" Create errorformat strings for detecting clang error messages.
" (We consume ../ from the beginning of the filename due to how build.bat
" works.)
set errorformat+=%f(%l\\,%c):\ %trror:\ %m
set errorformat+=%f(%l\\,%c):\ %tarning:\ %m

noremap <F4> :make!<cr>:cwindow<cr>
" Allow us to run the tests with a keystroke.
noremap <F5> :!test<cr>
