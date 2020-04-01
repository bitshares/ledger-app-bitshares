#!/usr/bin/env python3

VERSION="0.0.1-rc"

BUNDLE_NAME="SimpleGUIWallet"
UNIX_NAME="simpleguiwallet"
SHORT_DESCRIPTION="Super-Simple BitShares Wallet"

if __name__ == "__main__":
    import sys
    def out(s):
        sys.stdout.write(s)
        sys.stdout.flush()
        sys.exit(0)
    if ("--bundle" in sys.argv):
        out(BUNDLE_NAME)
    if ("--uname" in sys.argv):
        out(UNIX_NAME)
    out(VERSION)
