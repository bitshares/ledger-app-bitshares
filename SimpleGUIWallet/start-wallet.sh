#!/bin/bash
#
# Starts up the SimpleGUIWallet.  Ensures dependencies are met, and if
# not, installs them in a virtualenv (so as not to stomp on user's
# existing py libs).
#

function do_checks() {
    if [ ! -f ./SimpleGUIWallet.py ]; then
        echo "Must be run from the folder that contains SimpleGUIWallet.py."
        exit
    fi
    if ! which virtualenv > /dev/null; then
        echo "Package 'virtualenv' is required."
        echo "On many Linux distributions, this can be installed with:"
        echo "$ sudo apt-get install virtualenv"
        exit
    fi
}

function install_env_and_deps() {
    echo "Installing python dependencies into a subdirectory of this folder..."
    echo "(This may take a few minutes.)"
    virtualenv env > /dev/null
    source env/bin/activate
    pip3 install -r requirements.txt > /dev/null
    echo "Done. (Python packages installed into `pwd`/env/lib.)"
}

function refresh_dependencies() {
    echo "Checking dependencies..."
    pip3 install -r requirements.txt > /dev/null
}

# Check that we're in the right location and dependencies are installed:
do_checks
if [ ! -d env ]; then
    install_env_and_deps
fi
source env/bin/activate
refresh_dependencies  # (In case requirements.txt changes. Fast if no update needed.)

# Start the Wallet:
echo "Starting SimpleGUIWallet..."
# Note, if you want the wallet to "remember" your user id, replace next line with:
# python3 SimpleGUIWallet.py --user your-user-name "$@"
python3 SimpleGUIWallet.py "$@"
