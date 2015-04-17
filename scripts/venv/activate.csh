# This file must be used with "source bin/activate.csh" *from csh*.
# You cannot run it directly.
# Created by Davide Di Blasi <davidedb@gmail.com>.
# Ported to Python 3.3 venv by Andrew Svetlov <andrew.svetlov@gmail.com>

alias deactivate 'test $?_OLD_VIRTUAL_PATH != 0 && setenv PATH "$_OLD_VIRTUAL_PATH" && unset _OLD_VIRTUAL_PATH; rehash; test $?_OLD_VIRTUAL_PROMPT != 0 && set prompt="$_OLD_VIRTUAL_PROMPT" && unset _OLD_VIRTUAL_PROMPT; unsetenv VIRTUAL_ENV; test "\!:*" != "nondestructive" && unalias deactivate'

# Unset irrelavent variables.
deactivate nondestructive

setenv VIRTUAL_ENV "~/Devel/nix-env"
setenv PROJECT_ROOT "~/Devel/nix-env/nix"
setenv LD_LIBRARY_PATH "/usr/local/lib64/soci:/usr/local/lib64/yami4:$VIRTUAL_ENV/nix/lib/external/jsoncpp"


set _OLD_VIRTUAL_PATH="$PATH"
setenv PATH "$VIRTUAL_ENV/bin:$PATH"
setenv PYTHONPATH "$VIRTUAL_ENV/nix/python"


set _OLD_VIRTUAL_PROMPT="$prompt"

if (! "$?VIRTUAL_ENV_DISABLE_PROMPT") then
    if ("nix" != "") then
        set env_name = "nix"
    else
        if (`basename "VIRTUAL_ENV"` == "__") then
            # special case for Aspen magic directories
            # see http://www.zetadev.com/software/aspen/
            set env_name = `basename \`dirname "$VIRTUAL_ENV"\``
        else
            set env_name = `basename "$VIRTUAL_ENV"`
        endif
    endif
    set prompt = "%{\033[1;34m%}[$env_name] %{\033[1;1;38m%}%C6%{\033[1;0m%} > "
    unset env_name
endif

alias pydoc python -m pydoc

rehash
